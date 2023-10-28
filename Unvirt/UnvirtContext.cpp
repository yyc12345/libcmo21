#include "UnvirtContext.hpp"
#include <CK2/MgrImpls/CKPathManager.hpp>
#include <cstdarg>
#include <regex>

namespace Unvirt::Context {

#pragma region UnvirtContext Misc

	UnvirtContext::UnvirtContext() :
		m_Root(), m_Splitter(), m_Help(nullptr),
		m_PageLen(10u), m_ListStyleIsFull(true), m_OrderExit(false),
		m_SearchPart(SearchPart::None), m_SearchIdxResult(),
		m_Ctx(nullptr), m_FileReader(nullptr), m_IsShallowRead(true) {

		// create command root
		CmdHelper::CommandRoot* root = &m_Root;

		root
		->Then((new CmdHelper::Literal("load"))
			->Then((new CmdHelper::Choice("stage", { "deep", "shallow"}))
				->Comment("The stage of loading. 'deep' will load to CKObject stage. 'shallow' will load to CKStateChunk stage.")
				->Then((new CmdHelper::StringArgument("filepath"))
					->Comment("The path to loading file.")
					->Executes(
						std::bind(&UnvirtContext::ProcLoad, this, std::placeholders::_1),
						"Load a Virtools composition."
					)
				)
			)
		)
		->Then((new CmdHelper::Literal("unload"))
			->Executes(
				std::bind(&UnvirtContext::ProcUnLoad, this, std::placeholders::_1),
				"Release loaded Virtools composition."
			)
		)
		->Then((new CmdHelper::Literal("save"))
			->Then((new CmdHelper::StringArgument("filepath"))
				->Comment("The path to save file.")
				->Executes(
					std::bind(&UnvirtContext::ProcSave, this, std::placeholders::_1),
					"Save the loaded file into a new file."
				)
			)
		)
		->Then((new CmdHelper::Literal("info"))
			->Executes(
				std::bind(&UnvirtContext::ProcInfo, this, std::placeholders::_1),
				"Show the header info of loaded Virtools composition."
			)
		)
		->Then((new CmdHelper::Literal("ls"))
			->Then((new CmdHelper::Choice("part", { "obj", "mgr", "search"}))
				->Comment("Which list you want to list.")
				->Then((new CmdHelper::IntArgument("page", [](int32_t v) -> bool { return v > 0; }))
					->Comment("The page index. Start with 1.")
					->Executes(
						std::bind(&UnvirtContext::ProcLs, this, std::placeholders::_1),
						"List something about loaded Virtools composition."
					)
				)
			)
		)
		->Then((new CmdHelper::Literal("data"))
			->Then((new CmdHelper::Choice("part", { "obj", "mgr"}))
				->Comment("Which list you want to show data.")
				->Then((new CmdHelper::IntArgument("index", [](int32_t v) -> bool { return v >= 0; }))
					->Comment("The index. Start with 0.")
					->Executes(
						std::bind(&UnvirtContext::ProcData, this, std::placeholders::_1),
						"Show the specific CKObject / CKBaseManager data."
					)
				)
			)
		)
		->Then((new CmdHelper::Literal("chunk"))
			->Then((new CmdHelper::Choice("part", { "obj", "mgr"}))
				->Comment("Which list you want to show chunk.")
				->Then((new CmdHelper::IntArgument("index", [](int32_t v) -> bool { return v >= 0; }))
					->Comment("The index. Start with 0.")
					->Executes(
						std::bind(&UnvirtContext::ProcChunk, this, std::placeholders::_1),
						"Show the specific CKStateChunk data."
					)
				)
			)
		)
		->Then((new CmdHelper::Literal("search"))
			->Then((new CmdHelper::Choice("part", { "obj", "mgr"}))
				->Comment("Which list you want to search.")
				->Then((new CmdHelper::Choice("mode", { "plain", "re"}))
					->Comment("The search mode. `plain` will search by substring and `re` will do regex search.")
					->Then((new CmdHelper::StringArgument("text"))
						->Comment("The text or regex to search.")
						->Executes(
							std::bind(&UnvirtContext::ProcSearch, this, std::placeholders::_1),
							"Search object or manager by text or regex. Please note that the regex have limited UTF8 support and may cause undefined behavior."
						)
					)
				)
			)
		)
		->Then((new CmdHelper::Literal("items"))
			->Then((new CmdHelper::IntArgument("count", [](int32_t v) -> bool { return v > 0; }))
				->Comment("The count of items you want to show in one page.")
				->Executes(
					std::bind(&UnvirtContext::ProcItems, this, std::placeholders::_1),
					"Set up how many items should be listed in one page when using 'ls' command."
				)
			)
		)
		->Then((new CmdHelper::Literal("style"))
			->Then((new CmdHelper::Choice("level", { "full", "simple"}))
				->Comment("The amount of showen content.")
				->Executes(
					std::bind(&UnvirtContext::ProcStyle, this, std::placeholders::_1),
					"Change the detail level of showen data in `ls` command."
				)
			)
		)
		->Then((new CmdHelper::Literal("encoding"))
			->Then((new CmdHelper::EncodingArgument("enc"))
				->Comment("CKContext used encoding splitted by ','. Support mutiple encoding.")
				->Executes(
					std::bind(&UnvirtContext::ProcEncoding, this, std::placeholders::_1),
					"Set the encoding series for CKContext."
				)
			)
		)
		->Then((new CmdHelper::Literal("temp"))
			->Then((new CmdHelper::StringArgument("temppath"))
				->Comment("The path to Temp folder.")
				->Executes(
					std::bind(&UnvirtContext::ProcTemp, this, std::placeholders::_1),
					"Set the Temp path for CKContext."
				)
			)
		)
		->Then((new CmdHelper::Literal("rsc"))
			->Then((new CmdHelper::Literal("clear"))
				->Executes(
					std::bind(&UnvirtContext::ProcRsc, this, std::placeholders::_1, true),
					"Clear all data resources paths."
				)
			)
			->Then((new CmdHelper::Literal("add"))
				->Then((new CmdHelper::StringArgument("datares"))
					->Comment("The data resources path .")
					->Executes(
						std::bind(&UnvirtContext::ProcRsc, this, std::placeholders::_1, false),
						"Add a path to let Virtools find resources."
					)
				)
			)
		)
		->Then((new CmdHelper::Literal("test"))
			->Executes(
				std::bind(&UnvirtContext::ProcTest, this, std::placeholders::_1),
				"Call custom debugging function (only available in Debug mode)."
			)
		)
		->Then((new CmdHelper::Literal("help"))
			->Executes(
				std::bind(&UnvirtContext::ProcHelp, this, std::placeholders::_1),
				"Output this help page."
			)
		)
		->Then((new CmdHelper::Literal("exit"))
			->Executes(
				std::bind(&UnvirtContext::ProcExit, this, std::placeholders::_1),
				"Exit program."
			)
		);

		// create help
		m_Help = root->RootHelp();

		// create context
		LibCmo::CK2::CKStartUp();
		m_Ctx = new LibCmo::CK2::CKContext();
		m_Ctx->SetOutputCallback(std::bind(&UnvirtContext::PrintContextMsg, this, std::placeholders::_1));
		m_Ctx->SetGlobalImagesSaveOptions(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_EXTERNAL);
	}

	UnvirtContext::~UnvirtContext() {
		if (m_Help != nullptr)
			delete m_Help;

		// free context
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
			fprintf(stdout, UNVIRT_TERMCOL_LIGHT_YELLOW(("[CKContext] ")) "%s\n", msg);
		}
	}
	
	void UnvirtContext::PrintCommonInfo(const char* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		std::vfprintf(stdout, u8_fmt, argptr);
		va_end(argptr);
		std::fputc('\n', stdout);
	}

	void UnvirtContext::PrintCommonError(const char* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		std::fputs(UNVIRT_TERMCOLHDR_LIGHT_RED, stdout);
		std::vfprintf(stdout, u8_fmt, argptr);
		std::fputs(UNVIRT_TERMCOLTAIL, stdout);
		va_end(argptr);
		std::fputc('\n', stdout);
	}

#pragma endregion

#pragma region Main Run

	void UnvirtContext::Run() {
		Unvirt::TerminalHelper::EnsureTerminalColor();
		Unvirt::TerminalHelper::EnsureTerminalEncoding();
		std::string u8cmd;

		while (true) {
			// get command
			TerminalHelper::GetCmdLine(u8cmd);

			// split cmd and parse it
			auto cmds = m_Splitter.Convert(u8cmd);
			if (cmds.empty()) continue;

			// get sub command
			if (!m_Root.RootConsume(cmds)) {
				this->PrintCommonError("Syntax error \"%s\".\nType 'help' for usage.", u8cmd.c_str());
			}

			if (m_OrderExit) {
				break;
			}
		}
	}

#pragma endregion

#pragma region Proc Detail

	void UnvirtContext::ProcLoad(const CmdHelper::ArgumentsMap* amap) {
		// check pre-requirement
		if (HasOpenedFile()) {
			PrintCommonError("Already have a opened file. Close it before calling 'load'.");
			return;
		}

		std::string filepath = *amap->Get<CmdHelper::StringArgument::vType>("filepath");
		bool is_deep = *amap->Get<CmdHelper::Choice::vType>("stage") == 0;

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
			PrintCommonError("Fail to load file. Function return: %s\n\t%s",
				Unvirt::AccessibleValue::GetCkErrorName(err).c_str(),
				Unvirt::AccessibleValue::GetCkErrorDescription(err).c_str()
			);

			ClearDocument();
		}
	}

	void UnvirtContext::ProcUnLoad(const CmdHelper::ArgumentsMap*) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			this->PrintCommonError("No loaded file.");
			return;
		}

		// free all
		ClearDocument();
	}

	void Unvirt::Context::UnvirtContext::ProcSave(const CmdHelper::ArgumentsMap* amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError("No loaded file.");
			return;
		}

		std::string filepath = *amap->Get<CmdHelper::StringArgument::vType>("filepath");

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
			PrintCommonError("Fail to save file. Function return: %s\n\t%s",
				Unvirt::AccessibleValue::GetCkErrorName(err).c_str(),
				Unvirt::AccessibleValue::GetCkErrorDescription(err).c_str()
			);
		}

		// restore file write mode
		m_Ctx->SetFileWriteMode(backup);

	}

	void UnvirtContext::ProcInfo(const CmdHelper::ArgumentsMap*) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError("No loaded file.");
			return;
		}

		// print
		Unvirt::StructFormatter::PrintCKFileInfo(m_FileReader->GetFileInfo());
	}

	void UnvirtContext::ProcLs(const CmdHelper::ArgumentsMap* amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError("No loaded file.");
			return;
		}

		// get 0 based page (-1)
		size_t page = *amap->Get<CmdHelper::IntArgument::vType>("page") - 1;

		// show list
		switch (*amap->Get<CmdHelper::Choice::vType>("part")) {
			case 0:
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
			case 1:
			{
				// mgr list
				Unvirt::StructFormatter::PrintManagerList(
					m_FileReader->GetManagersData(), 
					page, this->m_PageLen,
					m_ListStyleIsFull
				);
				break;
			}
			case 2:
			{
				// search list
				switch (m_SearchPart) {
					case SearchPart::None:
					{
						PrintCommonError("No search result to list.");
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
		}
	}

	void UnvirtContext::ProcData( const CmdHelper::ArgumentsMap* amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError("No loaded file.");
			return;
		}

		// get index
		size_t index = *amap->Get<CmdHelper::IntArgument::vType>("index");

		// show data
		switch (*amap->Get<CmdHelper::Choice::vType>("part")) {
			case 0:
			{
				if (index >= m_FileReader->GetFileObjects().size()) {
					PrintCommonError("Index out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintCKObject(m_FileReader->GetFileObjects()[index].ObjPtr);
				break;
			}
			case 1:
			{
				if (index >= m_FileReader->GetManagersData().size()) {
					PrintCommonError("Index out of range.");
					return;
				}
				// todo: finish manager display
				PrintCommonError("Not supported now.");
				//Unvirt::StructFormatter::PrintCKBaseManager(m_FileReader->GetManagersData()[index].Data);
				break;
			}
		}
	}

	void UnvirtContext::ProcChunk(const CmdHelper::ArgumentsMap* amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError("No loaded file.");
			return;
		}

		// get index
		size_t index = *amap->Get<CmdHelper::IntArgument::vType>("index");

		// show data
		switch (*amap->Get<CmdHelper::Choice::vType>("part")) {
			case 0:
			{
				if (index >= m_FileReader->GetFileObjects().size()) {
					PrintCommonError("Index out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintCKStateChunk(m_FileReader->GetFileObjects()[index].Data);
				break;
			}
			case 1:
			{
				if (index >= m_FileReader->GetManagersData().size()) {
					PrintCommonError("Index out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintCKStateChunk(m_FileReader->GetManagersData()[index].Data);
				break;
			}
		}
	}

	void UnvirtContext::ProcSearch(const CmdHelper::ArgumentsMap* amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintCommonError("No loaded file.");
			return;
		}

		// get search text
		std::string search_text(*amap->Get<CmdHelper::StringArgument::vType>("text"));
	
		// analyse search mode
		std::function<bool(const LibCmo::XContainer::XString&)> search_fct = [](const LibCmo::XContainer::XString&) -> bool { return false; };
		switch (*amap->Get<CmdHelper::Choice::vType>("mode")) {
			case 0:
			{
				// plain mode
				search_fct = [&search_text](const LibCmo::XContainer::XString& cmp) -> bool {
					return cmp.find(search_text) != std::string::npos;
				};
				break;
			}
			case 1:
			{
				// regex mode
				
				// try construct regex
				std::regex re;
				try {
					re = std::regex(search_text, std::regex_constants::ECMAScript);
				} catch (const std::regex_error& e) {
					PrintCommonError("Invalid regular expressions: %s", e.what());
					return;
				}

				// use copy ctor capture to input regex
				// because re will be freed when exiting this switch.
				search_fct = [re](const LibCmo::XContainer::XString& cmp) -> bool {
					return std::regex_search(cmp, re);
				};
				break;
			}
		}

		// start search
		switch (*amap->Get<CmdHelper::Choice::vType>("part")) {
			case 0:
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
			case 1:
			{
				// manager
				m_SearchPart = SearchPart::Manager;
				m_SearchIdxResult.clear();
				PrintCommonError("Not supported now.");
				// todo: remove this return when fixing manager searching.
				return;
				break;
			}
		}

		// report search result
		if (m_SearchIdxResult.empty()) {
			PrintCommonInfo("Search done, but no result.");
		} else {
			PrintCommonInfo("Search done with %" PRIuSIZET " results. Use `ls search` to check them.", m_SearchIdxResult.size());
		}
	}

	void UnvirtContext::ProcItems(const CmdHelper::ArgumentsMap* amap) {
		// assign
		m_PageLen = *amap->Get<CmdHelper::IntArgument::vType>("count");
	}

	void UnvirtContext::ProcStyle(const CmdHelper::ArgumentsMap* amap) {
		// set list style level
		switch (*amap->Get<CmdHelper::Choice::vType>("level")) {
			case 0:
			{
				// full level
				m_ListStyleIsFull = true;
				break;
			}
			case 1:
			{
				// simple level
				m_ListStyleIsFull = false;
				break;
			}
		}
	}

	void UnvirtContext::ProcEncoding(const CmdHelper::ArgumentsMap* amap) {
		const auto& encodings = *amap->Get<CmdHelper::EncodingArgument::vType>("enc");
		m_Ctx->SetEncoding(encodings);
	}

	void UnvirtContext::ProcTemp(const CmdHelper::ArgumentsMap* amap) {
		// assign
		if (!m_Ctx->GetPathManager()->SetTempFolder(amap->Get<CmdHelper::StringArgument::vType>("temppath")->c_str())) {
			PrintCommonError("Set temp folder failed. Check your path first.");
		}
	}

	void Unvirt::Context::UnvirtContext::ProcRsc(const CmdHelper::ArgumentsMap* amap, bool isClear) {
		if (isClear) {
			m_Ctx->GetPathManager()->ClearPath();
		} else {
			if (!m_Ctx->GetPathManager()->AddPath(amap->Get<CmdHelper::StringArgument::vType>("datares")->c_str())) {
				PrintCommonError("Set data resource folder failed. Check your path first.");
			}
		}
	}

	void Unvirt::Context::UnvirtContext::ProcTest(const CmdHelper::ArgumentsMap* amap) {
#if defined(LIBCMO_BUILD_DEBUG)
		// MARK: Add the debug code here.

#else
		PrintCommonError("Test command only available in Debug mode.");
#endif
	}

	void Unvirt::Context::UnvirtContext::ProcHelp(const CmdHelper::ArgumentsMap*) {
		m_Help->Print();
	}

	void UnvirtContext::ProcExit(const CmdHelper::ArgumentsMap*) {
		m_OrderExit = true;
	}

#pragma endregion

}
