#include "IronPad.hpp"
#include <Windows.h>
#include <DbgHelp.h>
#include <filesystem>
#include <cstdarg>
#include <cstdio>

namespace IronPad {

#if defined(IRONPAD_ENABLED)

	/**
	 * @brief true if the exception handler already registered.
	 * This variable is served for singleton.
	*/
	static bool g_IsRegistered = false;
	/**
	 * @brief true if a exception handler is running.
	 * This variable is served for blocking possible infinity recursive exception handling.
	*/
	static bool g_IsProcessing = false;
	/**
	 * @brief The backup of original exception handler.
	*/
	LPTOP_LEVEL_EXCEPTION_FILTER g_ProcBackup;

#pragma region Exception Handler Detail
	

	static HMODULE GetCurrentModule() {
		// Reference: https://stackoverflow.com/questions/557081/how-do-i-get-the-hmodule-for-the-currently-executing-code
		HMODULE hModule = NULL;
		GetModuleHandleExW(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,	// get address and do not inc ref counter.
			(LPCWSTR)GetCurrentModule,
			&hModule);

		return hModule;
	}

	static const char* UExceptionGetCodeName(DWORD code) {
		switch (code) {
			case EXCEPTION_ACCESS_VIOLATION:
				return "access violation";
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				return "array index out of bound";
			case EXCEPTION_BREAKPOINT:
				return "breakpoint reached";
			case EXCEPTION_DATATYPE_MISALIGNMENT:
				return "misaligned data access";
			case EXCEPTION_FLT_DENORMAL_OPERAND:
				return "operand had denormal value";
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				return "floating-point division by zero";
			case EXCEPTION_FLT_INEXACT_RESULT:
				return "no decimal fraction representation for value";
			case EXCEPTION_FLT_INVALID_OPERATION:
				return "invalid floating-point operation";
			case EXCEPTION_FLT_OVERFLOW:
				return "floating-point overflow";
			case EXCEPTION_FLT_STACK_CHECK:
				return "floating-point stack corruption";
			case EXCEPTION_FLT_UNDERFLOW:
				return "floating-point underflow";
			case EXCEPTION_ILLEGAL_INSTRUCTION:
				return "illegal instruction";
			case EXCEPTION_IN_PAGE_ERROR:
				return "inaccessible page";
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				return "integer division by zero";
			case EXCEPTION_INT_OVERFLOW:
				return "integer overflow";
			case EXCEPTION_INVALID_DISPOSITION:
				return "documentation says this should never happen";
			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				return "can't continue after a noncontinuable exception";
			case EXCEPTION_PRIV_INSTRUCTION:
				return "attempted to execute a privileged instruction";
			case EXCEPTION_SINGLE_STEP:
				return "one instruction has been executed";
			case EXCEPTION_STACK_OVERFLOW:
				return "stack overflow";
			default:
				return "unknown exception";
		}
	}

	static void UExceptionFormat(std::FILE* fs, const char* fmt, ...) {
		// write to file
		va_list arg1;
		va_start(arg1, fmt);
		std::vfprintf(fs, fmt, arg1);
		va_end(arg1);
		// write to stdout
		va_list arg2;
		va_start(arg2, fmt);
		std::vfprintf(stdout, fmt, arg2);
		va_end(arg2);
	}

	static void UExceptionPrint(std::FILE* fs, const char* strl) {
		// write to file
		std::fputs(strl, fs);
		// write to stdout
		std::fputs(strl, stdout);
	}

	static void UExceptionBacktrace(FILE* fs, LPCONTEXT context, int maxdepth) {
		// setup loading symbol options
		SymSetOptions(SymGetOptions() | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);	// lazy load symbol, and load line number.

		// setup handle
		HANDLE process = GetCurrentProcess();
		HANDLE thread = GetCurrentThread();

		// init symbol
		if (!SymInitialize(process, 0, TRUE)) {
			// fail to load. return
			UExceptionPrint(fs, "Lost symbol file!\n");
			return;
		}

		// ========== CORE DUMP ==========
		// prepare frame. setup correct fields
		DWORD machine_type = 0;
		STACKFRAME64 frame;
		memset(&frame, 0, sizeof(frame));
#if defined(_M_IX86) || defined(__i386__)
		machine_type = IMAGE_FILE_MACHINE_I386;
		frame.AddrPC.Offset = context->Eip;
		frame.AddrStack.Offset = context->Esp;
		frame.AddrFrame.Offset = context->Ebp;
#elif defined(_M_IX64) || defined(__amd64__)
		machine_type = IMAGE_FILE_MACHINE_AMD64;
		frame.AddrPC.Offset = context->Rip;
		frame.AddrStack.Offset = context->Rsp;
		frame.AddrFrame.Offset = context->Rbp;
#else
#error "Unsupported platform"
		//IA-64 anybody?
#endif
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrStack.Mode = AddrModeFlat;
		frame.AddrFrame.Mode = AddrModeFlat;

		// other variables
		char module_name_raw[MAX_PATH];

		// stack walker
		while (StackWalk64(machine_type, process, thread, &frame, context,
			0, SymFunctionTableAccess64, SymGetModuleBase64, 0)) {

			// depth breaker
			--maxdepth;
			if (maxdepth < 0) {
				UExceptionPrint(fs, "...\n");		// indicate there are some frames not listed
				break;
			}

			// get module name
			DWORD64 module_base = SymGetModuleBase64(process, frame.AddrPC.Offset);
			const char* module_name = "[unknown module]";
			if (module_base && GetModuleFileNameA((HINSTANCE)module_base, module_name_raw, MAX_PATH)) {
				module_name = module_name_raw;
			}

			// get source file and line
			const char* source_file = "[unknow_source_file]";
			DWORD64 source_file_line = 0;
			DWORD dwDisplacement;
			IMAGEHLP_LINE64 winline;
			winline.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &dwDisplacement, &winline)) {
				source_file = winline.FileName;
				source_file_line = winline.LineNumber;
			}

			// write to file
			UExceptionFormat(fs, "0x%016llx(rel: 0x%016llx)[%s]\t%s#%llu\n",
				frame.AddrPC.Offset, frame.AddrPC.Offset - module_base, module_name,
				source_file, source_file_line
			);

		}

		// ========== END CORE DUMP ==========

		// free symbol
		SymCleanup(process);
	}

	static void UExceptionCoreDump(LPCWSTR filename, LPEXCEPTION_POINTERS info) {
		// open file and write
		HANDLE hFile = CreateFileW(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			MINIDUMP_EXCEPTION_INFORMATION exception_info;
			exception_info.ThreadId = GetCurrentThreadId();
			exception_info.ExceptionPointers = info;
			exception_info.ClientPointers = TRUE;
			MiniDumpWriteDump(
				GetCurrentProcess(), GetCurrentProcessId(), hFile,
				MiniDumpWithFullMemory,
				&exception_info,
				NULL, NULL
			);
			CloseHandle(hFile);
		}
	}

	static LONG WINAPI UExceptionImpl(LPEXCEPTION_POINTERS info) {
		// detect loop calling
		if (g_IsProcessing) {
			goto end_proc;
		}
		// start process
		g_IsProcessing = true;

		{
			// get main folder first
			std::filesystem::path ironpad_path;
			WCHAR module_path[MAX_PATH];
			std::memset(module_path, 0, sizeof(module_path));
			if (GetModuleFileNameW(GetCurrentModule(), module_path, MAX_PATH) == 0) {
				goto failed;
			}
			ironpad_path = module_path;
			ironpad_path = ironpad_path.parent_path();

			// create 2 filename
			auto logfilename = ironpad_path / "IronPad.log";
			auto dmpfilename = ironpad_path / "IronPad.dmp";
			std::fputc('\n', stdout);
			std::fprintf(stdout, "Exception Log: %s\n", logfilename.string().c_str());
			std::fprintf(stdout, "Exception Coredump: %s\n", dmpfilename.string().c_str());

			// output log file
			{
				std::FILE* fs = _wfopen(logfilename.wstring().c_str(), L"w");
				if (fs == nullptr) {
					goto failed;
				}

				// record exception type first
				PEXCEPTION_RECORD rec = info->ExceptionRecord;
				fprintf(fs, "Unhandled exception occured at 0x%08p: %s (%lu).\n",
					rec->ExceptionAddress,
					UExceptionGetCodeName(rec->ExceptionCode),
					rec->ExceptionCode
				);

				// special proc for 2 exceptions
				if (rec->ExceptionCode == EXCEPTION_ACCESS_VIOLATION || rec->ExceptionCode == EXCEPTION_IN_PAGE_ERROR) {
					if (rec->NumberParameters >= 2) {
						const char* op =
							rec->ExceptionInformation[0] == 0 ? "read" :
							rec->ExceptionInformation[0] == 1 ? "written" : "executed";
						fprintf(fs, "The data at memory address 0x%08x could not be %s.\n",
							rec->ExceptionInformation[1], op);
					}
				}

				// output stacktrace
				UExceptionBacktrace(fs, info->ContextRecord, 1024);

				std::fclose(fs);
			}

			// output minidump
			{
				UExceptionCoreDump(dmpfilename.wstring().c_str(), info);
			}

		}

		// end process
	failed:
		g_IsProcessing = false;
		// if backup proc can be run, run it
		// otherwise directly return.
	end_proc:
		if (g_ProcBackup != nullptr) {
			return g_ProcBackup(info);
		} else {
			return EXCEPTION_CONTINUE_SEARCH;
		}
	}

#pragma endregion

	void IronPadRegister() {
		if (g_IsRegistered) return;
		g_ProcBackup = SetUnhandledExceptionFilter(UExceptionImpl);
		g_IsRegistered = true;
	}

	void IronPadUnregister() {
		if (!g_IsRegistered) return;
		SetUnhandledExceptionFilter(g_ProcBackup);
		g_IsRegistered = false;
	}

#else

	// blank implement

	void IronPadRegister() {}
	void IronPadUnregister() {}

#endif

	}
