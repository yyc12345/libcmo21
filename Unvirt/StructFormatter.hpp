#pragma once

#include <CKFile.hpp>

namespace Unvirt::StructFormatter {

	void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo);
	void PrintObjectList(const LibCmo::CK2::XArray<LibCmo::CK2::CKFileObject>& ls, size_t page, size_t pageitems);
	void PrintManagerList(const LibCmo::CK2::XArray<LibCmo::CK2::CKFileManagerData>& ls, size_t page, size_t pageitems);

}
