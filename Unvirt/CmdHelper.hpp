#pragma once

#include <YYCCommonplace.hpp>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <functional>
#include <initializer_list>
#include <type_traits>
#include <stdexcept>
#include <memory>

namespace Unvirt::CmdHelper {

	class CmdSplitter {
	public:
		using Result_t = std::deque<std::u8string>;
	private:
		enum class StateType : int {
			SPACE,
			SINGLE,
			DOUBLE,
			ESCAPE,
			NORMAL
		};

	public:
		CmdSplitter() :
			m_CurrentChar(u8'\0'), m_Buffer(), m_Result(), m_ValidResult(false),
			m_State(StateType::NORMAL), m_PrevState(StateType::NORMAL) {}
		~CmdSplitter() {}
		YYCC_DEL_CLS_COPY_MOVE(CmdSplitter);

		bool Convert(const std::u8string& u8cmd);
		const Result_t& GetResult() const;

	private:
		void ProcSpace();
		void ProcSingle();
		void ProcDouble();
		void ProcEscape();
		void ProcNormal();

		char8_t m_CurrentChar;
		std::u8string m_Buffer;
		Result_t m_Result;
		bool m_ValidResult;
		StateType m_State, m_PrevState;
	};

	namespace AMItems {

		class AbstractItem {
		public:
			AbstractItem() {}
			virtual ~AbstractItem() {}
			YYCC_DEF_CLS_COPY_MOVE(AbstractItem);
		};

		template<typename _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>, int> = 0>
		class ArithmeticItem : public AbstractItem {
		public:
			ArithmeticItem(_Ty value) : AbstractItem(), m_Data(value) {}
			virtual ~ArithmeticItem() {}
			YYCC_DEF_CLS_COPY_MOVE(ArithmeticItem);
		public:
			_Ty Get() const { return m_Data; }
		protected:
			_Ty m_Data;
		};

		template<typename _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>, int> = 0>
		class ArithmeticArrayItem : public AbstractItem {
		public:
			ArithmeticArrayItem(const std::vector<_Ty>& values) : AbstractItem(), m_Data(values) {}
			virtual ~ArithmeticArrayItem() {}
			YYCC_DEF_CLS_COPY_MOVE(ArithmeticArrayItem);
		public:
			const std::vector<_Ty>& Get() const { return m_Data; }
		protected:
			std::vector<_Ty> m_Data;
		};

		class StringItem : public AbstractItem {
		public:
			StringItem(const std::u8string_view& value) : AbstractItem(), m_Data(value) {}
			virtual ~StringItem() {}
			YYCC_DEF_CLS_COPY_MOVE(StringItem);
		public:
			const std::u8string& Get() const { return m_Data; }
		protected:
			std::u8string m_Data;
		};

		class StringArrayItem : public AbstractItem {
		public:
			StringArrayItem(const std::vector<std::u8string>& value) : AbstractItem(), m_Data(value) {}
			virtual ~StringArrayItem() {}
			YYCC_DEF_CLS_COPY_MOVE(StringArrayItem);
		public:
			const std::vector<std::u8string>& Get() const { return m_Data; }
		protected:
			std::vector<std::u8string> m_Data;
		};

	}

	class ArgumentsMap {
	public:
		ArgumentsMap();
		~ArgumentsMap();
		YYCC_DEF_CLS_COPY_MOVE(ArgumentsMap);

	protected:
		std::map<std::u8string, std::shared_ptr<AMItems::AbstractItem>> m_Data;

	public:
		template<class _Ty, class... _Types, std::enable_if_t<std::is_base_of_v<AMItems::AbstractItem, _Ty>, int> = 0>
		void Add(const std::u8string_view& key, _Types&&... args) {
			// check argument
			if (key.empty())
				throw std::invalid_argument("argument key should not be empty");
			// insert into data
			auto result = m_Data.try_emplace(std::u8string(key), std::make_shared<_Ty>(std::forward<_Types>(args)...));
			if (!result.second)
				throw std::runtime_error("try to add an existing key.");
		}
		template<class _Ty, std::enable_if_t<std::is_base_of_v<AMItems::AbstractItem, _Ty>, int> = 0>
		const _Ty& Get(const std::u8string_view& key) const {
			// check argument
			if (key.empty())
				throw std::invalid_argument("argument key should not be empty");
			// find key first
			auto finder = m_Data.find(std::u8string(key));
			if (finder == m_Data.end())
				throw std::runtime_error("try to get a non-existent key.");
			// get stored value data
			const AMItems::AbstractItem& value = *finder->second.get();
			return static_cast<const _Ty&>(value);
		}
		void Remove(const std::u8string_view& key) {
			// check argument
			if (key.empty())
				throw std::invalid_argument("argument key should not be empty");
			// remove it from map and check whether remove item
			if (m_Data.erase(std::u8string(key)) == 0u)
				throw std::runtime_error("try to delete a non-existent key.");
		}
	};

	class HelpDocument {
	public:
		HelpDocument();
		~HelpDocument();
		YYCC_DEF_CLS_COPY_MOVE(HelpDocument);

	public:
		void Push(const std::u8string& arg_name, const std::u8string& arg_desc);
		void Pop();
		void Terminate(std::u8string& command_desc);
		void Print();

	protected:
		struct StackItem {
			StackItem();
			StackItem(const std::u8string& name, const std::u8string& desc);
			YYCC_DEF_CLS_COPY_MOVE(StackItem);

			std::u8string m_Name;
			std::u8string m_Desc;
		};
		std::deque<StackItem> m_Stack;

		struct ResultItem {
			ResultItem();
			ResultItem(const std::u8string& cmd_desc, const std::deque<StackItem>& arg_desc);
			YYCC_DEF_CLS_COPY_MOVE(ResultItem);

			std::u8string m_CmdDesc;
			std::vector<StackItem> m_ArgDesc;
		};
		std::vector<ResultItem> m_Results;
	};

	class ConflictSet {
	public:
		ConflictSet();
		~ConflictSet();
		YYCC_DEF_CLS_COPY_MOVE(ConflictSet);

	public:
		/**
		 * @brief Add literal item into conflict set.
		 * @param[in] value Literal item.
		 * @remarks
		 * \li Literal item is the string input in command line.
		 * \li The word in Literal, and the vocabulary in Choice should be put by this function.
		 * \li Added item will add \c literal: prefix to make it in literal scope,
		 * so that it will not be compared with argument name items.
		 * Because we allow 2 literal item and argument name item have same name.
		*/
		void AddLiteral(const std::u8string_view& value);
		/**
		 * @brief Add argument name item into conflict
		 * @param[in] value Argument name item.
		 * @remarks
		 * \li Argument name item is the key name put in ArgumentsMap.
		 * \li The argument name in Choice and Argument should be put by this function.
		 * \li Added item will add \c argument: prefix to make it in argument name scope,
		 * so that it will not be compared with literal items.
		 * Because we allow 2 literal item and argument name item have same name.
		*/
		void AddArgument(const std::u8string_view& value);
		/**
		 * @brief Check whether this set is conflict with another.
		 * @param[in] rhs The set to be compared.
		 * @return True if they have conflict.
		 * @remarks
		 * This function simply compute the intersection of two set.
		 * If the result is not empty, it means that there is a conflict.
		*/
		bool IsConflictWith(const ConflictSet& rhs) const;

	protected:
		std::set<std::u8string> m_ConflictSet;
	};

	// Forward declaration of CommandParser for Nodes::RootNode
	class CommandParser;

	namespace Nodes {

		class AbstractNode {
		public:
			using FctExecution_t = void(*)(const ArgumentsMap&);

		public:
			AbstractNode();
			virtual ~AbstractNode();
			YYCC_DEF_CLS_COPY_MOVE(AbstractNode);

		protected:
			std::vector<std::shared_ptr<AbstractNode>> m_Nodes;
			FctExecution_t m_Execution;
			std::u8string m_ExecutionDesc;
			std::u8string m_Comment;

		protected:
			/**
			 * @brief The core function to generate help document by following hierarchy.
			 * @param[in] doc The generating help document.
			*/
			void Help(HelpDocument& doc);
			/**
			 * @brief The core function to consume splitted commands by following hierarchy.
			 * @param[in] al The splitted commands deque.
			 * @param[in] am Argument map for operating.
			 * @return True if we reach a legal terminal, otherwise false.
			 * Once this function return true, there is no need to process other nodes.
			 * Because the final command processor has been executed when this function return true.
			*/
			bool Consume(CmdSplitter::Result_t& al, ArgumentsMap& am);

		protected:
			/**
			 * @brief Check whether current node is root node.
			 * @return True if it is, otherwise false.
			 * @remarks
			 * This function usually return false.
			 * It only return true when using special node called root node with CommandParser.
			*/
			virtual bool IsRootNode() = 0;
			/**
			 * @brief Check whether current node is argument.
			 * @return True if it is, otherwise false.
			 * @remakrs
			 * \li Sub-class must implement this function.
			 * \li This function is used internally because when consuming nodes,
			 * we need consume literal and choice first, then consume argument.
			*/
			virtual bool IsArgument() = 0;
			/**
			 * @brief Get a set of identifier used for checking node conflict.
			 * @return The set of identifier.
			 * @remarks
			 * Sub-class must implement this function.
			 * \par
			 * This function return the reference to the set.
			 * It means that sub-class need allocate some memory by themselves
			 * to store the value returned by this function.
			 * \par
			 * When adding new nodes, we use this function to check whether there is conflict.
			 * If the intersection between 2 sets coming from different nodes is not empty,
			 * it means that they have conflict, the process of adding will be aborted.
			 * \par
			 * In details:
			 * \li Literal: Put its literal in set directly.
			 * \li Choice: Put its vocabulary and associated argument name in set.
			 * \li Argument: Put its argument name in set.
			*/
			virtual const ConflictSet& GetConflictSet() = 0;
			/**
			 * @brief Get the string presented in syntax part in help messages.
			 * @return The string presented in syntax part in help messages.
			 * @remarks Sub-class must implement this function.
			 * \li Literal: Return its literal directly.
			 * \li Choice: Join vocabulary with \c \| then brack it with square bracker.
			 * \li Argument: Bracket its argument name with sharp bracket.
			*/
			virtual std::u8string GetHelpSymbol() = 0;
			/**
			 * @brief Try consume given command for this node.
			 * @param[in] cur_cmd The command hope to be accepted.
			 * @param[in] am Argument map for operating.
			 * @return True if this node accept this command.
			 * @remarks
			 * Sub-class must implement this function.
			 * \par
			 * For choice and argument, if given command is accepted,
			 * implementation should insert data into argument map.
			 * So that user can visit it.
			*/
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) = 0;
			/**
			 * @brief End the consume of this node.
			 * @param[in] am Argument map for operating.
			 * @remarks
			 * Sub-class must implement this function.
			 * \par
			 * If BeginConsume() return false, this function will not be called.
			 * \par
			 * For choice and argument, if you have accepted one command,
			 * implementation should remove data from argument map.
			 * So that following nodes (not belongs to this tree) may add the argument with same name.
			*/
			virtual void EndConsume(ArgumentsMap& am) = 0;

		public:
			/**
			 * @brief Add a new node as child nodes for this node.
			 * @tparam _Ty The child class type of AbstractNode.
			 * @param[in] node The node instance to be added.
			 * @return Return self for chain calling.
			*/
			template<class _Ty, std::enable_if_t<std::is_base_of_v<AbstractNode, _Ty> && !std::is_same_v<AbstractNode, _Ty>, int> = 0>
			AbstractNode& Then(_Ty&& node) {
				// create node first
				auto new_node = std::make_shared<_Ty>(std::forward<_Ty>(node));
				// check root node.
				if (new_node->IsRootNode())
					throw std::invalid_argument("root node should not be inserted as child node.");
				// check conflict
				const auto& new_node_set = new_node->GetConflictSet();
				for (auto& node : m_Nodes) {
					const auto& node_set = node->GetConflictSet();
					if (new_node_set.IsConflictWith(node_set))
						throw std::invalid_argument("try to add a conflict node. please check your code.");
				}
				// add into list
				m_Nodes.emplace_back(std::move(new_node));
				return *this;
			}
			/**
			 * @brief Setup execution infomation for this node.
			 * @param[in] fct Associated execution function. nullptr is not allowed.
			 * @param[in] exec_desc Associated execution message presented in help message.
			 * @return Return self for chain calling.
			 * @remarks This function only can be called once for one node.
			*/
			AbstractNode& Executes(FctExecution_t fct, const std::u8string_view& exec_desc = u8"");
			/**
			 * @brief Setup command for this node.
			 * @param[in] comment The command of current node.
			 * @return Return self for chain calling.
			*/
			AbstractNode& Comment(const std::u8string_view& comment = u8"");

		};

		class RootNode : public AbstractNode {
			friend class CommandParser;
		public:
			RootNode();
			virtual ~RootNode();
			YYCC_DEF_CLS_COPY_MOVE(RootNode);

		protected:
			virtual bool IsRootNode() override;
			virtual bool IsArgument() override;
			virtual const ConflictSet& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;
		};

		class Literal : public AbstractNode {
		public:
			Literal(const std::u8string_view& words);
			virtual ~Literal();
			YYCC_DEF_CLS_COPY_MOVE(Literal);

		protected:
			virtual bool IsRootNode() override;
			virtual bool IsArgument() override;
			virtual const ConflictSet& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;

			std::u8string m_Literal;
			ConflictSet m_ConflictSet;
		};

		class Choice : public AbstractNode {
		public:
			using ArgValue_t = AMItems::ArithmeticItem<size_t>;
		public:
			Choice(const std::u8string_view& argname, const std::initializer_list<std::u8string>& vocabulary);
			virtual ~Choice();
			YYCC_DEF_CLS_COPY_MOVE(Choice);

		protected:
			virtual bool IsRootNode() override;
			virtual bool IsArgument() override;
			virtual const ConflictSet& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;

			std::u8string m_ChoiceName;
			std::vector<std::u8string> m_Vocabulary;
			ConflictSet m_ConflictSet;
		};

		class AbstractArgument : public AbstractNode {
		public:
			AbstractArgument(const std::u8string_view& argname);
			virtual ~AbstractArgument();
			YYCC_DEF_CLS_COPY_MOVE(AbstractArgument);

			// AbstractArgument do not implement BeginConsume().
			// Because it involve the detail of data parsing.
			// However, other parts are shared by all argument type.

		protected:
			virtual bool IsRootNode() override;
			virtual bool IsArgument() override;
			virtual const ConflictSet& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			//virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;

			std::u8string m_ArgumentName;
			ConflictSet m_ConflictSet;
		};

		template<typename _Ty, std::enable_if_t<std::is_arithmetic_v<_Ty>, int> = 0>
		class ArithmeticArgument : public AbstractArgument {
		public:
			using ArgValue_t = AMItems::ArithmeticItem<_Ty>;
			using Constraint_t = YYCC::Constraints::Constraint<_Ty>;
		public:
			ArithmeticArgument(const std::u8string_view& argname, Constraint_t constraint = Constraint_t {}) :
				AbstractArgument(argname), m_Constraint(constraint) {}
			virtual ~ArithmeticArgument() {}
			YYCC_DEF_CLS_COPY_MOVE(ArithmeticArgument);

		protected:
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override {
				// try parse
				_Ty result;
				if (!YYCC::ParserHelper::TryParse<_Ty>(cur_cmd, result))
					return false;
				// check constraint
				if (m_Constraint.IsValid() && !m_Constraint.m_CheckFct(result))
					return false;
				// okey
				am.Add<ArgValue_t>(m_ArgumentName, result);
				return true;
			}
		protected:
			Constraint_t m_Constraint;
		};

		class StringArgument : public AbstractArgument {
		public:
			using ArgValue_t = AMItems::StringItem;
			using Constraint_t = YYCC::Constraints::Constraint<std::u8string>;
		public:
			StringArgument(const std::u8string_view& argname, Constraint_t constraint = Constraint_t {}) :
				AbstractArgument(argname), m_Constraint(constraint) {}
			virtual ~StringArgument() {}
			YYCC_DEF_CLS_COPY_MOVE(StringArgument);

		protected:
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			Constraint_t m_Constraint;
		};

		class EncodingListArgument : public AbstractArgument {
		public:
			using ArgValue_t = AMItems::StringArrayItem;
		public:
			EncodingListArgument(const std::u8string_view& argname) :
				AbstractArgument(argname) {}
			virtual ~EncodingListArgument() {}
			YYCC_DEF_CLS_COPY_MOVE(EncodingListArgument);

		protected:
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
		};

	}

	class CommandParser {
	public:
		CommandParser();
		~CommandParser();

	public:
		bool Parse(const CmdSplitter::Result_t& cmds);
		HelpDocument Help();
		Nodes::RootNode& GetRoot();

	private:
		Nodes::RootNode m_RootNode;
	};

	//class CommandRoot : public AbstractNode {
	//public:
	//	CommandRoot();
	//	virtual ~CommandRoot();
	//	YYCC_DEL_CLS_COPY_MOVE(CommandRoot);

	//	// Root use special consume and help functions.
	//	bool RootConsume(std::deque<std::string>&);
	//	HelpDocument* RootHelp();

	//public:
	//protected:
	//	virtual NodeType GetNodeType() override { throw std::logic_error("Root can not be called."); }
	//	virtual bool IsConflictWith(AbstractNode*) override { throw std::logic_error("Root can not be called."); }
	//	virtual std::string GetHelpSymbol()  override { throw std::logic_error("Root can not be called."); }
	//	virtual bool BeginAccept(const std::string&, ArgumentsMap*) override { throw std::logic_error("Root can not be called."); }
	//	virtual void EndAccept(ArgumentsMap*) override { throw std::logic_error("Root can not be called."); }
	//};

}
