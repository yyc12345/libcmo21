#pragma once

namespace LibCmo {

	void* CKUnPackData(CKINT DestSize, const void* SrcBuffer, CKINT SrcSize);
	CKDWORD CKComputeDataCRC(const void* data, size_t size, CKDWORD PreviousCRC = 0);

}
