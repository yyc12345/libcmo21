#include "CmdHelper.hpp"
#include <algorithm>

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

#pragma region Conflict Set

	ConflictSet::ConflictSet() : m_ConflictSet() {}

	ConflictSet::~ConflictSet() {}

	void ConflictSet::AddLiteral(const std::u8string_view& value) {
		if (value.empty())
			throw std::invalid_argument("try to insert empty item to conflict set.");
		auto result = m_ConflictSet.emplace(u8"literal:" + std::u8string(value));
		if (!result.second)
			throw std::runtime_error("try to insert duplicated item in conflict set.");
	}

	void ConflictSet::AddArgument(const std::u8string_view& value) {
		if (value.empty())
			throw std::invalid_argument("try to insert empty item to conflict set.");
		auto result = m_ConflictSet.emplace(u8"argument:" + std::u8string(value));
		if (!result.second)
			throw std::runtime_error("try to insert duplicated item in conflict set.");
	}

	bool ConflictSet::IsConflictWith(const ConflictSet& rhs) const {
		// create a cache to store computed intersection
		std::vector<std::u8string> intersection;
		// compute intersection
		std::set_intersection(
			this->m_ConflictSet.begin(), this->m_ConflictSet.end(),
			rhs.m_ConflictSet.begin(), rhs.m_ConflictSet.begin(),
			std::back_inserter(intersection)
		);
		// check whether it is empty intersection
		return !intersection.empty();
	}

#pragma endregion

	namespace Nodes {

#pragma region Abstract Node

		AbstractNode::AbstractNode() :
			m_Execution(nullptr), m_Comment(), m_Nodes() {}

		AbstractNode::~AbstractNode() {}

		AbstractNode& AbstractNode::Executes(FctExecution_t fct, const std::u8string_view& exec_desc) {
			if (this->IsRootNode())
				throw std::logic_error("root node should not have execution.");
			if (m_Execution != nullptr)
				throw std::invalid_argument("you should not assign execution multiuple times.");
			if (fct == nullptr)
				throw std::invalid_argument("the function passed for executing should not be nullptr.");
			m_Execution = fct;
			m_ExecutionDesc = exec_desc;
			return *this;
		}

		AbstractNode& AbstractNode::Comment(const std::u8string_view& comment) {
			if (this->IsRootNode())
				throw std::logic_error("root node should not have comment.");
			m_Comment = comment;
			return *this;
		}

		void AbstractNode::Help(HelpDocument& doc) {
			// If this node is not root node
			if (!this->IsRootNode()) {
				// Push self symbol to help document stack.
				if (!this->IsRootNode()) {
					doc.Push(GetHelpSymbol(), m_Comment);
				}

				// Check whether this node is terminal.
				// If it is, terminate it once.
				if (m_Execution != nullptr) {
					doc.Terminate(m_ExecutionDesc);
				}
			}

			// Then process its children nodes (both root node and common node).
			for (auto& node : m_Nodes) {
				node->Help(doc);
			}

			// Pop self from help document stack
			// if this node is not root node
			if (!this->IsRootNode()) {
				doc.Pop();
			}
		}

		bool AbstractNode::Consume(CmdSplitter::Result_t& al, ArgumentsMap& am) {
			// If no command to be consumed, return directly.
			if (al.empty()) return false;

			// Process for self if we are not root node
			std::u8string cur_cmd;
			if (!this->IsRootNode()) {
				// Backup the top item in command stack for personal consume.
				cur_cmd = al.front();
				// Try to consume it for self.
				if (!BeginConsume(cur_cmd, am)) {
					// Fail to consume self.
					// It means that this command is not matched with self.
					// Return directly.
					return false;
				}
				// Yes, command matched, we try consume it for child nodes.
				// Pop the top item of command stack for child nodes processing.
				al.pop_front();
			}

			// Define free function if we are not the root node.
			// Because we just pop the top item of command stack.
#define CONSUME_DEFER \
if (!this->IsRootNode()) { \
	al.emplace_front(cur_cmd); \
	EndConsume(am); \
}

			if (al.empty()) {
				// Root node do not have execution, return false directly
				if (this->IsRootNode()) {
					CONSUME_DEFER;
					return false;
				}
				// If no more data for parsing, this is must be a terminal.
				// Check whether we have execution if we are not root node.
				if (m_Execution == nullptr) {
					CONSUME_DEFER;
					return false;
				} else {
					m_Execution(am);
					CONSUME_DEFER;
					return true;
				}
			} else {
				// Command stack still item to be consumed.
				// To consume them, we need iterate node list and find the real terminal.
				// However, we need iterate literal and choice first, the iterate argument.
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

				// If all node can not consume it, 
				// it means that this command can not match this node tree.
				// Return false directly.
				CONSUME_DEFER;
				return false;
			}

#undef CONSUME_DEFER

		}

#pragma endregion

#pragma region Root Node

		RootNode::RootNode() : AbstractNode() {}
		RootNode::~RootNode() {}

		bool RootNode::IsRootNode() { return true; }
		bool RootNode::IsArgument() { throw std::logic_error("this function is not allowed on root function."); }
		const ConflictSet& RootNode::GetConflictSet() { throw std::logic_error("this function is not allowed on root function."); }
		std::u8string RootNode::GetHelpSymbol() { throw std::logic_error("this function is not allowed on root function."); }
		bool RootNode::BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) { throw std::logic_error("this function is not allowed on root function."); }
		void RootNode::EndConsume(ArgumentsMap& am) { throw std::logic_error("this function is not allowed on root function."); }

#pragma endregion

#pragma region Literal

		Literal::Literal(const std::u8string_view& words) :
			AbstractNode(), m_Literal(words), m_ConflictSet() {
			// check argument
			if (words.empty())
				throw std::invalid_argument("The word of literal node should not be empty.");
			// set conflict set
			m_ConflictSet.AddLiteral(m_Literal);
		}
		Literal::~Literal() {}

		bool Literal::IsRootNode() { return false; }
		bool Literal::IsArgument() { return false; }
		const ConflictSet& Literal::GetConflictSet() { return m_ConflictSet; }
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
			std::set<std::u8string> vocabulary_set(m_Vocabulary.begin(), m_Vocabulary.end());
			if (vocabulary_set.size() != m_Vocabulary.size())
				throw std::invalid_argument("Vocabulary of choice should not have duplicated items.");
			// init conflict set
			m_ConflictSet.AddArgument(m_ChoiceName);
			for (const auto& word : m_Vocabulary) {
				m_ConflictSet.AddLiteral(word);
			}
		}
		Choice::~Choice() {}

		bool Choice::IsRootNode() { return false; }
		bool Choice::IsArgument() { return false; }
		const ConflictSet& Choice::GetConflictSet() { return m_ConflictSet; }
		std::u8string Choice::GetHelpSymbol() {
			return YYCC::StringHelper::Printf(u8"[%s]",
				YYCC::StringHelper::Join(m_Vocabulary, u8" | ").c_str()
			);
		}
		bool Choice::BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) {
			for (size_t i = 0; i < m_Vocabulary.size(); ++i) {
				if (cur_cmd == m_Vocabulary[i]) {
					am.Add<ArgValue_t>(m_ChoiceName, i);
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
			m_ArgumentName(argname), m_ConflictSet() {
			// check argument
			if (argname.empty())
				throw std::invalid_argument("Argument name should not be empty.");
			// setup conflict set
			m_ConflictSet.AddArgument(m_ArgumentName);
		}
		AbstractArgument::~AbstractArgument() {}

		bool AbstractArgument::IsRootNode() { return false; }
		bool AbstractArgument::IsArgument() { return true; }
		const ConflictSet& AbstractArgument::GetConflictSet() { return m_ConflictSet; }
		std::u8string AbstractArgument::GetHelpSymbol() {
			return YYCC::StringHelper::Printf(u8"<%s>", m_ArgumentName.c_str());
		}
		//bool AbstractArgument::BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) { return false; }
		void AbstractArgument::EndConsume(ArgumentsMap& am) { am.Remove(m_ArgumentName); }

#pragma endregion

#pragma region Arithmetic Argument

		// It's a template class
		// We are forced to implement it in header file.

#pragma endregion

#pragma region String Argument

		bool StringArgument::BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) {
			// check constraint
			if (m_Constraint.IsValid() && !m_Constraint.m_CheckFct(cur_cmd))
				return false;
			// accept
			am.Add<ArgValue_t>(m_ArgumentName, cur_cmd);
			return true;
		}

#pragma endregion

#pragma region Encoding List Argument

		bool EncodingListArgument::BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) {
			// split given argument
			std::vector<std::u8string> encs = YYCC::StringHelper::Split(cur_cmd, u8",");
			// add into map
			am.Add<ArgValue_t>(m_ArgumentName, encs);
			return true;
		}

#pragma endregion

	}

#pragma region Command Parser

	CommandParser::CommandParser() {}

	CommandParser::~CommandParser() {}

	bool CommandParser::Parse(const CmdSplitter::Result_t& cmds) {
		// Create a copy of given command
		CmdSplitter::Result_t al(cmds);
		// Create argument map
		ArgumentsMap am;

		// Call root node Consume function and return its result.
		return m_RootNode.Consume(al, am);
	}

	HelpDocument CommandParser::Help() {
		// Create help docuemnt
		HelpDocument doc;
		// use node tree to fill help document.
		m_RootNode.Help(doc);
		// return result.
		return doc;
	}

	Nodes::RootNode& CommandParser::GetRoot() {
		return m_RootNode;
	}

#pragma endregion

}
