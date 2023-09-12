#pragma once

#include "CKGlobals.hpp"

namespace LibCmo::CK2 {
	
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

}
