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

#pragma region Help Document

	HelpDocument::HelpDocument() : m_Stack(), m_Results() {}

	HelpDocument::~HelpDocument() {}

	void HelpDocument::Push(const std::string& arg_name, const std::string& arg_desc) {
		m_Stack.emplace_back(StackItem { arg_name, arg_desc });
	}

	void HelpDocument::Pop() {
		m_Stack.pop_back();
	}

	void HelpDocument::Terminate(std::string& command_desc) {
		// create new result and copy stack
		ResultItem result(command_desc);
		result.m_ArgDesc.insert(result.m_ArgDesc.end(), m_Stack.begin(), m_Stack.end());
		// add into result
		m_Results.emplace_back(std::move(result));
	}

	void HelpDocument::Print() {
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

#pragma region Abstract Node

	AbstractNode::AbstractNode() :
		m_Execution(nullptr), m_Comment(),
		m_Literals(), m_Choices(), m_Args() {}

	AbstractNode::~AbstractNode() {
		for (auto& ptr : m_Literals) {
			delete ptr;
		}
		for (auto& ptr : m_Choices) {
			delete ptr;
		}
		for (auto& ptr : m_Args) {
			delete ptr;
		}
	}

	AbstractNode* AbstractNode::Then(AbstractNode* node) {
		// check conflict
		for (auto& pnode : m_Literals) {
			if (pnode->IsConflictWith(node))
				throw std::invalid_argument("conflict node.");
		}
		for (auto& pnode : m_Choices) {
			if (pnode->IsConflictWith(node))
				throw std::invalid_argument("conflict node.");
		}
		for (auto& pnode : m_Args) {
			if (pnode->IsConflictWith(node))
				throw std::invalid_argument("conflict node.");
		}

		// add into list
		switch (node->GetNodeType()) {
			case NodeType::Literal:
				m_Literals.emplace_back(node);
				break;
			case NodeType::Choice:
				m_Choices.emplace_back(node);
				break;
			case NodeType::Argument:
				m_Args.emplace_back(node);
				break;
			default:
				throw std::runtime_error("No such node type.");
		}

		return this;
	}

	AbstractNode* AbstractNode::Executes(ExecutionFct fct, const char* cmt) {
		if (m_Execution != nullptr) throw std::invalid_argument("duplicated executions.");
		if (fct == nullptr)  throw std::invalid_argument("no function.");
		m_Execution = fct;
		m_ExecutionDesc = cmt == nullptr ? "" : cmt;
		return this;
	}

	AbstractNode* AbstractNode::Comment(const char* cmt) {
		if (cmt == nullptr)
			throw std::invalid_argument("no comment.");
		m_Comment = cmt;
		return this;
	}

	void AbstractNode::Help(HelpDocument* doc) {
		// add self
		std::string symbol(GetHelpSymbol());
		doc->Push(symbol, m_Comment);

		// check terminal
		if (m_Execution != nullptr) {
			doc->Terminate(m_ExecutionDesc);
		}

		// iterate children
		for (auto& pnode : m_Literals) {
			pnode->Help(doc);
		}
		for (auto& pnode : m_Choices) {
			pnode->Help(doc);
		}
		for (auto& pnode : m_Args) {
			pnode->Help(doc);
		}

		// pop self
		doc->Pop();
	}

	bool AbstractNode::Consume(std::deque<std::string>& arglist, ArgumentsMap* argmap) {
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
			for (auto& pnode : m_Literals) {
				if (pnode->Consume(arglist, argmap)) {
					CONSUME_DEFER;
					return true;
				}
			}
			for (auto& pnode : m_Choices) {
				if (pnode->Consume(arglist, argmap)) {
					CONSUME_DEFER;
					return true;
				}
			}
			for (auto& pnode : m_Args) {
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

#pragma region Command Root

	CommandRoot::CommandRoot() : AbstractNode() {}

	CommandRoot::~CommandRoot() {}

	bool CommandRoot::RootConsume(std::deque<std::string>& arglist) {
		// if no data can consume, return
		if (arglist.empty()) return false;

		// create a argument map
		ArgumentsMap amap;

		// and we only just need iterate all children
		for (auto& pnode : m_Literals) {
			if (pnode->Consume(arglist, &amap)) {
				return true;
			}
		}
		for (auto& pnode : m_Choices) {
			if (pnode->Consume(arglist, &amap)) {
				return true;
			}
		}
		for (auto& pnode : m_Args) {
			if (pnode->Consume(arglist, &amap)) {
				return true;
			}
		}

		// no matched
		return false;
	}

	HelpDocument* CommandRoot::RootHelp() {
		HelpDocument* doc = new HelpDocument();

		// we only just need iterate all children
		for (auto& pnode : m_Literals) {
			pnode->Help(doc);
		}
		for (auto& pnode : m_Choices) {
			pnode->Help(doc);
		}
		for (auto& pnode : m_Args) {
			pnode->Help(doc);
		}

		return doc;
	}

#pragma endregion

#pragma region Literal

	Literal::Literal(const char* words) : 
		AbstractNode(), 
		m_Literal(words == nullptr ? "" : words) {
		if (words == nullptr || m_Literal.empty())
			throw std::invalid_argument("Invalid literal.");
	}

	Literal::~Literal() {}

	NodeType Literal::GetNodeType() {
		return NodeType::Literal;
	}

	bool Literal::IsConflictWith(AbstractNode* node) {
		switch (node->GetNodeType()) {
			case NodeType::Literal:
				return dynamic_cast<Literal*>(node)->m_Literal == m_Literal;
			case NodeType::Choice:
				for (const auto& item : dynamic_cast<Choice*>(node)->m_Vocabulary) {
					if (item == m_Literal) return true;
				}
				return false;
			case NodeType::Argument:
				return false;
			default:
				throw std::runtime_error("No such node type.");
		}
	}

	std::string Literal::GetHelpSymbol() {
		return m_Literal;
	}

	bool Literal::BeginAccept(const std::string& strl, ArgumentsMap*) {
		return strl == m_Literal;
	}

	void Literal::EndAccept(ArgumentsMap*) {}

#pragma endregion

#pragma region Choice

	Choice::Choice(const char* argname, const std::initializer_list<std::string>& vocabulary) :
		AbstractNode(),
		m_GottenIndex(0u), m_Accepted(false),
		m_ChoiceName(argname == nullptr ? "" : argname), m_Vocabulary(vocabulary) {
		if (argname == nullptr || m_ChoiceName.empty())
			throw std::invalid_argument("Invalid choice name.");
		if (m_Vocabulary.size() < 2)
			throw std::invalid_argument("Too less vocabulary. At least 2 items.");
	}

	Choice::~Choice() {}

	size_t* Choice::GetIndex() {
		return &m_GottenIndex;
	}

	NodeType Choice::GetNodeType() {
		return NodeType::Choice;
	}

	bool Choice::IsConflictWith(AbstractNode* node) {
		switch (node->GetNodeType()) {
			case NodeType::Literal:
			{
				Literal* pliteral = dynamic_cast<Literal*>(node);
				for (const auto& word : m_Vocabulary) {
					if (word == pliteral->m_Literal)
						return true;
				}
				return false;
			}
			case NodeType::Choice:
			{
				Choice* pchoice = dynamic_cast<Choice*>(node);
				if (pchoice->m_ChoiceName == m_ChoiceName)
					return true;

				for (const auto& thisword : m_Vocabulary) {
					for (const auto& thatword : pchoice->m_Vocabulary) {
						if (thisword == thatword)
							return true;
					}
				}
				return false;
			}
			case NodeType::Argument:
				return m_ChoiceName == dynamic_cast<AbstractArgument*>(node)->m_ArgName;
			default:
				throw std::runtime_error("No such node type.");
		}
	}

	std::string Choice::GetHelpSymbol() {
		std::string switches;
		for (const auto& item : m_Vocabulary) {
			if (!switches.empty()) switches += " | ";
			switches += item;
		}
		return "[" + switches + "]";
	}

	bool Choice::BeginAccept(const std::string& strl, ArgumentsMap* amap) {
		for (size_t i = 0; i < m_Vocabulary.size(); ++i) {
			if (strl == m_Vocabulary[i]) {
				m_Accepted = true;
				m_GottenIndex = i;
				amap->Add(m_ChoiceName, this);
				return true;
			}
		}

		return false;
	}

	void Choice::EndAccept(ArgumentsMap* amap) {
		if (m_Accepted) {
			m_Accepted = false;
			amap->Remove(m_ChoiceName);
		}
	}

#pragma endregion

#pragma region Abstract Argument

	AbstractArgument::AbstractArgument(const char* argname) :
		AbstractNode(),
		m_ArgName(argname == nullptr ? "" : argname), 
		m_Accepted(false), m_ParsedData(nullptr) {
		if (argname == nullptr || m_ArgName.empty())
			throw std::invalid_argument("Invalid argument name.");
	}

	AbstractArgument::~AbstractArgument() {}

	NodeType AbstractArgument::GetNodeType() {
		return NodeType::Argument;
	}

	bool AbstractArgument::IsConflictWith(AbstractNode* node) {
		switch (node->GetNodeType()) {
			case NodeType::Literal:
				return false;
			case NodeType::Choice:
				return m_ArgName == dynamic_cast<Choice*>(node)->m_ChoiceName;
			case NodeType::Argument:
				return m_ArgName == dynamic_cast<AbstractArgument*>(node)->m_ArgName;
			default:
				throw std::runtime_error("No such node type.");
		}
	}

	std::string AbstractArgument::GetHelpSymbol() {
		std::string newargname = "<";
		newargname.append(m_ArgName);
		newargname.append(">");
		return newargname;
	}

	bool AbstractArgument::BeginAccept(const std::string& strl, ArgumentsMap* amap) {
		m_Accepted = BeginParse(strl);
		if (m_Accepted) amap->Add(m_ArgName, this);
		return m_Accepted;
	}

	void AbstractArgument::EndAccept(ArgumentsMap* amap) {
		if (m_Accepted) {
			amap->Remove(m_ArgName);
			EndParse();
			m_Accepted = false;
		}
	}

#pragma endregion

#pragma region Argument Detail Impl

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

		m_ParsedData = new IntArgument::vType(value);
		return true;
	}

	void IntArgument::EndParse() {
		delete reinterpret_cast<IntArgument::vType*>(m_ParsedData);
		m_ParsedData = nullptr;
	}

	bool StringArgument::BeginParse(const std::string& strl) {
		// string always accept every text
		m_ParsedData = new StringArgument::vType(strl);
		return true;
	}

	void StringArgument::EndParse() {
		delete reinterpret_cast<StringArgument::vType*>(m_ParsedData);
		m_ParsedData = nullptr;
	}

	// Copy from Gamepiaynmo/BallanceModLoader
	std::vector<std::string> SplitString(const std::string& str, const std::string& de) {
		size_t lpos, pos = 0;
		std::vector<std::string> res;

		lpos = str.find_first_not_of(de, pos);
		while (lpos != std::string::npos) {
			pos = str.find_first_of(de, lpos);
			res.push_back(str.substr(lpos, pos - lpos));
			if (pos == std::string::npos) break;

			lpos = str.find_first_not_of(de, pos);
		}

		if (pos != std::string::npos)
			res.push_back("");

		return res;
	}

	bool EncodingArgument::BeginParse(const std::string& strl) {
		// encoding always accept every text
		m_ParsedData = new EncodingArgument::vType(SplitString(strl, ","));
		return true;
	}

	void EncodingArgument::EndParse() {
		delete reinterpret_cast<EncodingArgument::vType*>(m_ParsedData);
		m_ParsedData = nullptr;
	}

#pragma endregion

#pragma region Argument Map

	void ArgumentsMap::Add(const std::string& k, AbstractNode* v) {
		m_Data.emplace(std::make_pair(k, v));
	}

	void ArgumentsMap::Remove(const std::string& k) {
		m_Data.erase(k);
	}

#pragma endregion

}
