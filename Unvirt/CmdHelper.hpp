#pragma once

#include <VTAll.hpp>
#include <string>
#include <vector>
#include <functional>
#include <deque>
#include <map>
#include <stdexcept>
#include <cinttypes>
#include <initializer_list>
#include <type_traits>
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

#pragma region ArgumentsMap

	namespace ArgumentsMapItem {

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
		ArgumentsMap() : m_Data() {}
		~ArgumentsMap() {}
		YYCC_DEF_CLS_COPY_MOVE(ArgumentsMap);

	protected:
		std::map<std::u8string, std::unique_ptr<ArgumentsMapItem::AbstractItem>> m_Data;

	public:
		template<class _Ty, class... _Types, std::enable_if_t<std::is_base_of_v<ArgumentsMapItem::AbstractItem, _Ty>, int> = 0>
		void Add(const std::u8string_view& key, _Types&&... args) {
			// check argument
			if (key.empty())
				throw std::invalid_argument("argument key should not be empty");
			// insert into data
			auto result = m_Data.try_emplace(std::u8string(key), std::make_unique<_Ty>(std::forward<_Types>(args)...));
			if (!result.second)
				throw std::runtime_error("try to add an existing key.");
		}
		template<class _Ty, std::enable_if_t<std::is_base_of_v<ArgumentsMapItem::AbstractItem, _Ty>, int> = 0>
		const _Ty& Get() const {
			// check argument
			if (key.empty())
				throw std::invalid_argument("argument key should not be empty");
			// find key first
			auto finder = m_Data.find(std::u8string(key));
			if (finder == m_Data.end())
				throw std::runtime_error("try to get a non-existent key.");
			// get stored value data
			const ArgumentsMapItem::AbstractItem& value = *finder->second.get();
			return static_cast<const _Ty&>(value);
		}
		void Remove(const std::u8string_view& key) {
			// check argument
			if (key.empty())
				throw std::invalid_argument("argument key should not be empty");
			// remove and return remove result.
			if (m_Data.erase(std::u8string(key)) == 0u)
				throw std::runtime_error("try to delete a non-existent key.");
		}
	};

#pragma endregion

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

	class AbstractNode {
		friend class CommandRoot;
	public:
		using ExecutionFct = void(*)(const ArgumentsMap&);

	public:
		AbstractNode();
		virtual ~AbstractNode();
		YYCC_DEF_CLS_COPY_MOVE(AbstractNode);

		AbstractNode* Then(AbstractNode*);
		AbstractNode* Executes(ExecutionFct, const char* = nullptr);
		AbstractNode* Comment(const char*);

	protected:
		void Help(HelpDocument& doc);
		bool Consume(CmdSplitter::Result_t& cmds, ArgumentsMap& am);
		virtual bool IsConflictWith(AbstractNode* node) = 0;
		virtual bool IsArgument() = 0;
		virtual std::u8string HelpSymbol() = 0;
		virtual bool BeginConsume(const std::u8string& cur_cmd, ArgumentsMap& am) = 0;
		virtual void EndConsume(ArgumentsMap*) = 0;

	protected:
		std::vector<std::unique_ptr<AbstractNode>> m_Nodes;
		ExecutionFct m_Execution;
		std::string m_ExecutionDesc;
		std::string m_Comment;
	};

	class CommandRoot : public AbstractNode {
	public:
		CommandRoot();
		virtual ~CommandRoot();
		YYCC_DEL_CLS_COPY_MOVE(CommandRoot);

		// Root use special consume and help functions.
		bool RootConsume(std::deque<std::string>&);
		HelpDocument* RootHelp();

	public:
		virtual NodeType GetNodeType() override { throw std::logic_error("Root can not be called."); }
		virtual bool IsConflictWith(AbstractNode*) override { throw std::logic_error("Root can not be called."); }
	protected:
		virtual std::string GetHelpSymbol()  override { throw std::logic_error("Root can not be called."); }
		virtual bool BeginAccept(const std::string&, ArgumentsMap*) override { throw std::logic_error("Root can not be called."); }
		virtual void EndAccept(ArgumentsMap*) override { throw std::logic_error("Root can not be called."); }
	};

	class Literal : public AbstractNode {
		friend class Choice;
		friend class AbstractArgument;
	public:
		Literal(const char* words);
		virtual ~Literal();
		YYCC_DEL_CLS_COPY_MOVE(Literal);

	public:
		virtual NodeType GetNodeType() override;
		virtual bool IsConflictWith(AbstractNode* node) override;
	protected:
		virtual std::string GetHelpSymbol() override;
		virtual bool BeginAccept(const std::string&, ArgumentsMap*) override;
		virtual void EndAccept(ArgumentsMap*) override;

		std::string m_Literal;
	};

	class Choice : public AbstractNode {
		friend class Literal;
		friend class AbstractArgument;
	public:
		using vType = size_t;
		Choice(const char* argname, const std::initializer_list<std::string>& vocabulary);
		virtual ~Choice();
		YYCC_DEL_CLS_COPY_MOVE(Choice);

		vType* GetIndex();

	public:
		virtual NodeType GetNodeType() override;
		virtual bool IsConflictWith(AbstractNode* node) override;
	protected:
		virtual std::string GetHelpSymbol() override;
		virtual bool BeginAccept(const std::string&, ArgumentsMap*) override;
		virtual void EndAccept(ArgumentsMap*) override;

		std::string m_ChoiceName;
		std::vector<std::string> m_Vocabulary;
		bool m_Accepted;
		size_t m_GottenIndex;
	};

	class AbstractArgument : public AbstractNode {
		friend class Literal;
		friend class Choice;
	public:
		AbstractArgument(const char* argname);
		virtual ~AbstractArgument();
		YYCC_DEL_CLS_COPY_MOVE(AbstractArgument);

		template<class T>
		T GetData() {
			return reinterpret_cast<T>(m_ParsedData);
		}

	public:
		virtual NodeType GetNodeType() override;
		virtual bool IsConflictWith(AbstractNode* node) override;
	protected:
		virtual std::string GetHelpSymbol() override;
		virtual bool BeginAccept(const std::string&, ArgumentsMap*) override;
		virtual void EndAccept(ArgumentsMap*) override;

		virtual bool BeginParse(const std::string&) = 0;
		virtual void EndParse() = 0;

		std::string m_ArgName;
		bool m_Accepted;
		void* m_ParsedData;
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
