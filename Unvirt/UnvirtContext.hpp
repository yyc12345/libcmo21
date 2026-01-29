#pragma once
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include "CmdHelper.hpp"

namespace Unvirt::Context {

	class UnvirtContext {
	public:
		UnvirtContext();
		~UnvirtContext();
		YYCC_DELETE_COPY_MOVE(UnvirtContext)

	public:
		void Run();

	private:
		void PrintInfo(const char8_t* u8_fmt, ...);
		void PrintError(const char8_t* u8_fmt, ...);

	private:
		void HandleLoad(const CmdHelper::LoadParam& param);
		void HandleUnLoad(const CmdHelper::UnloadParam& param);
		void HandleSave(const CmdHelper::SaveParam& param);
		void HandleInfo(const CmdHelper::InfoParam& param);
		void HandleLs(const CmdHelper::LsParam& param);
		void HandleData(const CmdHelper::DataParam& param);
		void HandleChunk(const CmdHelper::ChunkParam& param);
		void HandleSearch(const CmdHelper::SearchParam& param);
		void HandleItems(const CmdHelper::ItemsParam& param);
		void HandleStyle(const CmdHelper::StyleParam& param);
		void HandleEncoding(const CmdHelper::EncodingParam& param);
		void HandleTemp(const CmdHelper::TempParam& param);
		void HandleRscClear(const CmdHelper::RscClearParam& param);
		void HandleRscAdd(const CmdHelper::RscAddParam& param);
		void HandleTest(const CmdHelper::TestParam& param);
		void HandleVersion(const CmdHelper::VersionParam& param);
		void HandleHelp(const CmdHelper::HelpParam& param);
		void HandleExit(const CmdHelper::ExitParam& param);

	private:
		CmdHelper::Commander m_Commander;
		bool m_OrderExit;
		size_t m_ItemPerPage;
		CmdHelper::StyleLevel m_ListStyle;
		bool m_IsSearching;
		CmdHelper::SearchPart m_SearchPart;
		std::vector<size_t> m_SearchIdxResult;

	private:
		bool HasOpenedFile();
		void ClearDocument();
		void PrintContextMsg(LibCmo::CKSTRING msg);

	private:
		LibCmo::CK2::CKContext* m_Ctx;
		LibCmo::CK2::CKFileReader* m_FileReader;
		bool m_IsShallowRead;

	};


}
