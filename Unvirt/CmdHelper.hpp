#pragma once

#include <VTAll.hpp>
#include <string>
#include <vector>
#include <functional>
#include <deque>
#include <unordered_map>
#include <stdexcept>
#include <cinttypes>
#include <initializer_list>

namespace Unvirt::CmdHelper {

	class CmdSplitter {
	public:
		enum class StateType : int {
			SPACE,
			SINGLE,
			DOUBLE,
			ESCAPE,
			NORMAL
		};
	public:
		CmdSplitter() :
			mCmdChar(0), mBuffer(nullptr), mResult(nullptr),
			mState(StateType::NORMAL), mPreState(StateType::NORMAL) {}
		~CmdSplitter() {}
		LIBCMO_DISABLE_COPY_MOVE(CmdSplitter);

		std::deque<std::string> Convert(const std::string& u8cmd);
	protected:
		char mCmdChar;
		std::string* mBuffer;
		std::deque<std::string>* mResult;

		StateType mState, mPreState;

		void ProcSpace(void);
		void ProcSingle(void);
		void ProcDouble(void);
		void ProcEscape(void);
		void ProcNormal(void);
	};

	class HelpDocument {
	public:
		HelpDocument();
		~HelpDocument();
		LIBCMO_DISABLE_COPY_MOVE(HelpDocument);

		void Push(const std::string& arg_name, const std::string& arg_desc);
		void Pop();
		void Terminate(std::string& command_desc);
		void Print();

	protected:
		struct StackItem {
			StackItem() : m_Name(), m_Desc() {}
			StackItem(const std::string& name, const std::string& desc) : m_Name(name), m_Desc(desc) {}
			LIBCMO_DEFAULT_COPY_MOVE(StackItem);
			std::string m_Name;
			std::string m_Desc;
		};
		std::deque<StackItem> m_Stack;
		struct ResultItem {
			ResultItem() : m_CmdDesc(), m_ArgDesc() {}
			ResultItem(const std::string& desc) : m_CmdDesc(desc), m_ArgDesc() {}
			LIBCMO_DEFAULT_COPY_MOVE(ResultItem);
			std::string m_CmdDesc;
			std::vector<StackItem> m_ArgDesc;
		};
		std::vector<ResultItem> m_Results;
	};

	enum class NodeType {
		Literal, Choice, Argument
	};
	class ArgumentsMap;
	using ExecutionFct = std::function<void(const ArgumentsMap*)>;
	class AbstractNode {
	public:
		AbstractNode();
		virtual ~AbstractNode();
		LIBCMO_DISABLE_COPY_MOVE(AbstractNode);

		AbstractNode* Then(AbstractNode*);
		AbstractNode* Executes(ExecutionFct, const char* = nullptr);
		AbstractNode* Comment(const char*);

	public:
		void Help(HelpDocument*);
		bool Consume(std::deque<std::string>&, ArgumentsMap*);
		virtual NodeType GetNodeType() = 0;
		virtual bool IsConflictWith(AbstractNode*) = 0;
	protected:
		virtual std::string GetHelpSymbol() = 0;
		virtual bool BeginAccept(const std::string&, ArgumentsMap*) = 0;
		virtual void EndAccept(ArgumentsMap*) = 0;

		std::vector<AbstractNode*> m_Literals;
		std::vector<AbstractNode*> m_Choices;
		std::vector<AbstractNode*> m_Args;
		ExecutionFct m_Execution;
		std::string m_ExecutionDesc;
		std::string m_Comment;
	};

	class CommandRoot : public AbstractNode {
	public:
		CommandRoot();
		virtual ~CommandRoot();
		LIBCMO_DISABLE_COPY_MOVE(CommandRoot);

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
		LIBCMO_DISABLE_COPY_MOVE(Literal);

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
		using vType = int32_t;
		Choice(const char* argname, const std::initializer_list<std::string>& vocabulary);
		virtual ~Choice();
		LIBCMO_DISABLE_COPY_MOVE(Choice);

		size_t* GetIndex();

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
		LIBCMO_DISABLE_COPY_MOVE(AbstractArgument);

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
		LIBCMO_DISABLE_COPY_MOVE(IntArgument);

	protected:
		virtual bool BeginParse(const std::string&) override;
		virtual void EndParse() override;

		IntLimit m_IntLimit;
	};

	class StringArgument : public AbstractArgument {
	public:
		using vType = std::string;
		StringArgument(const char* argname) : AbstractArgument(argname) {}
		virtual ~StringArgument() {}
		LIBCMO_DISABLE_COPY_MOVE(StringArgument);

	protected:
		virtual bool BeginParse(const std::string&) override;
		virtual void EndParse() override;
	};

	class EncodingArgument : public AbstractArgument {
	public:
		using vType = std::vector<std::string>;
		EncodingArgument(const char* argname) : AbstractArgument(argname) {}
		virtual ~EncodingArgument() {}
		LIBCMO_DISABLE_COPY_MOVE(EncodingArgument);

	protected:
		virtual bool BeginParse(const std::string&) override;
		virtual void EndParse() override;
	};

	class ArgumentsMap {
	public:
		ArgumentsMap() : m_Data() {}
		~ArgumentsMap() {}
		LIBCMO_DISABLE_COPY_MOVE(ArgumentsMap);

		void Add(const std::string& k, AbstractNode* v);
		void Remove(const std::string& k);

		template<class _Ty>
		_Ty* Get(const char* k) const {
			if (k == nullptr) throw std::invalid_argument("Null argument name.");
			std::string conv(k);

			auto finder = m_Data.find(conv);
			if (finder == m_Data.end()) throw std::invalid_argument("No such argument name.");
			AbstractNode* node = finder->second;
			switch (node->GetNodeType()) {
				case NodeType::Argument:
					return dynamic_cast<AbstractArgument*>(node)->GetData<_Ty*>();
				case NodeType::Choice:
					return dynamic_cast<Choice*>(node)->GetIndex();
				case NodeType::Literal:
				default:
					throw std::runtime_error("No such argument type.");
			}
		}

	protected:
		std::unordered_map<std::string, AbstractNode*> m_Data;
	};

}
