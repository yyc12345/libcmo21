#include "CmdHelper.hpp"
#include "TerminalHelper.hpp"
#include "VTUtils.hpp"
#include "VTEncoding.hpp"
#include <iostream>

/*

namespace Unvirt {
	namespace CmdHelper {

#pragma region CmdSplitter

		CmdSplitter::CmdSplitter() :
			mCmdChar(0), mBuffer(nullptr), mResult(nullptr),
			mState(StateType::NORMAL), mPreState(StateType::NORMAL) {
			;
		}
		CmdSplitter::~CmdSplitter() {
			;
		}

		const std::vector<std::string> CmdSplitter::Convert(const std::string& u8cmd) {
			// set up variables
			std::vector<std::string> result;
			std::string buffer;
			mBuffer = &buffer;
			mResult = &result;
			mState = mPreState = StateType::SPACE;

			// split
			for (auto it = u8cmd.begin(); it != u8cmd.end(); ++it) {
				mCmdChar = (*it);

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

#pragma region OptionsDescription

		OptionsDescription::OptionsDescription() :
			mLongNameDict(), mShortNameMapping(), mPositionalArgMapping() {
			;
		}
		OptionsDescription::~OptionsDescription() { ; }

		void OptionsDescription::AddOption(
			const char* fullname, char shortname, CmdArgType type, const char* description) {
			// pre-check
			if (fullname == nullptr ||
				fullname[0] == '\0' ||
				fullname[0] == '-')
				throw std::invalid_argument("Invalid Option Long Name.");

			// construct data
			std::string sfullname(fullname);
			OptionDescription data{
				fullname, shortname, type, (description != nullptr ? description : "")
			};

			// check requirement
			if (mLongNameDict.contains(sfullname)) throw std::invalid_argument("Duplicated Option Long Name.");
			if (shortname != '\0')
				if (mShortNameMapping.contains(shortname)) throw std::invalid_argument("Duplicated Option Short Name.");

			// add them
			mShortNameMapping.emplace(shortname, sfullname);
			mLongNameDict.emplace(sfullname, std::move(data));
		}

		void OptionsDescription::AddPositionalOption(const char* corresponding_longname) {
			// pre-check
			if (corresponding_longname == nullptr) throw std::invalid_argument("Invalid Option Long Name.");

			// construct data
			std::string fullname(corresponding_longname);

			// check requirement
			if (!mLongNameDict.contains(fullname)) throw std::invalid_argument("No Matched Option.");
			if (!mPositionalArgMapping.empty()) {
				for (const auto& i : mPositionalArgMapping) {
					if (i == fullname)throw std::invalid_argument("Duplicated Option.");
				}
			}

			// set value
			mPositionalArgMapping.push_back(std::move(fullname));
		}

		OptionDescription* OptionsDescription::GetDescByLongName(const std::string& longname) {
			const auto search = mLongNameDict.find(longname);
			if (search == mLongNameDict.end()) return nullptr;
			return &(*search).second;
		}
		OptionDescription* OptionsDescription::GetDescByShortName(const char shortname) {
			const auto search = mShortNameMapping.find(shortname);
			if (search == mShortNameMapping.end()) return nullptr;
			return GetDescByLongName((*search).second);
		}
		OptionDescription* OptionsDescription::GetDescByPosition(size_t pos) {
			if (pos >= mPositionalArgMapping.size()) return nullptr;
			return GetDescByLongName(mPositionalArgMapping[pos]);
		}

		void OptionsDescription::PrintHelp(FILE* f) {
			fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("Allowed Options: \n")), f);
			for (const auto& [key, value] : mLongNameDict) {
				fprintf(f, "--%s\t%s\n", value.mLongName.c_str(), value.mDescription.c_str());
			}

			if (!mPositionalArgMapping.empty()) {
				fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("\nPositional Options: \n")), f);
				for (const auto& i : mPositionalArgMapping) {
					fprintf(f, "[%s] ", i.c_str());
				}
			}
		}


#pragma endregion

#pragma region VariablesMap

		VariablesMap::VariablesMap() : mDataPair() {
			;
		}
		VariablesMap::~VariablesMap() {
			this->Clear();
		}

		void VariablesMap::Clear(void) {
			for (const auto& [key, value] : mDataPair) {
				if (value.mData != nullptr) free(value.mData);
			}
			mDataPair.clear();
		}

		bool VariablesMap::AddPair(const std::string& name, CmdArgType t, const std::string& val) {
			if (mDataPair.contains(name)) return false;

			AnyVariable var;
			switch (t) {
				case Unvirt::CmdHelper::CmdArgType::NONE:
				{
					var.mDataBasicSize = 1;
					var.mData = nullptr;
					break;
				}
				case Unvirt::CmdHelper::CmdArgType::STRING:
				{
					var.mDataBasicSize = sizeof(char);
					var.mData = malloc(val.size() + 1);
					if (var.mData == nullptr) break;

					memcpy(var.mData, val.c_str(), val.size() + 1);
					break;
				}
				case Unvirt::CmdHelper::CmdArgType::INT:
				{
					var.mDataBasicSize = sizeof(int);
					var.mData = malloc(sizeof(int));
					if (var.mData == nullptr) break;

					char* pend = nullptr;
					errno = 0;
					int64_t v = std::strtoll(val.c_str(), &pend, 10);

					if (pend == val.c_str() || errno == ERANGE) v = INT64_C(0);
					*((int*)var.mData) = static_cast<int>(v);
					break;
				}
				default:
					throw std::invalid_argument("Invalid Option Type.");
			}

			mDataPair.emplace(name, std::move(var));
			return true;
		}

#pragma endregion

#pragma region ExecEnvironment

		ExecEnvironment::ExecEnvironment() :
			mVtFile(nullptr), mVtFileEnv(nullptr) {
			;
		}

		ExecEnvironment::~ExecEnvironment() {
			if (mVtFile != nullptr) delete mVtFile;
			if (mVtFileEnv != nullptr) delete mVtFileEnv;
		}

		void ExecEnvironment::ProcLoad(OptionsDescription& od, VariablesMap& vm) {
			if (mVtFile != nullptr || mVtFileEnv != nullptr) {
				printf(UNVIRT_TERMCOL_LIGHT_RED(("Please close current opened Vrtools file first.\n")));
				return;
			}

			const char* filename = vm.GetData<char>("file");
			if (filename == nullptr) {
				printf(UNVIRT_TERMCOL_LIGHT_RED(("You should specify a file first.\n")));
				od.PrintHelp(stdout);
				return;
			}

			mVtFileEnv = new LibCmo::Utils::VirtoolsContext();
			const char* enc = vm.GetData<char>("encoding");
			mVtFileEnv->NameEncoding = enc == nullptr ? "" : enc;

			mVtFile = new LibCmo::CKFile(*mVtFileEnv);
			mVtFile->Load(filename, LibCmo::CK_LOAD_FLAGS::CK_LOAD_DEFAULT);
		}

		void ExecEnvironment::ProcInfo(OptionsDescription& od, VariablesMap& vm) {
			printf(UNVIRT_TERMCOL_LIGHT_RED(("Sorry. This feature is not supported now.\n")));
		}

		void ExecEnvironment::ProcClear(OptionsDescription& od, VariablesMap& vm) {
			if (mVtFile == nullptr && mVtFileEnv == nullptr) {
				printf(UNVIRT_TERMCOL_LIGHT_RED(("Virtools file already is empty.\n")));
				return;
			}

			if (mVtFile != nullptr) delete mVtFile;
			if (mVtFileEnv != nullptr) delete mVtFileEnv;

			mVtFile = nullptr;
			mVtFileEnv = nullptr;
		}

		void ExecEnvironment::ProcExportSql(OptionsDescription& od, VariablesMap& vm) {
			printf(UNVIRT_TERMCOL_LIGHT_RED(("Sorry. This feature is not supported now.\n")));
		}


#pragma endregion

#pragma region InteractiveCmd

		InteractiveCmd::InteractiveCmd() :
			mCmdDispatcher(), mExecEnv(), mVm(), mBlank(), mExitRunFlag(false), mCmdSplitter() {
			// add load subcommand
			CmdRegisteryEntry entryLoad;
			const std::string entryLoadName = "load";
			entryLoad.mSubCmdDesc = "Load Virtools file.";
			entryLoad.mOptDesc.AddOption("file", 'f', CmdArgType::STRING, "The loaded Virtools file.");
			entryLoad.mOptDesc.AddPositionalOption("file");
			entryLoad.mOptDesc.AddOption("encoding", 'e', CmdArgType::STRING, "The encoding to decode Virtools string.");
			entryLoad.mOptDesc.AddPositionalOption("encoding");
			entryLoad.mOptDesc.AddOption("temp", 't', CmdArgType::STRING, "The temp folder used by engine.");
			entryLoad.mOptDesc.AddPositionalOption("temp");
			entryLoad.mBindProc = std::bind(&ExecEnvironment::ProcLoad, &this->mExecEnv, std::placeholders::_1, std::placeholders::_2);
			//mCmdDispatcher.emplace("load", std::move(entryLoad));

			CmdRegisteryEntry entryInfo;
			entryInfo.mSubCmdDesc = "Show loaded Virtools file header info.";
			entryInfo.mBindProc = std::bind(&ExecEnvironment::ProcInfo, &this->mExecEnv, std::placeholders::_1, std::placeholders::_2);
			//mCmdDispatcher.emplace("info", std::move(entryInfo));

			CmdRegisteryEntry entryClear;
			entryClear.mSubCmdDesc = "Clear current loaded Virtools file.";
			entryClear.mBindProc = std::bind(&ExecEnvironment::ProcClear, &this->mExecEnv, std::placeholders::_1, std::placeholders::_2);
			//mCmdDispatcher.emplace("clear", std::move(entryClear));

			CmdRegisteryEntry entryExportSql;
			entryExportSql.mSubCmdDesc = "Export loaded Virtools file as a SQList database file.";
			entryExportSql.mOptDesc.AddOption("file", 'f', CmdArgType::STRING, "The exported SQL file.");
			entryExportSql.mOptDesc.AddPositionalOption("file");
			entryExportSql.mBindProc = std::bind(&ExecEnvironment::ProcExportSql, &this->mExecEnv, std::placeholders::_1, std::placeholders::_2);
			//mCmdDispatcher.emplace("sql", std::move(entryExportSql));

			CmdRegisteryEntry entryExit;
			entryExit.mSubCmdDesc = "Exit this interactive commander.";
			entryExit.mBindProc = std::bind(&InteractiveCmd::ProcExit, this, std::placeholders::_1, std::placeholders::_2);
			//mCmdDispatcher.emplace("exit", std::move(entryExit));

		}

		InteractiveCmd::~InteractiveCmd() {
			;
		}

		void InteractiveCmd::Run(void) {
			std::string u8cmd;

			mExitRunFlag = false;
			while (!mExitRunFlag) {
				// get command
				GetCmdLine(u8cmd);

				// split cmd and parse it
				CmdParser(mCmdSplitter.Convert(u8cmd));
			}
		}

		void InteractiveCmd::GetCmdLine(std::string& u8cmd) {
#if defined(LIBCMO_OS_WIN32)
			std::wstring wcmd;
			std::getline(std::wcin, wcmd);
			LibCmo::Encoding::WcharToChar(wcmd, u8cmd, CP_UTF8);
#else
			std::getline(std::cin, u8cmd);
#endif
		}

		void InteractiveCmd::CmdParser(const std::vector<std::string>& args) {
			FILE* f = stdout;

			if (args.size() == 0) {
				fputs(UNVIRT_TERMCOL_LIGHT_RED(("Error! Fail to get subcommand token.\n")), f);
				PrintHelp(f);
				return;
			}

			auto arg = args.begin();
			auto subcmd = mCmdDispatcher.find(*arg);
			if (subcmd == mCmdDispatcher.end()) {
				fprintf(f, UNVIRT_TERMCOL_LIGHT_RED(("Error! No such subcommand \"%s\"! \n")), arg->c_str());
				PrintHelp(f);
				return;
			}

			// analyze options
			++arg;
			auto& optsDesc = subcmd->second.mOptDesc;
			mVm.Clear();
			int position_counter = 0;
			while (true) {
				if (arg == args.end()) break;

				const std::string& opt = *arg;
				OptionDescription* optDesc;
				if (opt.starts_with("--")) {
					// long name
					optDesc = optsDesc.GetDescByLongName(opt.substr(2));
				} else if (opt.starts_with("-")) {
					// short name
					if (opt.size() != 2u) {
						// invalid short name
						fprintf(f, UNVIRT_TERMCOL_LIGHT_RED(("Error! Invalid short name option \"%s\"! \n")), opt.c_str());
						optsDesc.PrintHelp(f);
						return;
					}
					optDesc = optsDesc.GetDescByShortName(opt[1]);
				} else {
					// position
					optDesc = optsDesc.GetDescByPosition(position_counter++);
				}

				// invalid option
				if (optDesc == nullptr) {
					fprintf(f, UNVIRT_TERMCOL_LIGHT_RED(("Error! Invalid option \"%s\"! \n")), opt.c_str());
					optsDesc.PrintHelp(f);
					return;
				}

				// get value
				bool add_success = true;
				switch (optDesc->mType) {
					case CmdArgType::NONE:
						// just a switch
						add_success = mVm.AddPair(optDesc->mLongName, optDesc->mType, this->mBlank);
						++arg;
						if (!add_success) {
							fprintf(f, UNVIRT_TERMCOL_LIGHT_RED(("Error!  Duplicated option \"%s\"! \n")), opt.c_str());
							optsDesc.PrintHelp(f);
							return;
						}
						break;
					case CmdArgType::INT:
					case CmdArgType::STRING:
						// check next value
						++arg;
						if (arg == args.end()) {
							fprintf(f, UNVIRT_TERMCOL_LIGHT_RED(("Error! Option \"%s\" lost parameter! \n")), opt.c_str());
							optsDesc.PrintHelp(f);
							return;
						}
						add_success = mVm.AddPair(optDesc->mLongName, optDesc->mType, *arg);
						++arg;
						if (!add_success) {
							fprintf(f, UNVIRT_TERMCOL_LIGHT_RED(("Error!  Duplicated option \"%s\"! \n")), opt.c_str());
							optsDesc.PrintHelp(f);
							return;
						}
						break;
					default:
						throw std::invalid_argument("Invalid Option Type.");
				}
			}

			// execute proc
			subcmd->second.mBindProc(optsDesc, mVm);
		}

		void InteractiveCmd::PrintHelp(FILE* f) {
			fputs(UNVIRT_TERMCOL_LIGHT_YELLOW(("Allowed Subcommands: \n")), f);
			for (const auto& [key, value] : mCmdDispatcher) {
				fprintf(f, "%s\t- %s\n", key.c_str(), value.mSubCmdDesc.c_str());
			}
		}

		void InteractiveCmd::ProcExit(OptionsDescription&, VariablesMap&) {
			mExitRunFlag = true;
		}

#pragma endregion

		}
	}

*/