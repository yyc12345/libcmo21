#include "CmdHelper.hpp"
#include "TerminalHelper.hpp"
#include <stdexcept>

namespace Unvirt {
	namespace CmdHelper {

#pragma region cmd splitter

		CmdSplitter::CmdSplitter() :
			mCmdChar(0), mBuffer(nullptr), mResult(nullptr),
			mState(StateType::NORMAL), mPreState(StateType::NORMAL) {
			;
		}
		CmdSplitter::~CmdSplitter() {
			;
		}

		std::vector<std::string> CmdSplitter::Convert(const std::string& u8cmd) {
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

#pragma region interactive cmd helper classes


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


		VariablesMap::VariablesMap() {
		}

		VariablesMap::~VariablesMap() {
		}


#pragma endregion


#pragma region interactive cmd

		InteractiveCmd::InteractiveCmd() {

		}

		InteractiveCmd::~InteractiveCmd() {

		}

		void InteractiveCmd::Run(void) {

		}

#pragma endregion



	}
}