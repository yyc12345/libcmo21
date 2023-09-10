#include "CKBitmapData.hpp"

namespace LibCmo::CK2 {

#pragma region Assist RW Functions

	bool CKBitmapData::ReadSpecificFormatBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* desc) {
		return false;
	}

	bool CKBitmapData::ReadRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* desc) {
		return false;
	}

	bool CKBitmapData::ReadOldRawBitmap(CKStateChunk* chk, VxMath::VxImageDescEx* desc) {
		return false;
	}

	void CKBitmapData::WriteSpecificFormatBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* desc) {

	}

	void CKBitmapData::WriteRawBitmap(CKStateChunk* chk, const VxMath::VxImageDescEx* desc) {
		
	}

#pragma endregion


	CKBitmapData::CKBitmapData(CKContext* ctx) {}

	CKBitmapData::~CKBitmapData() {}

}
