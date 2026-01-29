#pragma once

#include <VTAll.hpp>

namespace Unvirt::StructFmt {
	
#define PRIuSIZET "zu"

	void PrintCKFileInfo(const LibCmo::CK2::CKFileInfo& fileinfo);

	void PrintObjectList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls, 
		const LibCmo::CK2::CKFileInfo& fileinfo,
		size_t page, size_t pageitems,
		bool full_detail);
	void PrintSearchedObjectList(
		const LibCmo::XContainer::XArray<size_t>& idxls,
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileObject>& ls, 
		const LibCmo::CK2::CKFileInfo& fileinfo,
		size_t page, size_t pageitems,
		bool full_detail);

	void PrintManagerList(
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls, 
		size_t page, size_t pageitems,
		bool full_detail);
	void PrintSearchedManagerList(
		const LibCmo::XContainer::XArray<size_t>& idxls, 
		const LibCmo::XContainer::XArray<LibCmo::CK2::CKFileManagerData>& ls, 
		size_t page, size_t pageitems,
		bool full_detail);

	void PrintCKObject(const LibCmo::CK2::ObjImpls::CKObject*);
	void PrintCKBaseManager(const LibCmo::CK2::MgrImpls::CKBaseManager*);
	void PrintCKStateChunk(const LibCmo::CK2::CKStateChunk*);

}
