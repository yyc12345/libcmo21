#pragma once

#include "../../VTAll.hpp"
#include "CKBaseManager.hpp"
#include <filesystem>

namespace LibCmo::CK2::MgrImpls {

	class CKPathManager : public CKBaseManager {
	public:
		CKPathManager(CKContext* ctx);
		virtual ~CKPathManager();
		LIBCMO_DISABLE_COPY_MOVE(CKPathManager);
		
		/**
		 * @brief Set the temp folder of current context.
		 * @param u8_temp The temp folder you need to assign
		 * @return true if success.
		*/
		bool SetTempFolder(CKSTRING u8_temp);
		/**
		 * @brief Get current temp folder.
		 * @return 
		*/
		std::string GetTempFolder();
		/**
		 * @brief Get the path of temp file.
		 * @param u8_filename The relative path of file.
		 * @return The path of given path based on temp folder.
		*/
		std::string GetTempFilePath(CKSTRING u8_filename);
		
		/**
		 * @brief Add extra path for ResolveFileName
		 * @param u8path The added path.
		 * @return true if success.
		*/
		bool AddPath(CKSTRING u8path);
		/**
		 * @brief Clear all extra path.
		*/
		void ClearPath();

		/**
		 * @brief Finds a file in the paths
		 * @param u8_filename[inout] The given file path. overwritten by the final path if success.
		 * @remark
		 * We match file in following order.
		 *		+ Whether given file is absolute path. return if true.
		 *		+ User provided extra path.
		 *		+ Virtools temp folder.
		 * @return true if success
		*/
		bool ResolveFileName(std::string& u8_filename);

	protected:
		std::filesystem::path m_TempFolder;
		XContainer::XArray<std::filesystem::path> m_ExtraPathes;
	};

}
