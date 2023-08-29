#pragma once

#include <VTAll.hpp>
#include <CK2/CKFile.hpp>

namespace Unvirt::StructFormatter {

	void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo);

	void PrintObjectList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls, 
		const LibCmo::CK2::CKFileInfo& fileinfo,
		size_t page, size_t pageitems);
	void PrintManagerList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls, 
		size_t page, size_t pageitems);

	void PrintCKObject(const LibCmo::CK2::ObjImpls::CKObject*);
	void PrintCKBaseManager(const LibCmo::CK2::MgrImpls::CKBaseManager*);
	void PrintCKStateChunk(const LibCmo::CK2::CKStateChunk*);

}
