#include "CmdHelper.hpp"

namespace Unvirt::CmdHelper {

#pragma region CmdSplitter

	const std::deque<std::u8string>& CmdSplitter::GetResult() const {
		if (!m_ValidResult)
			throw std::runtime_error("try to get result from an invalid CmdSplitter.");
		return m_Result;
	}

	bool CmdSplitter::Convert(const std::u8string& u8cmd) {
		// Clear variables
		m_ValidResult = false;
		m_Result.clear();
		m_Buffer.clear();
		m_CurrentChar = u8'\0';
		m_State = m_PrevState = StateType::SPACE;

		// split
		for (char8_t c : u8cmd) {
			m_CurrentChar = c;

			// skip all invalid characters (ascii code unit lower than space char)
			// thus UTF8 code unit can directly accepted.
			if (m_CurrentChar < u8' ')
				continue;

			switch (m_State) {
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
		bool is_success = false;
		switch (m_State) {
			case StateType::SPACE:
				is_success = true;
				break;
			case StateType::NORMAL:
				// push the last one
				m_Result.emplace_back(m_Buffer);
				is_success = true;
				break;
			case StateType::SINGLE:
			case StateType::DOUBLE:
			case StateType::ESCAPE:
				// error
				is_success = false;
				break;
			default:
				throw std::runtime_error("unreachable code.");
		}

		// check success
		if (is_success) {
			m_ValidResult = true;
			return true;
		} else {
			m_Result.clear();
			return false;
		}
	}

	void CmdSplitter::ProcSpace() {
		switch (m_CurrentChar) {
			case u8'\'':
				m_State = StateType::SINGLE;
				break;
			case u8'"':
				m_State = StateType::DOUBLE;
				break;
			case u8'\\':
				m_State = StateType::ESCAPE;
				m_PrevState = StateType::NORMAL;
				break;
			case u8' ':
				break;	// skip blank
			default:
				m_Buffer.push_back(m_CurrentChar);
				m_State = StateType::NORMAL;
				break;
		}
	}
	void CmdSplitter::ProcSingle() {
		switch (m_CurrentChar) {
			case u8'\'':
				m_State = StateType::NORMAL;
				break;
			case u8'"':
				m_Buffer.push_back('"');
				break;
			case u8'\\':
				m_State = StateType::ESCAPE;
				m_PrevState = StateType::SINGLE;
				break;
			case u8' ':
				m_Buffer.push_back(u8' ');
				break;
			default:
				m_Buffer.push_back(m_CurrentChar);
				break;
		}
	}
	void CmdSplitter::ProcDouble() {
		switch (m_CurrentChar) {
			case u8'\'':
				m_Buffer.push_back(u8'\'');
				break;
			case u8'"':
				m_State = StateType::NORMAL;
				break;
			case u8'\\':
				m_State = StateType::ESCAPE;
				m_PrevState = StateType::DOUBLE;
				break;
			case u8' ':
				m_Buffer.push_back(u8' ');
				break;
			default:
				m_Buffer.push_back(m_CurrentChar);
				break;
		}
	}
	void CmdSplitter::ProcEscape() {
		// add itself
		m_Buffer.push_back(m_CurrentChar);
		// restore state
		m_State = m_PrevState;
	}
	void CmdSplitter::ProcNormal() {
		switch (m_CurrentChar) {
			case u8'\'':
				m_Buffer.push_back(u8'\'');
				break;
			case u8'"':
				m_Buffer.push_back(u8'"');
				break;
			case u8'\\':
				m_State = StateType::ESCAPE;
				m_PrevState = StateType::NORMAL;
				break;
			case u8' ':
				m_Result.emplace_back(m_Buffer);
				m_Buffer.clear();
				m_State = StateType::SPACE;
				break;
			default:
				m_Buffer.push_back(m_CurrentChar);
				break;
		}
	}
#pragma endregion

#pragma region Arguments Map

	ArgumentsMap::ArgumentsMap() : m_Data() {}

	ArgumentsMap::~ArgumentsMap() {}

#pragma endregion

#pragma region Help Document

	HelpDocument::HelpDocument() : m_Stack(), m_Results() {}

	HelpDocument::~HelpDocument() {}

	HelpDocument::StackItem::StackItem() : m_Name(), m_Desc() {}

	HelpDocument::StackItem::StackItem(const std::u8string& name, const std::u8string& desc) : m_Name(name), m_Desc(desc) {}

	HelpDocument::ResultItem::ResultItem() : m_CmdDesc(), m_ArgDesc() {}

	HelpDocument::ResultItem::ResultItem(const std::u8string& cmd_desc, const std::deque<StackItem>& arg_desc) :
		m_CmdDesc(cmd_desc), m_ArgDesc(arg_desc.begin(), arg_desc.end()) {}

	void HelpDocument::Push(const std::u8string& arg_name, const std::u8string& arg_desc) {
		m_Stack.emplace_back(StackItem { arg_name, arg_desc });
	}

	void HelpDocument::Pop() {
		if (m_Stack.empty())
			throw std::runtime_error("try pop back on an empty help document.");
		m_Stack.pop_back();
	}

	void HelpDocument::Terminate(std::u8string& command_desc) {
		// create new result
		ResultItem result(command_desc, this->m_Stack);
		// add into result
		m_Results.emplace_back(std::move(result));
	}

	void HelpDocument::Print() {
		for (auto& cmd : m_Results) {
			// syntax
			YYCC::ConsoleHelper::WriteLine(u8"Syntax: ");
			for (const auto& arg : cmd.m_ArgDesc) {
				YYCC::ConsoleHelper::Format(u8"%s ", arg.m_Name.c_str());
			}
			YYCC::ConsoleHelper::WriteLine(u8"");
			// command description
			if (!cmd.m_CmdDesc.empty()) {
				YYCC::ConsoleHelper::FormatLine(u8"Description: %s", cmd.m_CmdDesc.c_str());
			}
			// argument description
			YYCC::ConsoleHelper::WriteLine(u8"Arguments:");
			for (auto& arg : cmd.m_ArgDesc) {
				if (!arg.m_Desc.empty()) {
					YYCC::ConsoleHelper::FormatLine(u8"\t%s: %s", arg.m_Name.c_str(), arg.m_Desc.c_str());
				}
			}
			// space between each commands
			YYCC::ConsoleHelper::WriteLine(u8"");
		}
	}

#pragma endregion

	namespace Nodes {

#pragma region Abstract Node

		AbstractNode::AbstractNode() :
			m_Execution(nullptr), m_Comment(), m_Nodes() {}

		AbstractNode::~AbstractNode() {}

		AbstractNode& AbstractNode::Executes(FctExecution_t fct, const std::u8string_view& exec_desc) {
			if (m_Execution != nullptr)
				throw std::invalid_argument("you should not assign execution multiuple times.");
			if (fct == nullptr)
				throw std::invalid_argument("the function passed for executing should not be nullptr.");
			m_Execution = fct;
			m_ExecutionDesc = exec_desc;
			return *this;
		}

		AbstractNode& AbstractNode::Comment(const std::u8string_view& comment) {
			m_Comment = comment;
			return *this;
		}

		void AbstractNode::Help(HelpDocument& doc) {
			// Push self symbol to help document stack.
			doc.Push(GetHelpSymbol(), m_Comment);

			// Check whether this node is terminal.
			// If it is, terminate it once.
			if (m_Execution != nullptr) {
				doc.Terminate(m_ExecutionDesc);
			}

			// Then process its children nodes.
			for (auto& node : m_Nodes) {
				node->Help(doc);
			}

			// Pop self from help document stack
			doc.Pop();
		}

		bool AbstractNode::Consume(CmdSplitter::Result_t& al, ArgumentsMap& am) {
			// if no data can consume, return
			if (al.empty()) return false;

			// backup current value
			std::u8string cur_cmd = al.front();
			// consume self
			if (!BeginConsume(cur_cmd, am)) {
				// fail to consume self. not matched. return
				return false;
			}

			// pop front for processing child nodes.
			al.pop_front();

#define CONSUME_DEFER \
	al.emplace_front(cur_cmd); \
	EndConsume(am);

			if (al.empty()) {
				// if no more data for parsing.
				// this is must be a terminal.
				// check whether we have execution.
				if (m_Execution == nullptr) {
					CONSUME_DEFER;
					return false;
				} else {
					m_Execution(am);
					CONSUME_DEFER;
					return true;
				}
			} else {
				// still have data to be parsed. try  to match them.
				// iterate node list to find the real terminal
				// however, we need iterate literal and choice first, the iterate argument.
				for (auto& node : m_Nodes) {
					if (node->IsArgument()) continue;
					if (node->Consume(al, am)) {
						CONSUME_DEFER;
						return true;
					}
				}
				for (auto& node : m_Nodes) {
					if (!node->IsArgument()) continue;
					if (node->Consume(al, am)) {
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

#pragma region Literal

		Literal::Literal(const std::u8string_view& words) :
			AbstractNode(), m_Literal(words), m_ConflictSet { m_Literal } {
			if (words.empty())
				throw std::invalid_argument("The word of literal node should not be empty.");
		}

		Literal::~Literal() {}

		bool Literal::IsArgument() { return false; }
		const std::set<std::u8string>& Literal::GetConflictSet() { return m_ConflictSet; }
		std::u8string Literal::GetHelpSymbol() { return m_Literal; }
		bool Literal::BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) { return cur_cmd == m_Literal; }
		void Literal::EndConsume(ArgumentsMap& am) {}

#pragma endregion

#pragma region Choice

		Choice::Choice(const std::u8string_view& argname, const std::initializer_list<std::u8string>& vocabulary) :
			AbstractNode(),
			m_ChoiceName(argname), m_Vocabulary(vocabulary),
			m_ConflictSet() {
			// check argument
			if (argname.empty())
				throw std::invalid_argument("Choice argument name should not be empty.");
			if (m_Vocabulary.size() < 2u)
				throw std::invalid_argument("Too less vocabulary for choice. At least 2 items.");
			// init conflict set
			m_ConflictSet.insert(m_ChoiceName);
			m_ConflictSet.insert(m_Vocabulary.begin(), m_Vocabulary.end());
		}

		Choice::~Choice() {}

		bool Choice::IsArgument() { return false; }
		const std::set<std::u8string>& Choice::GetConflictSet() { return m_ConflictSet; }
		std::u8string Choice::GetHelpSymbol() {
			return YYCC::StringHelper::Printf(u8"[%s]",
				YYCC::StringHelper::Join(m_Vocabulary, u8" | ").c_str()
			);
		}
		bool Choice::BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) {
			for (size_t i = 0; i < m_Vocabulary.size(); ++i) {
				if (cur_cmd == m_Vocabulary[i]) {
					am.Add<AMItems::StringItem>(m_ChoiceName, cur_cmd);
					return true;
				}
			}
			return false;
		}
		void Choice::EndConsume(ArgumentsMap& am) { am.Remove(m_ChoiceName); }

#pragma endregion

#pragma region Abstract Argument

		AbstractArgument::AbstractArgument(const std::u8string_view& argname) :
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

	}

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

}
