#include "CmdHelper.hpp"

namespace Unvirt::CmdHelper {

#pragma region CmdSplitter

	std::deque<std::string> CmdSplitter::Convert(const std::string& u8cmd) {
		// set up variables
		std::deque<std::string> result;
		std::string buffer;
		mBuffer = &buffer;
		mResult = &result;
		mState = mPreState = StateType::SPACE;

		// split
		for (auto& c : u8cmd) {
			mCmdChar = c;

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

	void CmdSplitter::ProcSpace(void) {
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
	void CmdSplitter::ProcSingle(void) {
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
	void CmdSplitter::ProcDouble(void) {
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
	void CmdSplitter::ProcEscape(void) {
		// add itself
		mBuffer->push_back(mCmdChar);
		// restore state
		mState = mPreState;
	}
	void CmdSplitter::ProcNormal(void) {
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
#pragma endregion

#pragma region Abstract Node

	AbstractNode* AbstractNode::Then(AbstractNode* node) {
		// check literal duplication
		for (auto& pnode : m_Nodes) {
			if (pnode->IsConflictWith(node))
				throw std::invalid_argument("conflict node.");
		}
		// add into list
		m_Nodes.emplace_back(node);
		return this;
	}

	AbstractNode* AbstractNode::Executes(ExecutionFct fct, const char* cmt) {
		if (m_Execution != nullptr)
			throw std::invalid_argument("duplicated executions.");
		m_Execution = fct;
		m_Comment = cmt;
		return this;
	}

	void AbstractNode::Help(HelpDocument& doc) {
		// add self
		BeginHelp(doc);

		// check terminal
		if (m_Execution != nullptr) {
			doc.Terminate(m_Comment);
		}

		// iterate children
		for (auto& pnode : m_Nodes) {
			pnode->Help(doc);
		}

		// pop self
		EndHelp(doc);
	}

	bool AbstractNode::Consume(std::deque<std::string>& arglist, ArgumentsMap& argmap) {
		// if no data can consume, return
		if (arglist.empty()) return false;

		// backup current value
		std::string cur = arglist.front();
		// consume self
		if (!BeginAccept(cur, argmap)) {
			// fail to consume self. not matched. return
			return false;
		}

		// pop front for following code
		arglist.pop_front();

#define CONSUME_DEFER \
	arglist.push_front(cur); \
	EndAccept(argmap);

		if (arglist.empty()) {
			// this is must be a terminal.
			// check whether we have execution.
			if (m_Execution == nullptr) {
				CONSUME_DEFER;
				return false;
			} else {
				m_Execution(argmap);
				CONSUME_DEFER;
				return true;
			}
		} else {
			// have following command, try match them
			// iterate literal and argument to check terminal
			for (auto& pnode : m_Nodes) {
				if (pnode->Consume(arglist, argmap)) {
					CONSUME_DEFER;
					return true;
				}
			}

			// if still nothing to match, return false
			CONSUME_DEFER;
			return false;
		}

#undef CONSUME_DEFER

	}

#pragma endregion

#pragma region Argument Impl

	bool IntArgument::BeginParse(const std::string& val) {
		char* pend = nullptr;
		errno = 0;
		int64_t v = std::strtoll(val.c_str(), &pend, 10);

		if (pend == val.c_str() || errno == ERANGE) return false;

		// check limit
		int32_t value = static_cast<int32_t>(v);
		if (m_IntLimit != nullptr && !m_IntLimit(value)) {
			return false;
		}

		m_ParsedData = new int32_t(value);
		return true;
	}

	void IntArgument::EndParse() {
		delete reinterpret_cast<int32_t*>(m_ParsedData);
		m_ParsedData = nullptr;
	}

	bool StringArgument::BeginParse(const std::string& strl) {
		// string always accept every text
		m_ParsedData = new std::string(strl);
		return true;
	}

	void StringArgument::EndParse() {
		delete reinterpret_cast<std::string*>(m_ParsedData);
		m_ParsedData = nullptr;
	}

#pragma endregion

#pragma region Command Root

	bool CommandRoot::RootConsume(std::deque<std::string>& arglist) {
		// if no data can consume, return
		if (arglist.empty()) return false;

		// create a argument map
		ArgumentsMap amap;

		// and we only just need iterate all children
		for (auto& pnode : m_Nodes) {
			if (pnode->Consume(arglist, amap)) {
				return true;
			}
		}

		// no matched
		return false;
	}

	HelpDocument* CommandRoot::RootHelp() {
		HelpDocument* doc = new HelpDocument();

		// we only just need iterate all children
		for (auto& pnode : m_Nodes) {
			pnode->Help(*doc);
		}

		return doc;
	}

#pragma endregion

#pragma region Help Document

	void HelpDocument::Terminate(std::string& command_desc) {
		// create new result and copy stack
		ResultItem result(command_desc);
		result.m_ArgDesc.insert(result.m_ArgDesc.end(), m_Stack.begin(), m_Stack.end());
		// add into result
		m_Results.emplace_back(std::move(result));
	}

	void HelpDocument::Print() {
		fputs("Command Help:\n", stdout);

		for (auto& item : m_Results) {
			for (auto& cmd : item.m_ArgDesc) {
				fputs(cmd.m_Name.c_str(), stdout);
				fputc(' ', stdout);
			}
			fputc('\n', stdout);

			if (!item.m_CmdDesc.empty()) {
				fputs(item.m_CmdDesc.c_str(), stdout);
				fputc('\n', stdout);
			}

			for (auto& cmd : item.m_ArgDesc) {
				if (!cmd.m_Desc.empty()) {
					fprintf(stdout, "\t%s: %s\n", cmd.m_Name.c_str(), cmd.m_Desc.c_str());
				}
			}

			fputc('\n', stdout);
		}
	}

#pragma endregion


}
