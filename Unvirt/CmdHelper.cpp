#include "CmdHelper.hpp"
#include "TerminalHelper.hpp"
#include "StructFormatter.hpp"
#include "AccessibleValue.hpp"

#include <CKMinContext.hpp>
#include <CKFile.hpp>

#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <functional>

namespace Unvirt::CmdHelper {

	static FILE* fout = stdout;

#pragma region CmdSplitter

	CmdSplitter::CmdSplitter() :
		mCmdChar(0), mBuffer(nullptr), mResult(nullptr),
		mState(StateType::NORMAL), mPreState(StateType::NORMAL) {
		;
	}
	CmdSplitter::~CmdSplitter() {
		;
	}

	const std::deque<std::string> CmdSplitter::Convert(const std::string& u8cmd) {
		// set up variables
		std::deque<std::string> result;
		std::string buffer;
		mBuffer = &buffer;
		mResult = &result;
		mState = mPreState = StateType::SPACE;

		// split
		for (auto it = u8cmd.begin(); it != u8cmd.end(); ++it) {
			mCmdChar = (*it);

			// skip all invalid characters, \0 and etc.
			// mCmdChar >= 0 to ensure all non-ASCII UTF8 char can be accepted directly.
			if (mCmdChar >= 0 && (!std::isprint(mCmdChar)))
				continue;

			switch (mState) {
				case StateType::SPACE:
					ProcSpace();
					break;
				case StateType::SINGLE:
					ProcSingle();
					break;
				case StateType::DOUBLE:
					ProcDouble();
					break;
				case StateType::ESCAPE:
					ProcEscape();
					break;
				case StateType::NORMAL:
					ProcNormal();
					break;
			}
		}

		// final proc
		switch (mState) {
			case StateType::SPACE:
				break;
			case StateType::NORMAL:
				// push the last one
				mResult->push_back(*mBuffer);
				break;
			case StateType::SINGLE:
			case StateType::DOUBLE:
			case StateType::ESCAPE:
				// error
				result.clear();
				break;
		}

		// return value
		return result;
	}

#pragma endregion

#pragma region ArgParser

	bool ArgParser::ParseInt(const std::deque<std::string>& cmd, const size_t expected_index, int32_t& result) {
		if (expected_index >= cmd.size()) {
			result = 0;
			return false;
		}

		char* pend = nullptr;
		errno = 0;
		int64_t v = std::strtoll(cmd[expected_index].c_str(), &pend, 10);

		if (pend == cmd[expected_index].c_str() || errno == ERANGE) {
			result = 0;
			return false;
		}

		result = static_cast<int>(v);
		return true;
	}

	bool ArgParser::ParseString(const std::deque<std::string>& cmd, const size_t expected_index, std::string& result) {
		if (expected_index >= cmd.size()) {
			result.clear();
			return false;
		} else {
			result = cmd[expected_index];
			return true;
		}
	}

	bool ArgParser::ParseSwitch(const std::deque<std::string>& cmd, const size_t expected_index, const std::vector<std::string>& switches, std::string& gotten) {
		if (expected_index >= cmd.size()) {
			gotten.clear();
			return false;
		}

		for (const auto& sw : switches) {
			if (cmd[expected_index] == sw) {
				gotten = cmd[expected_index];
				return true;
			}
		}

		gotten.clear();
		return false;
	}

#pragma endregion

#pragma region InteractiveCmd Misc

	InteractiveCmd::InteractiveCmd() :
		m_CmdSplitter(), m_PageLen(10),
		m_Ctx(nullptr), m_File(nullptr), m_Doc(nullptr) {

		// create context and file
		m_Ctx = new LibCmo::CK2::CKMinContext();
		m_File = new LibCmo::CK2::CKFile(m_Ctx);

		// bind callback
		m_Ctx->SetPrintCallback(std::bind(&InteractiveCmd::PrintMinContextMsg, this, std::placeholders::_1));

	}

	InteractiveCmd::~InteractiveCmd() {
		// delete doc if necessary
		if (m_Doc != nullptr) delete m_Doc;
		// delete file and ctx
		delete m_File;
		delete m_Ctx;
	}

	bool InteractiveCmd::HasOpenedFile(void) {
		return m_Doc != nullptr;
	}

	void InteractiveCmd::ClearDocument(void) {
		if (m_Doc == nullptr) return;

		// clear doc
		delete m_Doc;
		m_Doc = nullptr;

		// clear all loaded objects
		m_Ctx->ClearCKObject();
	}

	void InteractiveCmd::PrintMinContextMsg(const std::string& msg) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("[CKMinContext] ")), fout);
		fputs(msg.c_str(), fout);
		fputc('\n', fout);
	}

#pragma endregion

#pragma region InteractiveCmd Dispatch

	void InteractiveCmd::Run(void) {
		std::string u8cmd;

		while (true) {
			// get command
			GetCmdLine(u8cmd);

			// split cmd and parse it
			auto cmds = m_CmdSplitter.Convert(u8cmd);

			// get sub command
			if (cmds.size() < 1u) {
				this->PrintCommonError("No command specified!");
				this->PrintHelp();
				continue;
			}
			std::string subcmd(cmds.front());
			cmds.pop_front();

			// dispatch command
			if (subcmd == "load") this->ProcLoad(cmds);
			else if (subcmd == "unload") this->ProcUnLoad(cmds);
			else if (subcmd == "info") this->ProcInfo(cmds);
			else if (subcmd == "ls") this->ProcLs(cmds);
			else if (subcmd == "items") this->ProcItems(cmds);
			else if (subcmd == "encoding") this->ProcEncoding(cmds);
			else if (subcmd == "temp") this->ProcTemp(cmds);
			else if (subcmd == "help") this->PrintHelp();
			else if (subcmd == "exit") break;
			else {
				this->PrintCommonError("No such command \"%s\".", subcmd.c_str());
				this->PrintHelp();
			}
			
		}
	}

	void InteractiveCmd::GetCmdLine(std::string& u8cmd) {
		fputs("Unvirt> ", fout);
#if defined(LIBCMO_OS_WIN32)
		std::wstring wcmd;
		std::getline(std::wcin, wcmd);
		LibCmo::EncodingHelper::WcharToChar(wcmd, u8cmd, CP_UTF8);
#else
		std::getline(std::cin, u8cmd);
#endif
	}

	void InteractiveCmd::PrintHelp(void) {
		fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("Allowed Subcommands: \n")), fout);

		fputs("load\n", fout);
		fputs("\tDescription: Load a Virtools composition.\n", fout);
		fputs("\tSyntax: load <file path>\n", fout);

		fputs("unload\n", fout);
		fputs("\tDescription: Release loaded Virtools composition.\n", fout);
		fputs("\tSyntax: unload\n", fout);

		fputs("info\n", fout);
		fputs("\tDescription: Show the header info of loaded Virtools composition.\n", fout);
		fputs("\tSyntax: info\n", fout);

		fputs("ls\n", fout);
		fputs("\tDescription: List something about loaded Virtools composition.\n", fout);
		fputs("\tSyntax: ls <obj | mgr> [page]\n", fout);

		fputs("items\n", fout);
		fputs("\tDescription: Set up how many items should be listed in one page when using \"ls\" command.\n", fout);
		fputs("\tSyntax: items <num>\n", fout);

		fputs("encoding\n", fout);
		fputs("\tDescription: Set the encoding series for CKMinContext.\n", fout);
		fputs("\tSyntax: encoding [encoding name1] [encoding name2] [encoding name3] ...\n", fout);

		fputs("temp\n", fout);
		fputs("\tDescription: Set the Temp path for CKMinContext.\n", fout);
		fputs("\tSyntax: temp <temp path>\n", fout);

		fputs("exit\n", fout);
		fputs("\tDescription: Exit program\n", fout);
		fputs("\tSyntax: exit\n", fout);

	}

	void InteractiveCmd::PrintArgParseError(const std::deque<std::string>& cmd, size_t pos) {
		if (pos >= cmd.size()) {
			fprintf(fout, UNVIRT_TERMCOL_LIGHT_RED(("Lost argument at position %zu.\n")), pos);
		} else {
			fprintf(fout, UNVIRT_TERMCOL_LIGHT_RED(("Unexpected argument \"%s\".\n")), cmd[pos].c_str());
		}

		// arg error always print help
		this->PrintHelp();
	}

	void InteractiveCmd::PrintCommonError(const char* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		std::fputs(UNVIRT_TERMCOLHDR_LIGHT_RED, fout);
		std::vfprintf(fout, u8_fmt, argptr);
		std::fputs(UNVIRT_TERMCOLTAIL, fout);
		va_end(argptr);
		std::fputc('\n', fout);
	}


#pragma endregion

#pragma region InteractiveCmd Processors

	void InteractiveCmd::ProcLoad(const std::deque<std::string>& cmd) {
		// check pre-requirement
		if (HasOpenedFile()) {
			this->PrintCommonError("Already have a opened file. Close it before calling \"load\".");
			return;
		}

		// check requirement
		size_t pos = 0u;
		std::string filepath;
		if (!ArgParser::ParseString(cmd, pos, filepath)) {
			this->PrintArgParseError(cmd, pos);
			return;
		}

		// proc
		m_Doc = new LibCmo::CK2::CKFileDocument();
		LibCmo::CK2::CKERROR err = m_File->DeepLoad(filepath.c_str(), &m_Doc);
		if (err != LibCmo::CK2::CKERROR::CKERR_OK) {
			// fail to load. release all.
			this->PrintCommonError("Fail to open file. Function return: %s\n%s",
				Unvirt::AccessibleValue::GetCkErrorName(err).c_str(),
				Unvirt::AccessibleValue::GetCkErrorDescription(err).c_str()
			);

			this->ClearDocument();
		}
	}

	void  InteractiveCmd::ProcUnLoad(const std::deque<std::string>& cmd) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			this->PrintCommonError("No loaded file.");
			return;
		}

		// free all
		this->ClearDocument();
	}

	void  InteractiveCmd::ProcInfo(const std::deque<std::string>& cmd) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			this->PrintCommonError("No loaded file.");
			return;
		}

		// print
		Unvirt::StructFormatter::PrintCKFileInfo(m_Doc->m_FileInfo);
	}

	void  InteractiveCmd::ProcLs(const std::deque<std::string>& cmd) {
		// static values of switches
		static const std::vector<std::string> c_AllowedSwitches{
			"obj", "mgr"
		};

		// check pre-requirement
		if (!HasOpenedFile()) {
			this->PrintCommonError("No loaded file.");
			return;
		}

		// check requirement
		size_t pos = 0u;
		std::string sw;
		if (!ArgParser::ParseSwitch(cmd, pos, c_AllowedSwitches, sw)) {
			this->PrintArgParseError(cmd, pos);
			return;
		}
		++pos;
		int32_t gotten_page;
		if (!ArgParser::ParseInt(cmd, pos, gotten_page)) {
			gotten_page = 1;	// asssume as 1
		}
		if (gotten_page <= 0) {
			this->PrintCommonError("Page out of range.");
			return;
		}
		size_t page = static_cast<size_t>(gotten_page) - 1;

		// show list
		if (sw == c_AllowedSwitches[0]) {
			// obj list
			if (page * this->m_PageLen >= m_Doc->m_FileObjects.size()) {
				this->PrintCommonError("Page out of range.");
				return;
			}

			Unvirt::StructFormatter::PrintObjectList(this->m_Doc->m_FileObjects, page, this->m_PageLen);

		} else {
			// mgr list
			if (page * this->m_PageLen >= m_Doc->m_FileManagersData.size()) {
				this->PrintCommonError("Page out of range.");
				return;
			}

			Unvirt::StructFormatter::PrintManagerList(this->m_Doc->m_FileManagersData, page, this->m_PageLen);
		}

	}

	void InteractiveCmd::ProcItems(const std::deque<std::string>& cmd) {
		// check requirement
		size_t pos = 0u;
		int32_t count;
		if (!ArgParser::ParseInt(cmd, pos, count) || count <= 0) {
			this->PrintArgParseError(cmd, pos);
			return;
		}

		// assign
		m_PageLen = static_cast<size_t>(count);
	}

	void InteractiveCmd::ProcEncoding(const std::deque<std::string>& cmd) {
		// create list first
		std::vector<std::string> encoding_list;

		// get list item
		size_t pos = 0u;
		while (true) {
			std::string pending;
			if (!ArgParser::ParseString(cmd, pos, pending)) {
				break;	// no more encoding, break
			}

			// add and move to next
			++pos;
			encoding_list.push_back(std::move(pending));
		}

		// apply list
		this->m_Ctx->SetEncoding(encoding_list);
	}

	void InteractiveCmd::ProcTemp(const std::deque<std::string>& cmd) {
		// check requirement
		size_t pos = 0u;
		std::string temppath;
		if (!ArgParser::ParseString(cmd, pos, temppath)) {
			this->PrintArgParseError(cmd, pos);
			return;
		}

		// assign
		m_Ctx->SetTempPath(temppath.c_str());
	}

#pragma endregion

}
