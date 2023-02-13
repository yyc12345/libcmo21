#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>

namespace Unvirt {
	namespace CmdHelper {

		class CmdSplitter {
		public:
			CmdSplitter();
			CmdSplitter(const CmdSplitter&) = delete;
			CmdSplitter& operator=(const CmdSplitter&) = delete;
			~CmdSplitter();

			std::vector<std::string> Convert(std::string& u8cmd);
		private:
			char mCmdChar;
			std::string* mBuffer;
			std::vector<std::string>* mResult;

			enum class StateType : int {
				SPACE,
				SINGLE,
				DOUBLE,
				ESCAPE,
				NORMAL
			};
			StateType mState, mPreState;

			inline void ProcSpace(void) {
				switch (mCmdChar) {
					case '\'':
						mState = StateType::SINGLE;
						break;
					case '"':
						mState = StateType::DOUBLE;
						break;
					case '\\':
						mState = StateType::ESCAPE;
						mPreState = StateType::NORMAL;
						break;
					case ' ':
						break;	// skip blank
					default:
						mBuffer->push_back(mCmdChar);
						mState = StateType::NORMAL;
						break;
				}
			}
			inline void ProcSingle(void) {
				switch (mCmdChar) {
					case '\'':
						mState = StateType::NORMAL;
						break;
					case '"':
						mBuffer->push_back('"');
						break;
					case '\\':
						mState = StateType::ESCAPE;
						mPreState = StateType::SINGLE;
						break;
					case ' ':
						mBuffer->push_back(' ');
						break;
					default:
						mBuffer->push_back(mCmdChar);
						break;
				}
			}
			inline void ProcDouble(void) {
				switch (mCmdChar) {
					case '\'':
						mBuffer->push_back('\'');
						break;
					case '"':
						mState = StateType::NORMAL;
						break;
					case '\\':
						mState = StateType::ESCAPE;
						mPreState = StateType::DOUBLE;
						break;
					case ' ':
						mBuffer->push_back(' ');
						break;
					default:
						mBuffer->push_back(mCmdChar);
						break;
				}
			}
			inline void ProcEscape(void) {
				// add itself
				mBuffer->push_back(mCmdChar);
				// restore state
				mState = mPreState;
			}
			inline void ProcNormal(void) {
				switch (mCmdChar) {
					case '\'':
						mBuffer->push_back('\'');
						break;
					case '"':
						mBuffer->push_back('"');
						break;
					case '\\':
						mState = StateType::ESCAPE;
						mPreState = StateType::NORMAL;
						break;
					case ' ':
						mResult->push_back(*mBuffer);
						mBuffer->clear();
						mState = StateType::SPACE;
						break;
					default:
						mBuffer->push_back(mCmdChar);
						break;
				}
			}
		};

		class SubCmd {
		public:
			SubCmd();
			~SubCmd();

		private:

		};

		class InteractiveCmd {
		public:
			InteractiveCmd();
			~InteractiveCmd();

			void Run(void);

		private:
			//std::unordered_map<std::string, CmdRegisteryEntry> mCmdDispatcher;


		};

	}
}