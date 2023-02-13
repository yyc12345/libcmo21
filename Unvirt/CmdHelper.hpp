#pragma once

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

namespace Unvirt {
	namespace CmdHelper {

		class CmdSplitter {
		public:
			CmdSplitter();
			CmdSplitter(const CmdSplitter&) = delete;
			CmdSplitter& operator=(const CmdSplitter&) = delete;
			~CmdSplitter();

			std::vector<std::string> Convert(const std::string& u8cmd);
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

		enum class CmdArgType {
			NONE,
			STRING,
			INT
		};

		struct OptionDescription {
			std::string mLongName;
			char mShortName;
			CmdArgType mType;
			std::string mDescription;
		};

		class OptionsDescription {
		public:
			OptionsDescription();
			OptionsDescription(const OptionsDescription&) = delete;
			OptionsDescription& operator=(const OptionsDescription&) = delete;
			~OptionsDescription();

			/// <summary>
			/// Add an option
			/// </summary>
			/// <param name="fullname">The long name of this option. Should NOT be blank or NULL.</param>
			/// <param name="shortname">A single char for the short name of this option. Leave ZERO to omit this.</param>
			/// <param name="type">The value type of this option. Set to CmdArgType::NONE to indicate this is a switch (no value).</param>
			/// <param name="sescription">The description of this option. This can be NULL.</param>
			void AddOption(const char* fullname, char shortname, CmdArgType type, const char* sescription);
			void AddPositionalOption(const char* corresponding_longname);

			void PrintHelp(FILE* f);
		private:
			std::unordered_map<std::string, OptionDescription> mLongNameDict;
			std::unordered_map<char, std::string> mShortNameMapping;
			std::vector<std::string> mPositionalArgMapping;
		};

		struct AnyVariable {
			CmdArgType mType;
			void* mData;
		};

		class VariablesMap {
		private:
			std::unordered_map<std::string, AnyVariable> mDataPair;

		public:
			VariablesMap();
			~VariablesMap();

			void Clear(void);
			bool AddPair(std::string& name, CmdArgType t, std::string& val);
			bool Contain(const char* probe);
			template<typename T>
			T* GetData(const char* opt) {
				;
			}
		};

		struct CmdRegisteryEntry {
			OptionsDescription mOptDesc;
			std::function<void(OptionsDescription&, VariablesMap&)> mBindProc;
		};

		class InteractiveCmd {
		public:
			InteractiveCmd();
			~InteractiveCmd();

			void Run(void);

		private:
			bool AnalyzeCmd(std::vector<std::string>& args);
			void PrintHelp(FILE* f);
			std::unordered_map<std::string, CmdRegisteryEntry> mCmdDispatcher;


		};

	}
}