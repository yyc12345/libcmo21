#include "CmdHelper.hpp"

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

		std::vector<std::string> CmdSplitter::Convert(std::string& u8cmd) {
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

		SubCmd::SubCmd() {
		}

		SubCmd::~SubCmd() {
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