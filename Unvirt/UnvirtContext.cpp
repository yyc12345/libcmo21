#include "UnvirtContext.hpp"

namespace Unvirt::Context {

#pragma region UnvirtContext Misc

	UnvirtContext::UnvirtContext() :
		m_Root(), m_Splitter(), m_Help(nullptr),
		m_PageLen(10u), m_OrderExit(false),
		m_Ctx(nullptr), m_FileReader(nullptr) {

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
						"Load a Virtools composition deeply."
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
		->Then((new CmdHelper::Literal("info"))
			->Executes(
				std::bind(&UnvirtContext::ProcInfo, this, std::placeholders::_1),
				"Show the header info of loaded Virtools composition."
			)
		)
		->Then((new CmdHelper::Literal("ls"))
			->Then((new CmdHelper::Choice("part", { "obj", "mgr"}))
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
		->Then((new CmdHelper::Literal("items"))
			->Then((new CmdHelper::IntArgument("count", [](int32_t v) -> bool { return v > 0; }))
				->Comment("The count of items you want to show in one page.")
				->Executes(
					std::bind(&UnvirtContext::ProcItems, this, std::placeholders::_1),
					"Set up how many items should be listed in one page when using 'ls' command."
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
		m_Ctx = new LibCmo::CK2::CKContext();
		m_Ctx->SetOutputCallback(std::bind(&UnvirtContext::PrintContextMsg, this, std::placeholders::_1));
	}

	UnvirtContext::~UnvirtContext() {
		if (m_Help != nullptr)
			delete m_Help;
	}

	bool UnvirtContext::HasOpenedFile() {
		return m_FileReader != nullptr;
	}

	void UnvirtContext::ClearDocument() {
		if (m_FileReader == nullptr) return;
		// delete reader
		delete m_FileReader;
		m_FileReader = nullptr;
		// clear context
		m_Ctx->DestroyAllCKObjects();
	}

	void UnvirtContext::PrintContextMsg(LibCmo::CK2::CKSTRING msg) {
		if (msg != nullptr) {
			fprintf(stdout, UNVIRT_TERMCOL_LIGHT_YELLOW(("[CKContext] ")) "%s\n", msg);
		}
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
		} else {
			err = m_FileReader->ShallowLoad(filepath.c_str());
		}
		if (err != LibCmo::CK2::CKERROR::CKERR_OK) {
			// fail to load. release all.
			PrintCommonError("Fail to open file. Function return: %s\n\t%s",
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

		// get page
		size_t page = *amap->Get<CmdHelper::IntArgument::vType>("page") - 1;

		// show list
		switch (*amap->Get<CmdHelper::Choice::vType>("part")) {
			case 0:
			{
				// obj list
				if (page * m_PageLen >= m_FileReader->GetFileObjects().size()) {
					PrintCommonError("Page out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintObjectList(m_FileReader->GetFileObjects(), page, this->m_PageLen);
				break;
			}
			case 1:
			{
				// mgr list
				if (page * m_PageLen >= m_FileReader->GetManagersData().size()) {
					PrintCommonError("Page out of range.");
					return;
				}
				Unvirt::StructFormatter::PrintManagerList(m_FileReader->GetManagersData(), page, this->m_PageLen);
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
				PrintCommonError("WIP function.");
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

	void UnvirtContext::ProcItems(const CmdHelper::ArgumentsMap* amap) {
		// assign
		m_PageLen = *amap->Get<CmdHelper::IntArgument::vType>("count");
	}

	void UnvirtContext::ProcEncoding(const CmdHelper::ArgumentsMap* amap) {
		const auto& encodings = *amap->Get<CmdHelper::EncodingArgument::vType>("enc");
		m_Ctx->SetEncoding(encodings);
	}

	void UnvirtContext::ProcTemp(const CmdHelper::ArgumentsMap* amap) {
		// assign
		m_Ctx->SetTempPath(amap->Get<CmdHelper::StringArgument::vType>("temppath")->c_str());
	}

	void Unvirt::Context::UnvirtContext::ProcHelp(const CmdHelper::ArgumentsMap* amap) {
		m_Help->Print();
	}

	void UnvirtContext::ProcExit(const CmdHelper::ArgumentsMap*) {
		m_OrderExit = true;
	}

#pragma endregion

}
