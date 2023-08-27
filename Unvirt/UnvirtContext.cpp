#include "UnvirtContext.hpp"

namespace Unvirt::Context {

	static FILE* fout = stdout;

#pragma region Encoding Arg

#pragma endregion

#pragma region UnvirtContext Misc

	UnvirtContext::UnvirtContext() :
		m_Root(), m_Splitter(), m_Help(nullptr),
		m_PageLen(10u), m_OrderExit(false),
		m_Ctx(nullptr), m_FileReader(nullptr) {

		// create command root
		CmdHelper::CommandRoot* root = &m_Root;

		root->Then(
			(new CmdHelper::Literal("deepload"))
			->Then((new CmdHelper::StringArgument("filepath"))
			->Comment("The path to loading file.")
			->Executes(
			std::bind(&UnvirtContext::ProcLoad, this, true, std::placeholders::_1),
			"Load a Virtools composition deeply. Load file to CKObject stage."
		)
		)
		);
		root->Then(
			(new CmdHelper::Literal("shallowload"))->Then(
			(new CmdHelper::StringArgument("filepath", "The path to loading file."))->Executes(
			std::bind(&UnvirtContext::ProcLoad, this, false, std::placeholders::_1),
			"Load a Virtools composition shallowly. Load file to CKStateChunk stage."
		)
		)
		);
		root->Then(
			(new CmdHelper::Literal("unload"))->Executes(
			std::bind(&UnvirtContext::ProcUnLoad, this, std::placeholders::_1),
			"Release loaded Virtools composition."
		)
		);
		root->Then(
			(new CmdHelper::Literal("info"))->Executes(
			std::bind(&UnvirtContext::ProcInfo, this, std::placeholders::_1),
			"Show the header info of loaded Virtools composition."
		)
		);


		root->Then(
			(new CmdHelper::Literal("foo"))->Then(
			(new CmdHelper::Literal("bar"))->Executes([](const CmdHelper::ArgumentsMap& amap) -> void {
				fprintf(stdout, "foobar!\n");
			}, "simple foobar")
		)->Then(
			(new CmdHelper::IntArgument("bar", "the calling target 1"))->Executes([](const CmdHelper::ArgumentsMap& amap) -> void {
					fprintf(stdout, "foo%" PRIi32 "!\n", *(amap.Get("bar")->GetData<int32_t>()));
				}, "specialized foo bar")
		)
				)->Then(
			(new CmdHelper::Literal("call"))->Then(
					(new CmdHelper::IntArgument("bar", "the calling taget 2"))->Executes([](const CmdHelper::ArgumentsMap& amap) -> void {
						fprintf(stdout, "call%" PRIi32 "!\n", *(amap.Get("bar")->GetData<int32_t>()));
					}, "calling someone")
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
		delete m_FileReader;
		m_FileReader = nullptr;
	}

	void UnvirtContext::PrintContextMsg(LibCmo::CK2::CKSTRING msg) {
		if (msg != nullptr) {
			fprintf(fout, UNVIRT_TERMCOL_LIGHT_YELLOW(("[CKContext] ")) "%s\n", msg);
		}
	}

	void UnvirtContext::PrintCommonError(const char* u8_fmt, ...) {
		va_list argptr;
		va_start(argptr, u8_fmt);
		std::fputs(UNVIRT_TERMCOLHDR_LIGHT_RED, fout);
		std::vfprintf(fout, u8_fmt, argptr);
		std::fputs(UNVIRT_TERMCOLTAIL, fout);
		va_end(argptr);
		std::fputc('\n', fout);
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
				this->PrintCommonError("Command syntax error \"%s\".", u8cmd.c_str());
				m_Help->Print();
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
			PrintCommonError("Already have a opened file. Close it before calling \"load\".");
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

	void UnvirtContext::ProcUnLoad(const CmdHelper::ArgumentsMap* amap) {
		// check pre-requirement
		if (!HasOpenedFile()) {
			this->PrintCommonError("No loaded file.");
			return;
		}

		// free all
		ClearDocument();
	}

	void UnvirtContext::ProcInfo(const CmdHelper::ArgumentsMap* amap) {
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
			this->PrintCommonError("No loaded file.");
			return;
		}

		// check requirement
		int32_t gotten_page = *amap.Get("page")->GetData<int32_t>();
		if (gotten_page <= 0) {
			PrintCommonError("Page out of range.");
			return;
		}
		size_t page = static_cast<size_t>(gotten_page) - 1;

		// show list
		switch (parts) {
			case ViewPart::Objects:
			{
				// obj list
				if (page * m_PageLen >= m_FileReader->GetFileObjects().size()) {
					PrintCommonError("Page out of range.");
					return;
				}

				Unvirt::StructFormatter::PrintObjectList(m_FileReader->GetFileObjects(), page, this->m_PageLen);
				break;
			}
			case ViewPart::Managers:
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

	}

	void UnvirtContext::ProcChunk(const CmdHelper::ArgumentsMap* amap) {

	}

	void UnvirtContext::ProcItems(const CmdHelper::ArgumentsMap* amap) {
		// check requirement
		int32_t count = *amap.Get("count")->GetData<int32_t>();
		if (count <= 0) {
			PrintCommonError("Value out of range.");
			return;
		}

		// assign
		m_PageLen = static_cast<size_t>(count);
	}

	void UnvirtContext::ProcEncoding(const CmdHelper::ArgumentsMap* amap) {

	}

	void UnvirtContext::ProcTemp(const CmdHelper::ArgumentsMap* amap) {
		// check requirement
		std::string temppath = *amap.Get("temppath")->GetData<std::string>();

		// assign
		m_Ctx->SetTempPath(temppath.c_str());
	}

	void UnvirtContext::ProcExit(const CmdHelper::ArgumentsMap* amap) {
		m_OrderExit = true;
	}

#pragma endregion

}
