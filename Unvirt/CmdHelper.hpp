#pragma once

#include <YYCCommonplace.hpp>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <functional>
#include <algorithm>
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
		const _Ty& Get() const {
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

	namespace Nodes {

		class AbstractNode {
			friend class CommandRoot;
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
			 * @brief Check whether current node is argument.
			 * @return True if it is.
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
			virtual const std::set<std::u8string>& GetConflictSet() = 0;
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
			template<class _Ty, std::enable_if_t<std::is_base_of_v<AbstractNode, _Ty>, int> = 0>
			AbstractNode& Then(_Ty&& node) {
				// create node first
				auto new_node = std::make_shared<_Ty>(std::forward<_Types>(node));
				// check conflict
				std::vector<std::u8string> intersection;
				const auto& new_node_set = new_node->GetConflictSet();
				for (auto& node : mNodes) {
					const auto& node_set = node->GetConflictSet();
					// compute intersection
					intersection.clear();
					std::set_intersection(
						new_node_set.begin(), new_node_set.end(),
						node_set.begin(), node_set.begin(),
						std::back_inserter(intersection)
					);
					// check whether it is empty intersection
					if (!intersection.empty())
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

		class Literal : public AbstractNode {
		public:
			Literal(const std::u8string_view& words);
			virtual ~Literal();
			YYCC_DEF_CLS_COPY_MOVE(Literal);

		protected:
			virtual bool IsArgument() override;
			virtual const std::set<std::u8string>& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;

			std::u8string m_Literal;
			std::set<std::u8string> m_ConflictSet;
		};

		class Choice : public AbstractNode {
		public:
			using ArgValue_t = size_t;
		public:
			Choice(const std::u8string_view& argname, const std::initializer_list<std::u8string>& vocabulary);
			virtual ~Choice();
			YYCC_DEF_CLS_COPY_MOVE(Choice);
			
		protected:
			virtual bool IsArgument() override;
			virtual const std::set<std::u8string>& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;

			std::u8string m_ChoiceName;
			std::vector<std::u8string> m_Vocabulary;
			std::set<std::u8string> m_ConflictSet;
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
			virtual bool IsArgument() override;
			virtual const std::set<std::u8string>& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			// virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;

			std::u8string m_ArgName;
			std::set<std::u8string> m_ConflictSet;
		};

		/**
		 * @brief Return true mean this value can accept.
		*/
		using IntLimit = std::function<bool(int32_t)>;
		class IntArgument : public AbstractArgument {
		public:
			using vType = int32_t;
			IntArgument(const char* argname, IntLimit limit = nullptr) :
				AbstractArgument(argname), m_IntLimit(limit) {}
			virtual ~IntArgument() {}
			YYCC_DEL_CLS_COPY_MOVE(IntArgument);

		protected:
			virtual bool BeginParse(const std::string&) override;
			virtual void EndParse() override;

			IntLimit m_IntLimit;
		};

		class StringArgument : public AbstractArgument {
		public:
			using vType = std::string;
			StringArgument(const char* argname) :
				AbstractArgument(argname) {}
			virtual ~StringArgument() {}
			YYCC_DEL_CLS_COPY_MOVE(StringArgument);

		protected:
			virtual bool BeginParse(const std::string&) override;
			virtual void EndParse() override;
		};

		class EncodingArgument : public AbstractArgument {
		public:
			using vType = std::vector<std::string>;
			EncodingArgument(const char* argname) :
				AbstractArgument(argname) {}
			virtual ~EncodingArgument() {}
			YYCC_DEL_CLS_COPY_MOVE(EncodingArgument);

		protected:
			virtual bool BeginParse(const std::string&) override;
			virtual void EndParse() override;
		};

	}

	class CommandParser {
	public:
		CommandParser();
		~CommandParser();

	public:
		bool Parse(std::deque<std::string>& cmds);
		HelpDocument Help();

	private:
		class RootNode : Nodes::AbstractNode {
		public:
			RootNode();
			virtual ~RootNode();
			YYCC_DEF_CLS_COPY_MOVE(RootNode);
			
		protected:
			virtual bool IsArgument() override;
			virtual const std::set<std::u8string>& GetConflictSet() override;
			virtual std::u8string GetHelpSymbol() override;
			virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) override;
			virtual void EndConsume(ArgumentsMap& am) override;
		};
		RootNode m_RootNode;
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
