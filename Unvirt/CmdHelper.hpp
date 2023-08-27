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
		HelpDocument() : m_Stack(), m_Results() {}
		~HelpDocument() {}
		LIBCMO_DISABLE_COPY_MOVE(HelpDocument);

		void PushLiteral(const std::string& literal_name) {
			m_Stack.emplace_back(StackItem(literal_name, ""));
		}
		void PushArgument(const std::string& arg_name, const std::string& arg_desc) {
			m_Stack.emplace_back(StackItem(arg_name, arg_desc));
		}
		void Pop() {
			m_Stack.pop_back();
		}
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

	class AbstractArgument;
	class ArgumentsMap {
	public:
		ArgumentsMap() : m_Data() {}
		~ArgumentsMap() {}
		LIBCMO_DISABLE_COPY_MOVE(ArgumentsMap);

		void Add(const std::string& k, AbstractArgument* v) {
			m_Data.emplace(std::make_pair(k, v));
		}
		void Remove(const std::string& k) {
			m_Data.erase(k);
		}
		AbstractArgument* Get(const char* k) const {
			auto finder = m_Data.find(k);
			if (finder == m_Data.end()) throw std::invalid_argument("No such argument name.");
			return finder->second;
		}

	protected:
		std::unordered_map<std::string, AbstractArgument*> m_Data;
	};

	class CommandRoot;
	using ExecutionFct = std::function<void(const ArgumentsMap&)>;
	class AbstractNode {
		friend class CommandRoot;
	public:
		AbstractNode() : m_Execution(nullptr), m_Nodes(), m_Comment() {}
		virtual ~AbstractNode() {
			for (auto& ptr : m_Nodes) {
				delete ptr;
			}
		}
		LIBCMO_DISABLE_COPY_MOVE(AbstractNode);

		AbstractNode* Then(AbstractNode*);
		AbstractNode* Executes(ExecutionFct, const char*);

	protected:
		void Help(HelpDocument&);
		bool Consume(std::deque<std::string>&, ArgumentsMap&);
		virtual bool IsLiteral() = 0;
		virtual bool IsConflictWith(AbstractNode*) = 0;
		virtual void BeginHelp(HelpDocument&) = 0;
		virtual void EndHelp(HelpDocument&) = 0;
		virtual bool BeginAccept(const std::string&, ArgumentsMap&) = 0;
		virtual void EndAccept(ArgumentsMap&) = 0;

		std::vector<AbstractNode*> m_Nodes;
		ExecutionFct m_Execution;
		std::string m_Comment;
	};

	class CommandRoot : public AbstractNode {
	public:
		CommandRoot() : AbstractNode() {}
		virtual ~CommandRoot() {}
		LIBCMO_DISABLE_COPY_MOVE(CommandRoot);

		// Root use special consume and help functions.
		bool RootConsume(std::deque<std::string>&);
		HelpDocument* RootHelp();

	protected:
		virtual bool IsLiteral() override { throw std::logic_error("Root can not be called."); }
		virtual bool IsConflictWith(AbstractNode*) override { throw std::logic_error("Root can not be called."); }
		virtual void BeginHelp(HelpDocument&)  override { throw std::logic_error("Root can not be called."); }
		virtual void EndHelp(HelpDocument&) override { throw std::logic_error("Root can not be called."); }
		virtual bool BeginAccept(const std::string&, ArgumentsMap&) override { throw std::logic_error("Root can not be called."); }
		virtual void EndAccept(ArgumentsMap&) override { throw std::logic_error("Root can not be called."); }
	};

	class Literal : public AbstractNode {
		friend class Choice;
	public:
		Literal(const char* words) : AbstractNode(), m_Literal(words) {
			if (m_Literal.empty() || words == nullptr) throw std::invalid_argument("Invalid literal.");
		}
		virtual ~Literal() {}
		LIBCMO_DISABLE_COPY_MOVE(Literal);

	protected:
		virtual bool IsLiteral() override { return true; }
		virtual bool IsConflictWith(AbstractNode* node) override {
			Literal* pliteral = dynamic_cast<Literal*>(node);
			if (pliteral == nullptr) return false;
			return pliteral->m_Literal == this->m_Literal;
		}
		virtual void BeginHelp(HelpDocument& doc) override {
			doc.PushLiteral(m_Literal);
		}
		virtual void EndHelp(HelpDocument& doc) override {
			doc.Pop();
		}
		virtual bool BeginAccept(const std::string& strl, ArgumentsMap&) override { return strl == m_Literal; }
		virtual void EndAccept(ArgumentsMap&) override {}

		std::string m_Literal;
	};

	class Choice : public AbstractNode {
	public:
		Choice(const char* argname, const std::initializer_list<std::string>& vocabulary) :
			AbstractNode(), m_ChoiceName(argname == nullptr ? "" : argname), m_Vocabulary(vocabulary) {
			if (m_ChoiceName.empty() || argname == nullptr) throw std::invalid_argument("Invalid choice name.");
			if (m_Vocabulary.size() < 2) throw std::invalid_argument("Too less vocabulary. At least 2 items.");
		}
		virtual ~Choice() {}
		LIBCMO_DISABLE_COPY_MOVE(Choice)

	protected:
		virtual bool IsLiteral() override { return true; }
		virtual bool IsConflictWith(AbstractNode* node) override {
			Literal* pliteral = dynamic_cast<Literal*>(node);
			if (pliteral != nullptr) {
				for (const auto& word : m_Vocabulary) {
					if (word == pliteral->m_Literal)
						return true;
				}
				return false;
			}

			Choice* pchoice = dynamic_cast<Choice*>(node);
			if (pchoice != nullptr) {
				for (const auto& thisword : m_Vocabulary) {
					for (const auto& thatword : pchoice->m_Vocabulary) {
						if (thisword == thatword)
							return true;
					}
				}
				return false;
			}

			return false;
		}
		virtual void BeginHelp(HelpDocument& doc) override {
			std::string switches;
			for (const auto& item : m_Vocabulary) {
				if (!switches.empty()) switches += " | ";
				switches += item;
			}
			doc.PushLiteral("[" + switches + "]");
		}
		virtual void EndHelp(HelpDocument& doc) override {
			doc.Pop();
		}
		virtual bool BeginAccept(const std::string& strl, ArgumentsMap& amap) override { 
			for (const auto& item : m_Vocabulary) {
				if (strl == item) {

				}
			}
		}
		virtual void EndAccept(ArgumentsMap&) override {}

		std::string m_ChoiceName;
		std::vector<std::string> m_Vocabulary;
	};

	class AbstractArgument : public AbstractNode {
	public:
		AbstractArgument(const char* argname, const char* argdesc) : AbstractNode(), 
			m_ArgName(argname == nullptr ? "" : argname), m_ArgDesc(argdesc), m_Accepted(false), m_ParsedData(nullptr) {
			if (m_ArgName.empty() || argname == nullptr) throw std::invalid_argument("Invalid argument name.");
		}
		virtual ~AbstractArgument() {}
		LIBCMO_DISABLE_COPY_MOVE(AbstractArgument);

		template<class T>
		T* GetData() { return reinterpret_cast<T*>(m_ParsedData); }

	protected:
		virtual bool BeginParse(const std::string&) = 0;
		virtual void EndParse() = 0;

		virtual bool IsLiteral() override { return false; }
		virtual bool IsConflictWith(AbstractNode* node) override {
			return false;
		}
		virtual void BeginHelp(HelpDocument& doc) override {
			std::string newargname = "<";
			newargname.append(m_ArgName);
			newargname.append(">");
			doc.PushArgument(newargname, m_ArgDesc);
		}
		virtual void EndHelp(HelpDocument& doc) override {
			doc.Pop();
		}
		virtual bool BeginAccept(const std::string& strl, ArgumentsMap& amap) override {
			m_Accepted = BeginParse(strl);
			if (m_Accepted) amap.Add(m_ArgName, this);
			return m_Accepted;
		}
		virtual void EndAccept(ArgumentsMap& amap) override {
			if (m_Accepted) {
				amap.Remove(m_ArgName);
				EndParse();
				m_Accepted = false;
			}
		}

		std::string m_ArgName;
		std::string m_ArgDesc;
		bool m_Accepted;
		void* m_ParsedData;
	};

	/**
	 * @brief Return true mean this value can accept.
	*/
	using IntLimit = std::function<bool(int32_t)>;
	class IntArgument : public AbstractArgument {
	public:
		IntArgument(const char* argname, const char* argdesc = nullptr, IntLimit limit = nullptr) :
			AbstractArgument(argname, argdesc), m_IntLimit(limit) {}
		virtual ~IntArgument() {}
		LIBCMO_DISABLE_COPY_MOVE(IntArgument);

	protected:
		virtual bool BeginParse(const std::string&) override;
		virtual void EndParse() override;

		IntLimit m_IntLimit;
	};

	class StringArgument : public AbstractArgument {
	public:
		StringArgument(const char* argname, const char* argdesc = nullptr) : AbstractArgument(argname, argdesc) {}
		virtual ~StringArgument() {}
		LIBCMO_DISABLE_COPY_MOVE(StringArgument);

	protected:
		virtual bool BeginParse(const std::string&) override;
		virtual void EndParse() override;
	};

	class EncodingArgument : public AbstractArgument {
	public:
		EncodingArgument(const char* argname, const char* argdesc = nullptr) : AbstractArgument(argname, argdesc) {}
		virtual ~EncodingArgument() {}
		LIBCMO_DISABLE_COPY_MOVE(EncodingArgument);

	protected:
		virtual bool BeginParse(const std::string&) override;
		virtual void EndParse() override;
	};


}
