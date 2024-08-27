#include "UnvirtContext.hpp"

#include <YYCCommonplace.hpp>
#include <CK2/MgrImpls/CKPathManager.hpp>
#include <cstdarg>
#include <regex>

namespace Unvirt::Context {

#pragma region Specialized for CmdHelper

	class EncodingListArgument : public CmdHelper::Nodes::AbstractArgument {
	public:
		using ArgValue_t = CmdHelper::AMItems::StringArrayItem;
	public:
		EncodingListArgument(const std::u8string_view& argname) :
			AbstractArgument(argname) {}
		virtual ~EncodingListArgument() {}
		YYCC_DEF_CLS_COPY_MOVE(EncodingListArgument);

	protected:
		virtual bool BeginConsume(const std::u8string& cur_cmd, CmdHelper::ArgumentsMap& am) override {
			// split given argument
			std::vector<std::u8string> encs = YYCC::StringHelper::Split(cur_cmd, u8",");
			// check each parts is a valid encoding name
			for (const auto& item : encs) {
				if (!LibCmo::EncodingHelper::IsValidEncodingName(item))
					return false;
			}
			// okey, add into map
			am.Add<ArgValue_t>(m_ArgumentName, encs);
			return true;
		}
	};

	static YYCC::Constraints::Constraint<size_t> GetOneBasedIndexConstraint() {
		return YYCC::Constraints::Constraint<size_t> {
			[](const size_t& val) -> bool {
				return val > 0u;
			}
		};
	}

#pragma endregion

#pragma region UnvirtContext Misc

	UnvirtContext::UnvirtContext() :
		m_Splitter(), m_Parser(), m_Help(),
		m_PageLen(10u), m_ListStyleIsFull(true), m_OrderExit(false),
		m_SearchPart(SearchPart::None), m_SearchIdxResult(),
		m_Ctx(nullptr), m_FileReader(nullptr), m_IsShallowRead(true) {

		// Setup command parser
		// get root node first
		CmdHelper::Nodes::RootNode& root = m_Parser.GetRoot();
		// setup tree
		root
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"load")
			.Then<CmdHelper::Nodes::Choice>(CmdHelper::Nodes::Choice(u8"stage", { u8"deep", u8"shallow"})
				.Comment(u8"The stage of loading. 'deep' will load to CKObject stage. 'shallow' will load to CKStateChunk stage.")
				.Then<CmdHelper::Nodes::StringArgument>(CmdHelper::Nodes::StringArgument(u8"filepath")
					.Comment(u8"The path to loading file.")
					.Executes(
						std::bind(&UnvirtContext::ProcLoad, this, std::placeholders::_1),
						u8"Load a Virtools composition."
					)
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"unload")
			.Executes(
				std::bind(&UnvirtContext::ProcUnLoad, this, std::placeholders::_1),
				u8"Release loaded Virtools composition."
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"save")
			.Then<CmdHelper::Nodes::StringArgument>(CmdHelper::Nodes::StringArgument(u8"filepath")
				.Comment(u8"The path to save file.")
				.Executes(
					std::bind(&UnvirtContext::ProcSave, this, std::placeholders::_1),
					u8"Save the loaded file into a new file."
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"info")
			.Executes(
				std::bind(&UnvirtContext::ProcInfo, this, std::placeholders::_1),
				u8"Show the header info of loaded Virtools composition."
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"ls")
			.Then<CmdHelper::Nodes::Choice>(CmdHelper::Nodes::Choice(u8"part", { u8"obj", u8"mgr", u8"search"})
				.Comment(u8"Which list you want to list.")
				.Then<CmdHelper::Nodes::ArithmeticArgument<size_t>>(CmdHelper::Nodes::ArithmeticArgument<size_t>(u8"page", GetOneBasedIndexConstraint())
					.Comment(u8"The page index. Start with 1.")
					.Executes(
						std::bind(&UnvirtContext::ProcLs, this, std::placeholders::_1),
						u8"List something about loaded Virtools composition."
					)
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"data")
			.Then<CmdHelper::Nodes::Choice>(CmdHelper::Nodes::Choice(u8"part", { u8"obj", u8"mgr"})
				.Comment(u8"Which list you want to show data.")
				.Then<CmdHelper::Nodes::ArithmeticArgument<size_t>>(CmdHelper::Nodes::ArithmeticArgument<size_t>(u8"index")
					.Comment(u8"The index. Start with 0.")
					.Executes(
						std::bind(&UnvirtContext::ProcData, this, std::placeholders::_1),
						u8"Show the specific CKObject / CKBaseManager data."
					)
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"chunk")
			.Then<CmdHelper::Nodes::Choice>(CmdHelper::Nodes::Choice(u8"part", { u8"obj", u8"mgr"})
				.Comment(u8"Which list you want to show chunk.")
				.Then<CmdHelper::Nodes::ArithmeticArgument<size_t>>(CmdHelper::Nodes::ArithmeticArgument<size_t>(u8"index")
					.Comment(u8"The index. Start with 0.")
					.Executes(
						std::bind(&UnvirtContext::ProcChunk, this, std::placeholders::_1),
						u8"Show the specific CKStateChunk data."
					)
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"search")
			.Then<CmdHelper::Nodes::Choice>(CmdHelper::Nodes::Choice(u8"part", { u8"obj", u8"mgr"})
				.Comment(u8"Which list you want to search.")
				.Then<CmdHelper::Nodes::Choice>(CmdHelper::Nodes::Choice(u8"mode", { u8"plain", u8"re"})
					.Comment(u8"The search mode. `plain` will search by substring and `re` will do regex search.")
					.Then<CmdHelper::Nodes::StringArgument>(CmdHelper::Nodes::StringArgument(u8"text")
						.Comment(u8"The text or regex to search.")
						.Executes(
							std::bind(&UnvirtContext::ProcSearch, this, std::placeholders::_1),
							u8"Search object or manager by text or regex. Please note that the regex have limited UTF8 support and may cause undefined behavior."
						)
					)
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"items")
			.Then<CmdHelper::Nodes::ArithmeticArgument<size_t>>(CmdHelper::Nodes::ArithmeticArgument<size_t>(u8"count", GetOneBasedIndexConstraint())
				.Comment(u8"The count of items you want to show in one page.")
				.Executes(
					std::bind(&UnvirtContext::ProcItems, this, std::placeholders::_1),
					u8"Set up how many items should be listed in one page when using 'ls' command."
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"style")
			.Then<CmdHelper::Nodes::Choice>(CmdHelper::Nodes::Choice(u8"level", { u8"full", u8"simple"})
				.Comment(u8"The amount of showen content.")
				.Executes(
					std::bind(&UnvirtContext::ProcStyle, this, std::placeholders::_1),
					u8"Change the detail level of showen data in `ls` command."
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"encoding")
			.Then<EncodingListArgument>(EncodingListArgument(u8"enc")
				.Comment(u8"CKContext used encoding splitted by ','. Support mutiple encoding.")
				.Executes(
					std::bind(&UnvirtContext::ProcEncoding, this, std::placeholders::_1),
					u8"Set the encoding series for CKContext."
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"temp")
			.Then<CmdHelper::Nodes::StringArgument>(CmdHelper::Nodes::StringArgument(u8"temppath")
				.Comment(u8"The path to Temp folder.")
				.Executes(
					std::bind(&UnvirtContext::ProcTemp, this, std::placeholders::_1),
					u8"Set the Temp path for CKContext."
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"rsc")
			.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"clear")
				.Executes(
					std::bind(&UnvirtContext::ProcRsc, this, std::placeholders::_1, true),
					u8"Clear all data resources paths."
				)
			)
			.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"add")
				.Then<CmdHelper::Nodes::StringArgument>(CmdHelper::Nodes::StringArgument(u8"datares")
					.Comment(u8"The data resources path .")
					.Executes(
						std::bind(&UnvirtContext::ProcRsc, this, std::placeholders::_1, false),
						u8"Add a path to let Virtools find resources."
					)
				)
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"test")
			.Executes(
				std::bind(&UnvirtContext::ProcTest, this, std::placeholders::_1),
				u8"Call custom debugging function (only available in Debug mode)."
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"help")
			.Executes(
				std::bind(&UnvirtContext::ProcHelp, this, std::placeholders::_1),
				u8"Output this help page."
			)
		)
		.Then<CmdHelper::Nodes::Literal>(CmdHelper::Nodes::Literal(u8"exit")
			.Executes(
				std::bind(&UnvirtContext::ProcExit, this, std::placeholders::_1),
				u8"Exit program."
			)
		);

		// create help document
		m_Help = m_Parser.Help();

		// initialize CK engine and create context
		LibCmo::CK2::CKERROR err = LibCmo::CK2::CKStartUp();
		if (err != LibCmo::CK2::CKERROR::CKERR_OK)
			throw std::runtime_error("fail to initialize CK2 engine.");
		m_Ctx = new LibCmo::CK2::CKContext();
		m_Ctx->SetOutputCallback(std::bind(&UnvirtContext::PrintContextMsg, this, std::placeholders::_1));
		m_Ctx->SetGlobalImagesSaveOptions(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_EXTERNAL);
	}

	UnvirtContext::~UnvirtContext() {
		// free context and shutdown CK engine.
		ClearDocument();
		delete m_Ctx;
		LibCmo::CK2::CKShutdown();
	}

	bool UnvirtContext::HasOpenedFile() {
		return m_FileReader != nullptr;
	}

	void UnvirtContext::ClearDocument() {
		if (m_FileReader == nullptr) return;
		// clear search result
		m_SearchPart = SearchPart::None;
		m_SearchIdxResult.clear();
		// delete reader
		delete m_FileReader;
		m_FileReader = nullptr;
		m_IsShallowRead = true;
		// clear context
		m_Ctx->ClearAll();
	}

	void UnvirtContext::PrintContextMsg(LibCmo::CKSTRING msg) {
		if (msg != nullptr) {
			YYCC::ConsoleHelper::FormatLine(YYCC_COLOR_LIGHT_YELLOW(u8"[CKContext] ") "%s", msg);
		}
	}

	void UnvirtContext::PrintCommonInfo(const char8_t* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		YYCC::ConsoleHelper::WriteLine(
			YYCC::StringHelper::VPrintf(u8_fmt, argptr).c_str()
		);
		va_end(argptr);
	}

	void UnvirtContext::PrintCommonError(const char8_t* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		YYCC::ConsoleHelper::FormatLine(YYCC_COLOR_LIGHT_RED(u8"%s"),
			YYCC::StringHelper::VPrintf(u8_fmt, argptr).c_str()
		);
		va_end(argptr);
	}

#pragma endregion

#pragma region Main Run

	void UnvirtContext::Run() {
		// Enable terminal color feature
		YYCC::ConsoleHelper::EnableColorfulConsole();

		// Show banner
		YYCC::ConsoleHelper::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"Unvirt"));
		YYCC::ConsoleHelper::WriteLine(YYCC_COLOR_LIGHT_YELLOW(u8"Type 'help' for more infomation. Type 'exit' to quit."));

		// start process loop
		while (true) {
			// get command
			YYCC::ConsoleHelper::Write(YYCC_COLOR_GREEN(u8"> "));
			std::u8string u8cmd = YYCC::ConsoleHelper::ReadLine();

			// lex command line first
			if (!m_Splitter.Lex(u8cmd)) {
				this->PrintCommonError(u8"Lexer error \"%s\".\nType 'help' for usage.", u8cmd.c_str());
				continue;
			}
			// if result is empty, skip to next one
			auto cmds = m_Splitter.GetResult();
			if (cmds.empty()) continue;

			// run command parser
			if (!m_Parser.Parse(cmds)) {
				this->PrintCommonError(u8"Parser error \"%s\".\nType 'help' for usage.", u8cmd.c_str());
			}

			// check whether sub processor need exit.
			if (m_OrderExit) {
				break;
			}
		}
	}

#pragma endregion

#pragma region Proc Detail

	void UnvirtContext::ProcLoad(const CmdHelper::ArgumentsMap& amap) {
		// check pre-requirement
		if (HasOpenedFile()) {
			PrintCommonError(u8"Already have a opened file. Close it before calling 'load'.");
			return;
		}
		if (!m_Ctx->IsValidEncoding()) {
			PrintCommonError(u8"You have not set encoding properly. Set it before loading by calling 'encoding'.");
			return;
		}

		// get argument
		std::u8string filepath = amap.Get<CmdHelper::Nodes::StringArgument::ArgValue_t>(u8"filepath").Get();
		bool is_deep = amap.Get<CmdHelper::Nodes::Choice::ArgValue_t>(u8"stage").Get() == 0u;

		// proc
		m_FileReader = new LibCmo::CK2::CKFileReader(m_Ctx);
		LibCmo::CK2::CKERROR err;
		if (is_deep) {
			err = m_FileReader->DeepLoad(filepath.c_str());
			m_IsShallowRead = false;
		} else {
			err = m_FileReader->ShallowLoad(filepath.c_str());
			m_IsShallowRead = true;
		}
		if (err != LibCmo::CK2::CKERROR::CKERR_OK) {
			// fail to load. release all.
			PrintCommonError(u8"Fail to load file. Function return: %s\n\t%s",
				Unvirt::AccessibleValue::GetCkErrorName(err).c_str(),
				Unvirt::AccessibleValue::GetCkErrorDescription(err).c_str()
			);

			ClearDocument();
		}
	}

	void UnvirtContext::ProcUnLoad(const CmdHelper::ArgumentsMap&) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			this->PrintCommonError(u8"No loaded file.");
			return;
		}

		// free all
		ClearDocument();
	}

	void Unvirt::Context::UnvirtContext::ProcSave(const CmdHelper::ArgumentsMap& amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError(u8"No loaded file.");
			return;
		}
		if (!m_Ctx->IsValidEncoding()) {
			PrintCommonError(u8"You have not set encoding properly. Set it before saving by calling 'encoding'.");
			return;
		}

		std::u8string filepath = amap.Get<CmdHelper::Nodes::StringArgument::ArgValue_t>(u8"filepath").Get();

		// construct writer from reader
		LibCmo::CK2::CKFileWriter writer(m_Ctx, m_FileReader, m_IsShallowRead);

		// backup current file write mode
		// and replace it with reader
		LibCmo::CK2::CK_FILE_WRITEMODE backup = m_Ctx->GetFileWriteMode();
		m_Ctx->SetFileWriteMode(m_FileReader->GetFileInfo().FileWriteMode);

		// run writer
		LibCmo::CK2::CKERROR err = writer.Save(filepath.c_str());
		if (err != LibCmo::CK2::CKERROR::CKERR_OK) {
			// fail to load. release all.
			PrintCommonError(u8"Fail to save file. Function return: %s\n\t%s",
				Unvirt::AccessibleValue::GetCkErrorName(err).c_str(),
				Unvirt::AccessibleValue::GetCkErrorDescription(err).c_str()
			);
		}

		// restore file write mode
		m_Ctx->SetFileWriteMode(backup);

	}

	void UnvirtContext::ProcInfo(const CmdHelper::ArgumentsMap&) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError(u8"No loaded file.");
			return;
		}

		// print
		Unvirt::StructFormatter::PrintCKFileInfo(m_FileReader->GetFileInfo());
	}

	void UnvirtContext::ProcLs(const CmdHelper::ArgumentsMap& amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError(u8"No loaded file.");
			return;
		}

		// get 0 based page (-1)
		size_t page = amap.Get<CmdHelper::Nodes::ArithmeticArgument<size_t>::ArgValue_t>(u8"page").Get() - 1u;

		// show list
		switch (amap.Get<CmdHelper::Nodes::Choice::ArgValue_t>(u8"part").Get()) {
			case 0u:
			{
				// obj list
				Unvirt::StructFormatter::PrintObjectList(
					m_FileReader->GetFileObjects(),
					m_FileReader->GetFileInfo(),
					page, this->m_PageLen,
					m_ListStyleIsFull
				);
				break;
			}
			case 1u:
			{
				// mgr list
				Unvirt::StructFormatter::PrintManagerList(
					m_FileReader->GetManagersData(),
					page, this->m_PageLen,
					m_ListStyleIsFull
				);
				break;
			}
			case 2u:
			{
				// search list
				switch (m_SearchPart) {
					case SearchPart::None:
					{
						PrintCommonError(u8"No search result to list.");
						break;
					}
					case SearchPart::Object:
					{
						Unvirt::StructFormatter::PrintSearchedObjectList(
							m_SearchIdxResult,
							m_FileReader->GetFileObjects(),
							m_FileReader->GetFileInfo(),
							page, this->m_PageLen,
							m_ListStyleIsFull
						);
						break;
					}
					case SearchPart::Manager:
					{
						Unvirt::StructFormatter::PrintSearchedManagerList(
							m_SearchIdxResult,
							m_FileReader->GetManagersData(),
							page, this->m_PageLen,
							m_ListStyleIsFull
						);
						break;
					}
				}
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}
	}

	void UnvirtContext::ProcData(const CmdHelper::ArgumentsMap& amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError(u8"No loaded file.");
			return;
		}

		// get index
		size_t index = amap.Get<CmdHelper::Nodes::ArithmeticArgument<size_t>::ArgValue_t>(u8"index").Get();

		// show data
		switch (amap.Get<CmdHelper::Nodes::Choice::ArgValue_t>(u8"part").Get()) {
			case 0u:
			{
				if (index >= m_FileReader->GetFileObjects().size()) {
					PrintCommonError(u8"Index out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintCKObject(m_FileReader->GetFileObjects()[index].ObjPtr);
				break;
			}
			case 1u:
			{
				if (index >= m_FileReader->GetManagersData().size()) {
					PrintCommonError(u8"Index out of range.");
					return;
				}
				// todo: finish manager display
				PrintCommonError(u8"Not supported now.");
				//Unvirt::StructFormatter::PrintCKBaseManager(m_FileReader->GetManagersData()[index].Data);
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}
	}

	void UnvirtContext::ProcChunk(const CmdHelper::ArgumentsMap& amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError(u8"No loaded file.");
			return;
		}

		// get index
		size_t index = amap.Get<CmdHelper::Nodes::ArithmeticArgument<size_t>::ArgValue_t>(u8"index").Get();

		// show data
		switch (amap.Get<CmdHelper::Nodes::Choice::ArgValue_t>(u8"part").Get()) {
			case 0:
			{
				if (index >= m_FileReader->GetFileObjects().size()) {
					PrintCommonError(u8"Index out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintCKStateChunk(m_FileReader->GetFileObjects()[index].Data);
				break;
			}
			case 1:
			{
				if (index >= m_FileReader->GetManagersData().size()) {
					PrintCommonError(u8"Index out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintCKStateChunk(m_FileReader->GetManagersData()[index].Data);
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}
	}

	void UnvirtContext::ProcSearch(const CmdHelper::ArgumentsMap& amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError(u8"No loaded file.");
			return;
		}

		// get search text *amap->Get<CmdHelper::StringArgument::vType>("text")
		std::u8string search_text = amap.Get<CmdHelper::Nodes::StringArgument::ArgValue_t>(u8"text").Get();

		// analyse search mode
		std::function<bool(const LibCmo::XContainer::XString&)> search_fct = nullptr;
		switch (amap.Get<CmdHelper::Nodes::Choice::ArgValue_t>(u8"mode").Get()) {
			case 0u:
			{
				// plain mode
				search_fct = [&search_text](const LibCmo::XContainer::XString& cmp) -> bool {
					return cmp.find(search_text) != std::string::npos;
				};
				break;
			}
			case 1u:
			{
				// regex mode

				// get ordinary search text
				std::string ordinary_search_text = YYCC::EncodingHelper::ToOrdinary(search_text);
				// try construct regex
				std::regex re;
				try {
					re = std::regex(ordinary_search_text, std::regex_constants::ECMAScript);
				} catch (const std::regex_error& e) {
					PrintCommonError(u8"Invalid regular expressions: %s", e.what());
					return;
				}

				// use copy ctor capture to input regex
				// because re will be freed when exiting this switch.
				search_fct = [re](const LibCmo::XContainer::XString& cmp) -> bool {
					// get ordinary name and comapre
					std::string ordinary_cmp = YYCC::EncodingHelper::ToOrdinary(cmp);
					return std::regex_search(ordinary_cmp, re);
				};
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}

		// start search
		switch (amap.Get<CmdHelper::Nodes::Choice::ArgValue_t>(u8"part").Get()) {
			case 0u:
			{
				// object
				m_SearchPart = SearchPart::Object;
				m_SearchIdxResult.clear();

				size_t counter = 0;
				for (const auto& obj : m_FileReader->GetFileObjects()) {
					if (search_fct(obj.Name)) {
						m_SearchIdxResult.emplace_back(counter);
					}
					++counter;
				}

				break;
			}
			case 1u:
			{
				// manager
				m_SearchPart = SearchPart::Manager;
				m_SearchIdxResult.clear();
				PrintCommonError(u8"Not supported now.");
				// todo: remove this return when fixing manager searching.
				return;
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}

		// report search result
		if (m_SearchIdxResult.empty()) {
			PrintCommonInfo(u8"Search done, but no result.");
		} else {
			PrintCommonInfo(u8"Search done with %" PRIuSIZET " results. Use `ls search` to check them.", m_SearchIdxResult.size());
		}
	}

	void UnvirtContext::ProcItems(const CmdHelper::ArgumentsMap& amap) {
		// assign
		m_PageLen = amap.Get<CmdHelper::Nodes::ArithmeticArgument<size_t>::ArgValue_t>(u8"count").Get();
	}

	void UnvirtContext::ProcStyle(const CmdHelper::ArgumentsMap& amap) {
		// set list style level
		switch (amap.Get<CmdHelper::Nodes::Choice::ArgValue_t>(u8"level").Get()) {
			case 0u:
			{
				// full level
				m_ListStyleIsFull = true;
				break;
			}
			case 1u:
			{
				// simple level
				m_ListStyleIsFull = false;
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}
	}

	void UnvirtContext::ProcEncoding(const CmdHelper::ArgumentsMap& amap) {
		const auto& encodings = amap.Get<EncodingListArgument::ArgValue_t>(u8"enc").Get();
		m_Ctx->SetEncoding(encodings);
	}

	void UnvirtContext::ProcTemp(const CmdHelper::ArgumentsMap& amap) {
		// assign
		std::u8string temp_path = amap.Get<CmdHelper::Nodes::StringArgument::ArgValue_t>(u8"temppath").Get();
		if (!m_Ctx->GetPathManager()->SetTempFolder(temp_path.c_str())) {
			PrintCommonError(u8"Set temp folder failed. Check your path first.");
		}
	}

	void Unvirt::Context::UnvirtContext::ProcRsc(const CmdHelper::ArgumentsMap& amap, bool is_clear) {
		if (is_clear) {
			m_Ctx->GetPathManager()->ClearPath();
		} else {
			std::u8string data_res = amap.Get<CmdHelper::Nodes::StringArgument::ArgValue_t>(u8"datares").Get();
			if (!m_Ctx->GetPathManager()->AddPath(data_res.c_str())) {
				PrintCommonError(u8"Set data resource folder failed. Check your path first.");
			}
		}
	}

	void Unvirt::Context::UnvirtContext::ProcTest(const CmdHelper::ArgumentsMap& amap) {
#if defined(LIBCMO_BUILD_DEBUG)
		// MARK: Add the debug code here.

		// todo: temporaryly write Transparent Column Fixer code.
		// move to independent app in future.

		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError(u8"No loaded file.");
			return;
		}
		if (!m_IsShallowRead) {
			PrintCommonError(u8"Transparent Column Fixer only accept shallow loaded file.");
			return;
		}

		// iterate objects
		LibCmo::CKDWORD expcode = static_cast<LibCmo::CKDWORD>(LibCmo::CK2::CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_TEXONLY);
		LibCmo::CKDWORD modcode = static_cast<LibCmo::CKDWORD>(LibCmo::CK2::CK_STATESAVEFLAGS_TEXTURE::CK_STATESAVE_OLDTEXONLY);
		for (const auto& cobj : m_FileReader->GetFileObjects()) {
			if (cobj.ObjectCid == LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE && cobj.Data != nullptr) {
				LibCmo::CK2::CKFileObject& obj = const_cast<LibCmo::CK2::CKFileObject&>(cobj);
				obj.Data->StartRead();
				auto chunkinfo = obj.Data->GetIdentifiersProfile();
				obj.Data->StopRead();

				// find CK_STATESAVE_TEXONLY and change it to CK_STATESAVE_OLDTEXONLY
				for (const auto& entry : chunkinfo) {
					if (entry.m_Identifier == expcode) {
						LibCmo::CKDWORD* p = static_cast<LibCmo::CKDWORD*>(entry.m_DataPtr);
						p -= 2;

						if (*p == expcode) {
							*p = modcode;
						}
					}
				}
			}
		}

#else
		PrintCommonError(u8"Test command only available in Debug mode.");
#endif
	}

	void Unvirt::Context::UnvirtContext::ProcHelp(const CmdHelper::ArgumentsMap&) {
		m_Help.Print();
	}

	void UnvirtContext::ProcExit(const CmdHelper::ArgumentsMap&) {
		m_OrderExit = true;
	}

#pragma endregion

}
