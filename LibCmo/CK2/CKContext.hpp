#pragma once

#include "../VTAll.hpp"
#include <filesystem>
#include <map>
#include <deque>
#include <functional>

namespace LibCmo::CK2 {

	/**
	@brief Main Interface Object
	@details
		+ The CKContext object is the heart of all Virtools based applications, It is the first object that should be created in order to
		use Virtools SDK. A CKContext can be simply created by calling its constructor.
		+ The CKContext object act as the central interface to create/destroy objects,to access managers, to load/save files.
		+ Several CKContext can be created inside a same process (in multiple threads for example) but objects created
		by a specific CKContext must not be used in other contextes.
	@see CKContext::CreateObject, CKContext::GetObject, CKContext::DestroyObject
	*/
	class CKContext {
	public:
		CKContext();
		~CKContext();
		LIBCMO_DISABLE_COPY_MOVE(CKContext);

		/**
		 * @brief Simply clear all CKContext to restore its status.
		*/
		void ClearAll();

		// ========== Common Managers ==========

		MgrImpls::CKObjectManager* GetObjectManager();
		MgrImpls::CKPathManager* GetPathManager();

		CKDWORD GetManagerCount();
		MgrImpls::CKBaseManager* GetManager(CKDWORD index);

		void ExecuteManagersOnPreClearAll();
		void ExecuteManagersOnPostClearAll();
		void ExecuteManagersSequenceToBeDeleted();
		void ExecuteManagersSequenceDeleted();

		// ========== File Save/Load Options ==========

		void SetCompressionLevel(CKINT level);
		CKINT GetCompressionLevel();

		void SetFileWriteMode(CK_FILE_WRITEMODE mode);
		CK_FILE_WRITEMODE GetFileWriteMode();

		CK_TEXTURE_SAVEOPTIONS GetGlobalImagesSaveOptions();
		void SetGlobalImagesSaveOptions(CK_TEXTURE_SAVEOPTIONS Options);

		CKBitmapProperties* GetGlobalImagesSaveFormat();
		void SetGlobalImagesSaveFormat(CKBitmapProperties* Format);

		CK_SOUND_SAVEOPTIONS GetGlobalSoundsSaveOptions();
		void SetGlobalSoundsSaveOptions(CK_SOUND_SAVEOPTIONS Options);

		// ========== Encoding utilities ==========

		void GetUtf8String(const std::string& native_name, std::string& u8_name);
		void GetNativeString(const std::string& u8_name, std::string& native_name);
		void SetEncoding(const std::vector<std::string> encoding_series);

		// ========== Temp IO utilities ==========

		void SetTempPath(CKSTRING u8_temp);
		std::string GetTempFilePath(CKSTRING u8_filename);

		// ========== Print utilities ==========

		using OutputCallback = std::function<void(CKSTRING)>;
		void OutputToConsole(CKSTRING str);
		void OutputToConsoleEx(CKSTRING fmt, ...);
		void SetOutputCallback(OutputCallback cb);

	protected:
		// ========== Common Managers ==========
		void ExecuteManagersGeneral(std::function<void(MgrImpls::CKBaseManager*)> fct);
		XContainer::XArray<MgrImpls::CKBaseManager*> m_ManagerList;

		// ========== File Save/Load Options ==========
		CKINT m_CompressionLevel;
		CK_FILE_WRITEMODE m_FileWriteMode;
		CK_TEXTURE_SAVEOPTIONS m_GlobalImagesSaveOptions;
		CK_SOUND_SAVEOPTIONS m_GlobalSoundsSaveOptions;
		CKBitmapProperties* m_GlobalImagesSaveFormat;

		// ========== Encoding utilities ==========

		std::vector<EncodingHelper::ENCODING_TOKEN> m_NameEncoding;

		// ========== Temp IO utilities ==========

		std::filesystem::path m_TempFolder;

		// ========== Print utilities ==========

		OutputCallback m_OutputCallback;
	};

}
