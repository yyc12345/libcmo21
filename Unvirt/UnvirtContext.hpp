#pragma once
#include <VTAll.hpp>

#include "AccessibleValue.hpp"
#include "TerminalHelper.hpp"
#include "StructFormatter.hpp"
#include "CmdHelper.hpp"

#include <cstdio>
#include <iostream>

namespace Unvirt::Context {

	class UnvirtContext {
	public:
		UnvirtContext();
		~UnvirtContext();
		YYCC_DEL_CLS_COPY_MOVE(UnvirtContext);

		void Run();

	protected:
		enum class SearchPart {
			None, Object, Manager
		};
		void PrintCommonInfo(const char* u8_fmt, ...);
		void PrintCommonError(const char* u8_fmt, ...);

		void ProcLoad(const CmdHelper::ArgumentsMap* amap);
		void ProcUnLoad(const CmdHelper::ArgumentsMap* amap);
		void ProcSave(const CmdHelper::ArgumentsMap* amap);
		void ProcInfo(const CmdHelper::ArgumentsMap* amap);
		void ProcLs(const CmdHelper::ArgumentsMap* amap);
		void ProcData(const CmdHelper::ArgumentsMap* amap);
		void ProcChunk(const CmdHelper::ArgumentsMap* amap);
		void ProcSearch(const CmdHelper::ArgumentsMap* amap);
		void ProcItems(const CmdHelper::ArgumentsMap* amap);
		void ProcStyle(const CmdHelper::ArgumentsMap* amap);
		void ProcEncoding(const CmdHelper::ArgumentsMap* amap);
		void ProcTemp(const CmdHelper::ArgumentsMap* amap);
		void ProcRsc(const CmdHelper::ArgumentsMap* amap, bool isClear);
		void ProcTest(const CmdHelper::ArgumentsMap* amap);
		void ProcHelp(const CmdHelper::ArgumentsMap* amap);
		void ProcExit(const CmdHelper::ArgumentsMap* amap);

	protected:
		bool HasOpenedFile();
		void ClearDocument();
		void PrintContextMsg(LibCmo::CKSTRING msg);

		CmdHelper::CommandRoot m_Root;
		CmdHelper::HelpDocument* m_Help;
		CmdHelper::CmdSplitter m_Splitter;

		size_t m_PageLen;
		bool m_ListStyleIsFull;
		SearchPart m_SearchPart;
		LibCmo::XContainer::XArray<size_t> m_SearchIdxResult;

		bool m_OrderExit;
		LibCmo::CK2::CKContext* m_Ctx;
		LibCmo::CK2::CKFileReader* m_FileReader;
		bool m_IsShallowRead;
	};


}
