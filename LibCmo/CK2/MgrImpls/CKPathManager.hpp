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
		 * @param u8_temp 
		*/
		void SetTempFolder(CKSTRING u8_temp);
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
		 * @brief Finds a file in the paths
		 * @param u8_filename[inout] The given file path. overwritten by the final path if success.
		 * @return true if success
		*/
		bool ResolveFileName(std::string& u8_filename);

	protected:
		std::filesystem::path m_TempFolder;
	};

}
