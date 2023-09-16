#pragma once

#include "CKTypes.hpp"
#include "CKGlobals.hpp"

namespace LibCmo::CK2 {

#pragma region Preregistred Managers

	// Virtools Managers GUID second data is 0

	constexpr const CKDWORD OBJECT_MANAGER_GUID1 = 0x7cbb3b91;
	constexpr const CKDWORD ATTRIBUTE_MANAGER_GUID1 = 0x3d242466;
	constexpr const CKDWORD MESSAGE_MANAGER_GUID1 = 0x466a0fac;
	constexpr const CKDWORD FLOOR_MANAGER_GUID1 = 0x420936f9;
	constexpr const CKDWORD COLLISION_MANAGER_GUID1 = 0x38244712;
	constexpr const CKDWORD GRID_MANAGER_GUID1 = 0x7f004791;
	constexpr const CKDWORD TIME_MANAGER_GUID1 = 0x89ce7b32;
	constexpr const CKDWORD BEHAVIOR_MANAGER_GUID1 = 0x58d621ae;
	constexpr const CKDWORD INPUT_MANAGER_GUID1 = 0xf787c904;
	constexpr const CKDWORD SOUND_MANAGER_GUID1 = 0xdce135f6;
	constexpr const CKDWORD MIDI_MANAGER_GUID1 = 0x594154a6;
	constexpr const CKDWORD INTERFACE_MANAGER_GUID1 = 0x9a4b8e3d;
	constexpr const CKDWORD RENDER_MANAGER_GUID1 = 0xa213c8d5;
	constexpr const CKDWORD PARAMETER_MANAGER_GUID1 = 0x9ce57ab6;
	constexpr const CKDWORD PATH_MANAGER_GUID1 = 0x15fd54b9;
	constexpr const CKDWORD VARIABLE_MANAGER_GUID1 = 0x98cc3cc9;

	constexpr const CKGUID OBJECT_MANAGER_GUID{ OBJECT_MANAGER_GUID1 ,0 };
	constexpr const CKGUID ATTRIBUTE_MANAGER_GUID{ ATTRIBUTE_MANAGER_GUID1, 0 };
	constexpr const CKGUID MESSAGE_MANAGER_GUID{ MESSAGE_MANAGER_GUID1 ,0 };
	constexpr const CKGUID TIME_MANAGER_GUID{ TIME_MANAGER_GUID1 ,0 };
	constexpr const CKGUID SOUND_MANAGER_GUID{ SOUND_MANAGER_GUID1 ,0 };
	constexpr const CKGUID MIDI_MANAGER_GUID{ MIDI_MANAGER_GUID1 ,0 };
	constexpr const CKGUID INPUT_MANAGER_GUID{ INPUT_MANAGER_GUID1 ,0 };
	constexpr const CKGUID BEHAVIOR_MANAGER_GUID{ BEHAVIOR_MANAGER_GUID1 ,0 };
	constexpr const CKGUID FLOOR_MANAGER_GUID{ FLOOR_MANAGER_GUID1 ,0 };
	constexpr const CKGUID COLLISION_MANAGER_GUID{ COLLISION_MANAGER_GUID1 ,0 };
	constexpr const CKGUID GRID_MANAGER_GUID{ GRID_MANAGER_GUID1 ,0 };
	constexpr const CKGUID INTERFACE_MANAGER_GUID{ INTERFACE_MANAGER_GUID1 ,0 };
	constexpr const CKGUID RENDER_MANAGER_GUID{ RENDER_MANAGER_GUID1 ,0 };
	constexpr const CKGUID PARAMETER_MANAGER_GUID{ PARAMETER_MANAGER_GUID1 ,0 };
	constexpr const CKGUID PATH_MANAGER_GUID{ PATH_MANAGER_GUID1 ,0 };
	constexpr const CKGUID VARIABLE_MANAGER_GUID{ VARIABLE_MANAGER_GUID1 ,0 };

#pragma endregion

#pragma region Misc Constant

	/**
	 * @brief The identifier of Virtools file.
	*/
	constexpr const char CKNEMOFI[] = "Nemo Fi";
	/**
	 * @brief Current Version of CK Engine (Day/Month/Year)
	*/
	constexpr const CKDWORD CKVERSION = 0x13022002u;
	/**
	 * @brief Current Version of Dev
	*/
	constexpr const CKDWORD DEVBUILD = 0x02010001u;
	constexpr const CKDWORD DEVVERSION = 0u;
	constexpr const CKGUID VIRTOOLS_GUID = CKGUID(0x56495254u, 0x4f4f4c53u);

#pragma endregion

#pragma region Common Used Struct
	
	// a stupid forward decl to remove something
	/**
	 * @brief Storage class for filename extensions
	*/
	class CKFileExtension {
	public:
		CKFileExtension() : m_Data() {
			std::memset(m_Data, 0, c_DataLen);
		}
		CKFileExtension(CKSTRING s) : CKFileExtension() {
			SetExt(s);
		}
		CKFileExtension(const CKFileExtension& rhs) : CKFileExtension() {
			std::memcpy(m_Data, rhs.m_Data, c_DataLen);
		}
		CKFileExtension(CKFileExtension&& rhs) : CKFileExtension() {
			std::memmove(m_Data, rhs.m_Data, c_DataLen);
			std::memset(rhs.m_Data, 0, c_DataLen);
		}
		CKFileExtension& operator=(const CKFileExtension& rhs) {
			std::memcpy(m_Data, rhs.m_Data, c_DataLen);
			return *this;
		}
		CKFileExtension& operator=(CKFileExtension&& rhs) {
			std::memmove(m_Data, rhs.m_Data, c_DataLen);
			std::memset(rhs.m_Data, 0, c_DataLen);
			return *this;
		}

		void SetExt(CKSTRING s) {
			if (s == nullptr) {
				m_Data[0] = '\0';
			} else {
				if (s[0] == '.') ++s;	// skip dot
				size_t len = std::strlen(s);
				if (len > (c_DataLen - 1)) len = c_DataLen - 1;
				std::memcpy(m_Data, s, len);
			}
		}

		CKSTRING GetExt() const {
			return m_Data;
		}

		bool operator==(const CKFileExtension& rhs) const {
			return CKStrEqualI(m_Data, rhs.m_Data);
		}

	protected:
		static constexpr size_t c_DataLen = 4u;
		CKCHAR m_Data[c_DataLen];
	};
	
	/**
	 * The struct describe the bitmap handler's infomation,
	 * including its GUID and supported file extension.
	 * This struct also will store some parameters related to bitmap handler,
	 * such as jpeg compress level and etc. But currently there are no
	 * such parameters.
	*/
	class CKBitmapProperties {
	public:
		CKBitmapProperties() :
			m_ReaderGuid(), m_Ext() {}
		CKBitmapProperties(const CKGUID& guid, CKSTRING ext) :
			m_ReaderGuid(guid), m_Ext(ext) {}
		LIBCMO_DEFAULT_COPY_MOVE(CKBitmapProperties);

		CKGUID m_ReaderGuid; /**< CKGUID that uniquely identifies the reader that created this properties structure */
		CKFileExtension m_Ext; /**< File Extension of the image being described by this structure */
	};

#pragma endregion


}
