#pragma once

#include <VTUtils.hpp>
#include <CKDefines.hpp>
#include <string>
#include <vector>
#include <deque>

namespace Unvirt::CmdHelper {

	class CmdSplitter {
	public:
		CmdSplitter();
		CmdSplitter(const CmdSplitter&) = delete;
		CmdSplitter& operator=(const CmdSplitter&) = delete;
		~CmdSplitter();

		const std::deque<std::string> Convert(const std::string& u8cmd);
	private:
		char mCmdChar;
		std::string* mBuffer;
		std::deque<std::string>* mResult;

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

	class ArgParser {
	public:
		ArgParser() {}
		ArgParser(const ArgParser&) = delete;
		ArgParser& operator=(const ArgParser&) = delete;
		~ArgParser() {}

		static bool ParseInt(const std::deque<std::string>& cmd, const size_t expected_index, int32_t& result);
		static bool ParseString(const std::deque<std::string>& cmd, const size_t expected_index, std::string& result);
		static bool ParseSwitch(const std::deque<std::string>& cmd, const size_t expected_index, const std::vector<std::string>& switches, std::string& gotten);
	};

	class InteractiveCmd {
	public:
		InteractiveCmd();
		InteractiveCmd(const InteractiveCmd&) = delete;
		InteractiveCmd& operator=(const InteractiveCmd&) = delete;
		~InteractiveCmd();

		void Run(void);

	private:
		void GetCmdLine(std::string& u8cmd);
		void PrintHelp(void);
		void PrintArgParseError(const std::deque<std::string>& cmd, size_t pos);
		void PrintCommonError(const char* u8_fmt, ...);

		void ProcLoad(const std::deque<std::string>& cmd);
		void ProcUnLoad(const std::deque<std::string>& cmd);
		void ProcInfo(const std::deque<std::string>& cmd);
		void ProcLs(const std::deque<std::string>& cmd);
		void ProcItems(const std::deque<std::string>& cmd);
		void ProcEncoding(const std::deque<std::string>& cmd);
		void ProcTemp(const std::deque<std::string>& cmd);

		bool HasOpenedFile(void);
		void ClearDocument(void);
		void PrintMinContextMsg(const std::string& msg);

		CmdSplitter m_CmdSplitter;
		size_t m_PageLen;
		LibCmo::CK2::CKMinContext* m_Ctx;
		LibCmo::CK2::CKFile* m_File;
		LibCmo::CK2::CKFileDocument* m_Doc;
	};

}