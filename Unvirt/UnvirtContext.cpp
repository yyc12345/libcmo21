#include "UnvirtContext.hpp"
#include "Docstring.hpp"
#include "StructFmt.hpp"
#include <LibCmo/CK2/MgrImpls/CKPathManager.hpp>
#include <yycc/string/op.hpp>
#include <yycc/string/reinterpret.hpp>
#include <yycc/carton/termcolor.hpp>
#include <yycc/patch/stream.hpp>
#include <yycc/windows/console.hpp>
#include <iostream>
#include <cstdarg>
#include <regex>

using namespace yycc::patch::stream;
namespace strop = yycc::string::op;
namespace termcolor = yycc::carton::termcolor;
namespace ph = std::placeholders;

namespace Unvirt::Context {

#pragma region UnvirtContext Misc

	UnvirtContext::UnvirtContext() :
	    // Initialize command processor
	    m_Commander(), m_OrderExit(false), m_ItemPerPage(10u), m_ListStyle(CmdHelper::StyleLevel::Full), m_IsSearching(false),
	    m_SearchPart(CmdHelper::SearchPart::Object), m_SearchIdxResult(),
	    // Initialize Virtools
	    m_Ctx(nullptr), m_FileReader(nullptr), m_IsShallowRead(true) {

		// Set up commander with our callback.
		m_Commander.SetLoadDelegate(std::bind(UnvirtContext::HandleLoad, this, ph::_1));
		m_Commander.SetUnloadDelegate(std::bind(UnvirtContext::HandleUnLoad, this, ph::_1));
		m_Commander.SetSaveDelegate(std::bind(UnvirtContext::HandleSave, this, ph::_1));
		m_Commander.SetInfoDelegate(std::bind(UnvirtContext::HandleInfo, this, ph::_1));
		m_Commander.SetLsDelegate(std::bind(UnvirtContext::HandleLs, this, ph::_1));
		m_Commander.SetDataDelegate(std::bind(UnvirtContext::HandleData, this, ph::_1));
		m_Commander.SetChunkDelegate(std::bind(UnvirtContext::HandleChunk, this, ph::_1));
		m_Commander.SetSearchDelegate(std::bind(UnvirtContext::HandleSearch, this, ph::_1));
		m_Commander.SetItemsDelegate(std::bind(UnvirtContext::HandleItems, this, ph::_1));
		m_Commander.SetStyleDelegate(std::bind(UnvirtContext::HandleStyle, this, ph::_1));
		m_Commander.SetEncodingDelegate(std::bind(UnvirtContext::HandleEncoding, this, ph::_1));
		m_Commander.SetTempDelegate(std::bind(UnvirtContext::HandleTemp, this, ph::_1));
		m_Commander.SetRscClearDelegate(std::bind(UnvirtContext::HandleRscClear, this, ph::_1));
		m_Commander.SetRscAddDelegate(std::bind(UnvirtContext::HandleRscAdd, this, ph::_1));
		m_Commander.SetTestDelegate(std::bind(UnvirtContext::HandleTest, this, ph::_1));
		m_Commander.SetVersionDelegate(std::bind(UnvirtContext::HandleVersion, this, ph::_1));
		m_Commander.SetHelpDelegate(std::bind(UnvirtContext::HandleHelp, this, ph::_1));
		m_Commander.SetExitDelegate(std::bind(UnvirtContext::HandleExit, this, ph::_1));

		// initialize CK engine and create context
		LibCmo::CK2::CKERROR err = LibCmo::CK2::CKStartUp();
		if (err != LibCmo::CK2::CKERROR::CKERR_OK)
			throw std::runtime_error("fail to initialize CK2 engine.");
		m_Ctx = new LibCmo::CK2::CKContext();
		m_Ctx->SetOutputCallback(std::bind(&UnvirtContext::PrintContextMsg, this, ph::_1));
		m_Ctx->SetGlobalImagesSaveOptions(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_EXTERNAL);
	}

	UnvirtContext::~UnvirtContext() {
		// free context and shutdown CK engine.
		ClearDocument();
		delete m_Ctx;
		LibCmo::CK2::CKShutdown();
	}

#pragma endregion

#pragma region Virtools Controller

	bool UnvirtContext::HasOpenedFile() {
		return m_FileReader != nullptr;
	}

	void UnvirtContext::ClearDocument() {
		if (m_FileReader == nullptr) return;
		// clear search result
		m_IsSearching = false;
		m_SearchPart = CmdHelper::SearchPart::Object;
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
			termcolor::cprintln(msg, termcolor::Color::LightYellow);
		}
	}

#pragma endregion

#pragma region Message Printer

	void UnvirtContext::PrintInfo(const char8_t* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		std::cout << strop::vprintf(u8_fmt, argptr) << std::endl;
		va_end(argptr);
	}

	void UnvirtContext::PrintError(const char8_t* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		termcolor::cprintln(strop::vprintf(u8_fmt, argptr), termcolor::Color::LightRed);
		va_end(argptr);
	}

#pragma endregion

#pragma region Main Runner

	void UnvirtContext::Run() {
		// Enable terminal color feature
#if defined(YYCC_OS_WINDOWS)
		yycc::windows::console::colorful_console();
#endif

		// Show banner
		std::cout << termcolor::colored(u8"Unvirt", termcolor::Color::LightYellow)
		          << " (based on LibCmo " LIBCMO_VER_STR ") built at " __DATE__ " " __TIME__ << std::endl
		          << u8"Type 'help' for more infomation. Type 'exit' to quit." << std::endl;

		// start process loop
		while (true) {
			// get command
			termcolor::cprint(u8"> ", termcolor::Color::Green);
			std::string cmd;
			if (std::getline(std::cin, cmd).fail()) continue;
			auto u8cmd = yycc::string::reinterpret::as_utf8_view(cmd);

			// pass to commander
			auto rv = this->m_Commander.Dispatch(u8cmd);
			if (!rv.has_value()) {
				switch (rv.error()) { 
					case CmdHelper::Error::Lexer:
						this->PrintError(u8"Lexer error.");
						break;
					case CmdHelper::Error::BadVerb:
						this->PrintError(u8"Unknown verb.");
						break;
					case CmdHelper::Error::BadArg:
						this->PrintError(u8"Bad argument.");
						break;
					case CmdHelper::Error::TooLessParam:
						this->PrintError(u8"Given arguments are too less.");
						break;
					case CmdHelper::Error::TooMuchParam:
						this->PrintError(u8"Given arguments are too much.");
						break;
				}
				this->PrintError(u8"Please type 'help' for usage.");
			}

			// check whether sub processor need exit.
			if (m_OrderExit) {
				break;
			}
		}
	}

#pragma endregion

#pragma region Handler

	void UnvirtContext::HandleLoad(const CmdHelper::LoadParam& param) {
		// check pre-requirement
		if (HasOpenedFile()) {
			PrintError(u8"Already have a opened file. Close it before calling 'load'.");
			return;
		}
		if (!m_Ctx->IsValidEncoding()) {
			PrintError(u8"You have not set encoding properly. Set it before loading by calling 'encoding'.");
			return;
		}

		// load file
		m_FileReader = new LibCmo::CK2::CKFileReader(m_Ctx);
		LibCmo::CK2::CKERROR err;
		switch (param.stage) {
			case CmdHelper::LoadStage::Shallow:
				err = m_FileReader->ShallowLoad(param.filepath.c_str());
				m_IsShallowRead = true;
				break;
			case CmdHelper::LoadStage::Deep:
				err = m_FileReader->DeepLoad(param.filepath.c_str());
				m_IsShallowRead = false;
				break;
			default:
				throw std::runtime_error("unreachable code");
		}
		if (err != LibCmo::CK2::CKERROR::CKERR_OK) {
			// fail to load. release all.
			PrintError(u8"Fail to load file. Function return: %s (%s)",
			           Docstring::GetCkErrorName(err).c_str(),
			           Docstring::GetCkErrorDescription(err).c_str());

			ClearDocument();
		}
	}

	void UnvirtContext::HandleUnLoad(const CmdHelper::UnloadParam& param) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			this->PrintError(u8"No loaded file.");
			return;
		}

		// free all
		ClearDocument();
	}

	void UnvirtContext::HandleSave(const CmdHelper::SaveParam& param) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintError(u8"No loaded file.");
			return;
		}
		if (!m_Ctx->IsValidEncoding()) {
			PrintError(u8"You have not set encoding properly. Set it before saving by calling 'encoding'.");
			return;
		}

		// construct writer from reader
		LibCmo::CK2::CKFileWriter writer(m_Ctx, m_FileReader, m_IsShallowRead);

		// backup current file write mode
		// and replace it with reader
		LibCmo::CK2::CK_FILE_WRITEMODE backup = m_Ctx->GetFileWriteMode();
		m_Ctx->SetFileWriteMode(m_FileReader->GetFileInfo().FileWriteMode);

		// run writer
		LibCmo::CK2::CKERROR err = writer.Save(param.filepath.c_str());
		if (err != LibCmo::CK2::CKERROR::CKERR_OK) {
			// fail to load. release all.
			PrintError(u8"Fail to save file. Function return: %s\n\t%s",
				Docstring::GetCkErrorName(err).c_str(),
				Docstring::GetCkErrorDescription(err).c_str()
			);
		}

		// restore file write mode
		m_Ctx->SetFileWriteMode(backup);

	}

	void UnvirtContext::HandleInfo(const CmdHelper::InfoParam& param) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintError(u8"No loaded file.");
			return;
		}

		// print
		StructFmt::PrintCKFileInfo(m_FileReader->GetFileInfo());
	}

	void UnvirtContext::HandleLs(const CmdHelper::LsParam& param) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintError(u8"No loaded file.");
			return;
		}

		// get 0 based page (-1)
		size_t page = param.page - 1;
		bool full_detail = m_ListStyle == CmdHelper::StyleLevel::Full;

		// show list
		switch (param.part) {
			case CmdHelper::LsPart::Object: {
				// obj list
				StructFmt::PrintObjectList(m_FileReader->GetFileObjects(),
				                           m_FileReader->GetFileInfo(),
				                           page,
				                           this->m_ItemPerPage,
				                           full_detail);
				break;
			}
			case CmdHelper::LsPart::Manager: {
				// mgr list
				StructFmt::PrintManagerList(m_FileReader->GetManagersData(), page, this->m_ItemPerPage, full_detail);
				break;
			}
			case CmdHelper::LsPart::Search: {
				// search list
				if (m_IsSearching) {
					switch (m_SearchPart) {
						case CmdHelper::SearchPart::Object: {
							StructFmt::PrintSearchedObjectList(m_SearchIdxResult,
							                                   m_FileReader->GetFileObjects(),
							                                   m_FileReader->GetFileInfo(),
							                                   page,
							                                   this->m_ItemPerPage,
							                                   full_detail);
							break;
						}
						case CmdHelper::SearchPart::Manager: {
							StructFmt::PrintSearchedManagerList(m_SearchIdxResult,
							                                    m_FileReader->GetManagersData(),
							                                    page,
							                                    this->m_ItemPerPage,
							                                    full_detail);
							break;
						}
						default:
							throw std::runtime_error("unreachable code");
					}
				} else {
					PrintError(u8"No search result to list.");
				}
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}
	}

	void UnvirtContext::HandleData(const CmdHelper::DataParam& param) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintError(u8"No loaded file.");
			return;
		}

		// get index
		size_t index = param.index;

		// show data
		switch (param.part) {
			case CmdHelper::DataPart::Object:
			{
				if (index >= m_FileReader->GetFileObjects().size()) {
					PrintError(u8"Index out of range.");
					return;
				}
				StructFmt::PrintCKObject(m_FileReader->GetFileObjects()[index].ObjPtr);
				break;
			}
			case CmdHelper::DataPart::Manager:
			{
				if (index >= m_FileReader->GetManagersData().size()) {
					PrintError(u8"Index out of range.");
					return;
				}
				// todo: finish manager display
				PrintError(u8"Not supported now.");
				//StructFmt::PrintCKBaseManager(m_FileReader->GetManagersData()[index].Data);
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}
	}

	void UnvirtContext::HandleChunk(const CmdHelper::ChunkParam& param) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintError(u8"No loaded file.");
			return;
		}

		// get index
		size_t index = param.index;

		// show data
		switch (param.part) {
			case CmdHelper::ChunkPart::Object:
			{
				if (index >= m_FileReader->GetFileObjects().size()) {
					PrintError(u8"Index out of range.");
					return;
				}
				StructFmt::PrintCKStateChunk(m_FileReader->GetFileObjects()[index].Data);
				break;
			}
			case CmdHelper::ChunkPart::Manager:
			{
				if (index >= m_FileReader->GetManagersData().size()) {
					PrintError(u8"Index out of range.");
					return;
				}
				StructFmt::PrintCKStateChunk(m_FileReader->GetManagersData()[index].Data);
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}
	}

	void UnvirtContext::HandleSearch(const CmdHelper::SearchParam& param) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintError(u8"No loaded file.");
			return;
		}

		// get search text
		std::u8string search_text = param.text;

		// analyse search mode
		std::function<bool(const LibCmo::XContainer::XString&)> search_fct = nullptr;
		switch (param.mode) {
			case CmdHelper::SearchMode::PlainText:
			{
				// plain mode
				search_fct = [&search_text](const LibCmo::XContainer::XString& cmp) -> bool {
					return cmp.find(search_text) != std::string::npos;
				};
				break;
			}
			case CmdHelper::SearchMode::Regex:
			{
				// regex mode

				// get ordinary search text
				std::string ordinary_search_text = yycc::string::reinterpret::as_ordinary(search_text);
				// try construct regex
				std::regex re;
				try {
					re = std::regex(ordinary_search_text, std::regex_constants::ECMAScript);
				} catch (const std::regex_error& e) {
					PrintError(u8"Invalid regular expressions: %s", e.what());
					return;
				}

				// use copy ctor capture to input regex
				// because re will be freed when exiting this switch.
				search_fct = [re](const LibCmo::XContainer::XString& cmp) -> bool {
					// get ordinary name and comapre
					std::string ordinary_cmp = yycc::string::reinterpret::as_ordinary(cmp);
					return std::regex_search(ordinary_cmp, re);
				};
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}

		// start search
		m_IsSearching = true;
		m_SearchPart = param.part;
		switch (param.part) {
			case CmdHelper::SearchPart::Object:
			{
				// object
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
			case CmdHelper::SearchPart::Manager:
			{
				// manager
				m_SearchIdxResult.clear();
				PrintError(u8"Not supported now.");
				// todo: remove this return when fixing manager searching.
				return;
				break;
			}
			default:
				throw std::runtime_error("unreachable code");
		}

		// report search result
		if (m_SearchIdxResult.empty()) {
			PrintInfo(u8"Search done, but no result.");
		} else {
			PrintInfo(u8"Search done with %" PRIuSIZET " results. Use `ls search` to check them.", m_SearchIdxResult.size());
		}
	}

	void UnvirtContext::HandleItems(const CmdHelper::ItemsParam& param) {
		// assign
		m_ItemPerPage = param.count;
	}

	void UnvirtContext::HandleStyle(const CmdHelper::StyleParam& param) {
		// assign
		m_ListStyle = param.level;
	}

	void UnvirtContext::HandleEncoding(const CmdHelper::EncodingParam& param) {
		m_Ctx->SetEncoding(param.enc);
	}

	void UnvirtContext::HandleTemp(const CmdHelper::TempParam& param) {
		// assign
		if (!m_Ctx->GetPathManager()->SetTempFolder(param.dirpath.c_str())) {
			PrintError(u8"Set temp folder failed. Check your path first.");
		}
	}

	void UnvirtContext::HandleRscClear(const CmdHelper::RscClearParam& param) {
		m_Ctx->GetPathManager()->ClearPath();
	}

	void UnvirtContext::HandleRscAdd(const CmdHelper::RscAddParam& param) {
		if (!m_Ctx->GetPathManager()->AddPath(param.dirpath.c_str())) {
			PrintError(u8"Set data resource folder failed. Check your path first.");
		}
	}

	void UnvirtContext::HandleTest(const CmdHelper::TestParam& param) {
#if defined(LIBCMO_BUILD_DEBUG)
		// MARK: Add the debug code here.

		// todo: temporaryly write Transparent Column Fixer code.
		// move to independent app in future.

		// check pre-requirement
		if (!HasOpenedFile()) {
			PrintError(u8"No loaded file.");
			return;
		}
		if (!m_IsShallowRead) {
			PrintError(u8"Transparent Column Fixer only accept shallow loaded file.");
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
		PrintError(u8"Test command only available in Debug mode.");
#endif
	}

	void UnvirtContext::HandleVersion(const CmdHelper::VersionParam& param) {
		// MARK: Copied from splash at Main Runner.
		std::cout << termcolor::colored(u8"Unvirt", termcolor::Color::LightYellow)
		          << " (based on LibCmo " LIBCMO_VER_STR ") built at " __DATE__ " " __TIME__ << std::endl;
	}

	void UnvirtContext::HandleHelp(const CmdHelper::HelpParam& param) {
		std::cout << u8"version: Show version info about this program." << std::endl
		          << u8"help: Print this page." << std::endl
		          << u8"exit: Quit this program." << std::endl
		          << std::endl
		          << u8"See manual provided with this program for more commands because it is so long." << std::endl;
	}

	void UnvirtContext::HandleExit(const CmdHelper::ExitParam& param) {
		m_OrderExit = true;
	}

#pragma endregion

}
