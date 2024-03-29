#include "../VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#endif
#include <zconf.h>
#include <zlib.h>

#include "CKGlobals.hpp"

#include <algorithm>
#include <initializer_list>

#include "ObjImpls/CKObject.hpp"
#include "ObjImpls/CKSceneObject.hpp"
#include "ObjImpls/CKBeObject.hpp"
#include "ObjImpls/CKGroup.hpp"
#include "ObjImpls/CKRenderObject.hpp"
#include "ObjImpls/CK3dEntity.hpp"
#include "ObjImpls/CK3dObject.hpp"
#include "ObjImpls/CKTexture.hpp"
#include "ObjImpls/CKMaterial.hpp"
#include "ObjImpls/CKMesh.hpp"

namespace LibCmo::CK2 {

#pragma region Compression utilities

	void* CKPackData(const void* Data, CKDWORD size, CKDWORD& NewSize, CKINT compressionlevel) {
		uLong boundary = compressBound(static_cast<uLong>(size));
		CKBYTE* DestBuffer = new CKBYTE[boundary];

		uLongf _destLen = static_cast<uLongf>(boundary);
		if (compress2(
			reinterpret_cast<Bytef*>(DestBuffer), &_destLen,
			static_cast<const Bytef*>(Data), static_cast<uLong>(size),
			static_cast<int>(compressionlevel)) != Z_OK) {
			NewSize = 0;
			delete[] DestBuffer;
			return nullptr;
		}

		NewSize = static_cast<CKDWORD>(_destLen);
		return DestBuffer;
	}

	void* CKUnPackData(CKDWORD DestSize, const void* SrcBuffer, CKDWORD SrcSize) {
		CKBYTE* DestBuffer = new CKBYTE[DestSize];

		uLongf cache = DestSize;
		if (uncompress(
			reinterpret_cast<Bytef*>(DestBuffer), &cache,
			static_cast<const Bytef*>(SrcBuffer), static_cast<uLong>(SrcSize)) != Z_OK) {
			delete[] DestBuffer;
			return nullptr;
		}

		return DestBuffer;
	}

	CKDWORD CKComputeDataCRC(const void* data, CKDWORD size, CKDWORD PreviousCRC) {
		return static_cast<CKDWORD>(adler32(
			static_cast<uLong>(PreviousCRC),
			static_cast<const Bytef*>(data),
			static_cast<uInt>(size)
			));
	}

#pragma endregion

#pragma region String Utilities 

	bool CKStrEqual(CKSTRING str1, CKSTRING str2) {
		if (str1 == nullptr) {
			if (str2 == nullptr) return true;
			else return false;
		} else {
			if (str2 == nullptr) return false;
			else {
				return std::strcmp(str1, str2) == 0;
			}
		}
	}

	bool CKStrEqualI(CKSTRING str1, CKSTRING str2) {
		if (str1 == nullptr) {
			if (str2 == nullptr) return true;
			else return false;
		} else {
			if (str2 == nullptr) return false;
			else {
				// do real cmp
				size_t i = 0;
				while (str1[i] != '\0' && str2[i] != '\0') {
					if (std::tolower(str1[i]) != std::tolower(str2[i])) return false;
					++str1;
					++str2;
				}

				// !XOR the result, if both of them is zero, return true(1)
				return !((str1[i] != '\0') ^ (str2[i] != '\0'));
			}
		}
	}

	bool CKStrEmpty(CKSTRING strl) {
		if (strl == nullptr) return true;
		return strl[0] == '\0';
	}

#pragma endregion

#pragma region CKClass Registration

	static XContainer::XArray<CKClassDesc> g_CKClassInfo;
	
	static bool GetClassIdIndex(CK_CLASSID cid, size_t& intcid) {
		intcid = static_cast<size_t>(cid);
		if (intcid >= g_CKClassInfo.size()) return false;
		if (!g_CKClassInfo[intcid].IsValid) return false;
		return true;
	}

	void CKClassNeedNotificationFrom(CK_CLASSID listener, CK_CLASSID listenTo) {
		size_t idxListener, idxListenTo;
		if (!GetClassIdIndex(listener, idxListener) || !GetClassIdIndex(listenTo, idxListenTo)) return;
		
		XContainer::NSXBitArray::Set(g_CKClassInfo[idxListener].ToBeNotify, static_cast<CKDWORD>(idxListenTo));
	}

	CK_CLASSID CKClassGetNewIdentifier() {
		size_t classsize = g_CKClassInfo.size();
		if (classsize < static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID)) {
			return CK_CLASSID::CKCID_MAXCLASSID;
		} else {
			return static_cast<CK_CLASSID>(classsize);
		}
	}

	void CKClassRegister(CK_CLASSID cid, CK_CLASSID parentCid,
		CKClassRegisterFct regFct, CKClassCreationFct createFct, CKClassReleaseFct relFct, CKClassNameFct nameFct) {

		// resize class desc array
		size_t intcid = static_cast<size_t>(cid);
		if (intcid >= g_CKClassInfo.size()) {
			g_CKClassInfo.resize(intcid + 1);
		}

		// emplace desc
		CKClassDesc desc;
		desc.IsValid = true;
		desc.Self = cid;
		desc.Parent = parentCid;
		desc.RegisterFct = regFct;
		desc.CreationFct = createFct;
		desc.ReleaseFct = relFct;
		desc.NameFct = nameFct;
		g_CKClassInfo[intcid] = std::move(desc);
	}

#pragma endregion

#pragma region Class Hierarchy Management

	CKDWORD CKGetClassCount() {
		return static_cast<CKDWORD>(g_CKClassInfo.size());
	}

	const CKClassDesc* CKGetClassDesc(CK_CLASSID cid) {
		size_t intcid;
		if (!GetClassIdIndex(cid, intcid)) return nullptr;
		return &g_CKClassInfo[intcid];
	}

	CKSTRING CKClassIDToString(CK_CLASSID cid) {
		const CKClassDesc* desc = CKGetClassDesc(cid);
		if (desc == nullptr) return "Undefined Type";
		else return desc->NameFct();
	}

	bool CKIsChildClassOf(CK_CLASSID child, CK_CLASSID parent) {
		size_t intchild, intparent;
		if (!GetClassIdIndex(child, intchild) || !GetClassIdIndex(parent, intparent)) return false;
		return g_CKClassInfo[intchild].Parents[intparent];
	}

	CK_CLASSID CKGetParentClassID(CK_CLASSID child) {
		const CKClassDesc* desc = CKGetClassDesc(child);
		if (desc == nullptr) return CK_CLASSID::CKCID_OBJECT;
		return desc->Parent;
	}

	CK_CLASSID CKGetCommonParent(CK_CLASSID cid1, CK_CLASSID cid2) {
		// I don't know algorithm, I just copy the decompiled code.
		while (true) {
			if (CKIsChildClassOf(cid1, cid2)) return cid2;
			if (CKIsChildClassOf(cid2, cid1)) break;

			cid2 = CKGetParentClassID(cid1);
			cid1 = cid2;
		}

		return cid1;
	}

	bool CKIsNeedNotify(CK_CLASSID listener, CK_CLASSID deletedObjCid) {
		const CKClassDesc* desc = CKGetClassDesc(listener);
		if (desc == nullptr) return false;
		return XContainer::NSXBitArray::IsSet(desc->CommonToBeNotify, static_cast<CKDWORD>(deletedObjCid));
	}

	XContainer::XBitArray CKGetAllNotifyClassID(const XContainer::XBitArray& delObjCids) {
		XContainer::XBitArray result;

		for (size_t i = 0; i < delObjCids.size(); ++i) {
			if (!XContainer::NSXBitArray::IsSet(delObjCids, static_cast<CKDWORD>(i))) continue;

			const CKClassDesc* desc = CKGetClassDesc(static_cast<CK_CLASSID>(i));
			if (desc == nullptr) continue;

			XContainer::NSXBitArray::Or(result, desc->ToNotify);
		}

		return result;
	}

#pragma endregion

#pragma region Initializations functions

	/*
	A small hint for ToBeNotify, CommonToBeNotify and ToNotify
	Assume that A need notification from B, we can see it as that A want buy something from B.
	This relation is represented in ToBeNotify, a pure relation without any inhertance hierarchy.

	Ok, now we assume A have children AA, B also have children BB.
	Because B is a businessman, so his children BB also is a bussinessman.
	B and BB have the same goods so A can buy his stuff from both of B and BB.
	This is the first step executed by ComputeParentsNotifyTable().
	In this step, the function expand existing business relations to all possible business relations (expand to businessman's children)
	
	Image there is a candy store, C. Because AA still is a kids.
	So AA want to buy something from C. Now C is in his ToBeNotify.
	Additionally, A, the parent of AA, force AA to buy something from B, just for A himself.
	For AA, he does not need want to buy something from B, but his parent A order he to do.
	This is the second step executed by ComputeParentsNotifyTable().
	For AA, his parent's relations also need to be merged after he processed his relations with C like I introduced previously.
	
	Now, AA have a full business list writing all trades he can do.
	This is represented as CommonToBeNotify.
	In this time, AA's ToBeNotify only have C, but his CommonToBeNotify have B, BB and C.

	So now, let we change the view from A to B.
	B want to know whom I can sell something to.
	Because a full trades list are created from A side.
	The better solution is just ask the guest: do you want to buy something from me?
	This operation will fill ToNotify and is implemented at ComputeHierarchyTable().
	
	At the end of this story,
	All bussiness man can use ToNofity to see whom they want to sell something to.
	And all buyer can use CommonToBeNofity to check who they can buy something from.
	ToBeNotify is just a list to indicate the initial bussiness relation and will never used anymore after real relation established.
	*/

	static void ComputeParentsTable(CKClassDesc& desc) {
		// if it has done, do not process it again.
		if (desc.Done) return;

		// find direct parent
		CKClassDesc& parent = g_CKClassInfo[static_cast<size_t>(desc.Parent)];
		if (!parent.IsValid) LIBCMO_PANIC("No such CK_CLASSID.");

		// if it is not self inheritance, call recursively
		if (desc.Self != desc.Parent) {
			ComputeParentsTable(parent);
		}

		// copy parent's parents
		desc.Parents = parent.Parents;
		// and set self as its parent
		XContainer::NSXBitArray::Set(desc.Parents, static_cast<CKDWORD>(desc.Self));

		// set derivation level
		desc.DerivationLevel = parent.DerivationLevel + 1;
		
		// set done
		desc.Done = true;
	}
	static void ComputeParentsNotifyTable(CKClassDesc& desc) {
		// if it has done, do not process it again.
		if (desc.Done) return;
		
		// find direct parent
		CKClassDesc& parent = g_CKClassInfo[static_cast<size_t>(desc.Parent)];
		if (!parent.IsValid) LIBCMO_PANIC("No such CK_CLASSID.");

		// if it is not self inheritance, call recursively
		if (desc.Self != desc.Parent) {
			ComputeParentsNotifyTable(parent);
		}
		
		// add all children of ToBeNofity list
		for (CKDWORD idx = 0; idx < desc.ToBeNotify.size(); ++idx) {
			if (!XContainer::NSXBitArray::IsSet(desc.ToBeNotify, idx)) 
				continue;

			CKClassDesc& target = g_CKClassInfo[idx];
			if (!target.IsValid) continue;
			XContainer::NSXBitArray::Or(desc.CommonToBeNotify, target.Children);
		}
		// and merge parent notify list
		XContainer::NSXBitArray::Or(desc.CommonToBeNotify, parent.CommonToBeNotify);

		// set done
		desc.Done = true;
	}
	static void CKBuildClassHierarchyTable() {
		size_t classCount = g_CKClassInfo.size();

		// ===== Build Inhertance Hierarchy =====
		// set Done to false and resize inhertance XBitArray
		for (auto& item : g_CKClassInfo) {
			if (!item.IsValid) continue;
			item.Done = false;
			XContainer::NSXBitArray::Resize(item.Parents, static_cast<CKDWORD>(classCount));
			XContainer::NSXBitArray::Resize(item.Children, static_cast<CKDWORD>(classCount));
		}
		// compute parents
		for (auto& item : g_CKClassInfo) {
			if (!item.IsValid) continue;
			ComputeParentsTable(item);
		}
		// compute children by parents table
		// iterate CKClassDesc::Parents and register it self to gotten parents
		for (auto& item : g_CKClassInfo) {
			if (!item.IsValid) continue;

			for (size_t idx = 0; idx < classCount; ++idx) {
				CKClassDesc& checking = g_CKClassInfo[idx];
				if (!checking.IsValid) continue;

				// if this idx is its parent,
				// add self to parent.
				if (XContainer::NSXBitArray::IsSet(item.Parents, static_cast<CKDWORD>(idx))) {
					XContainer::NSXBitArray::Set(checking.Children, static_cast<CKDWORD>(item.Self));
				}
			}
		}

		// ===== Register =====
		// run register
		for (auto& item : g_CKClassInfo) {
			if (!item.IsValid || item.RegisterFct == nullptr) continue;
			item.RegisterFct();
		}
		
		// ===== Build Notify Hierarchy =====
		// set array first
		for (auto& item : g_CKClassInfo) {
			if (!item.IsValid) continue;
			item.Done = false;
			XContainer::NSXBitArray::Resize(item.ToBeNotify, static_cast<CKDWORD>(classCount));
			XContainer::NSXBitArray::Resize(item.CommonToBeNotify, static_cast<CKDWORD>(classCount));
			XContainer::NSXBitArray::Resize(item.ToNotify, static_cast<CKDWORD>(classCount));
		}
		// compute CommonToBeNotify
		for (auto& item : g_CKClassInfo) {
			if (!item.IsValid) continue;
			ComputeParentsNotifyTable(item);
		}
		// compute ToNotify
		for (CKDWORD idx = 0; idx < classCount; ++idx) {
			CKClassDesc& thisDesc = g_CKClassInfo[idx];
			if (!thisDesc.IsValid) continue;

			for (auto& checking : g_CKClassInfo) {
				if (!checking.IsValid) continue;

				// if checkingDesc order me, add it.
				if (XContainer::NSXBitArray::IsSet(checking.CommonToBeNotify, idx)) {
					XContainer::NSXBitArray::Set(thisDesc.ToNotify, static_cast<CKDWORD>(checking.Self));
				}
			}
		}
	}

	static void NeedNotificationWrapper(CK_CLASSID thiscid, std::initializer_list<CK_CLASSID> il) {
		for (auto it = il.begin(); it != il.end(); ++it) {
			CKClassNeedNotificationFrom(thiscid, *it);
		}
	}
	CKERROR CKStartUp() {
		// reserve class info array.
		g_CKClassInfo.reserve(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));

		// MARK: add class type registrations here
#define EasyClassReg(clsname, cid, parentCid, strName) \
CKClassRegister(cid, parentCid, \
	nullptr, \
	[](CKContext* ctx, CK_ID id, CKSTRING name) -> ObjImpls::CKObject* { return new clsname(ctx, id, name); }, \
	[](CKContext* ctx, ObjImpls::CKObject* obj) -> void { delete obj; }, \
	[]() -> CKSTRING { return strName; });
#define EasyClassRegWithNotify(clsname, cid, parentCid, strName, notifyCids) \
CKClassRegister(cid, parentCid, \
	[]() -> void { NeedNotificationWrapper(cid, notifyCids); }, \
	[](CKContext* ctx, CK_ID id, CKSTRING name) -> ObjImpls::CKObject* { return new clsname(ctx, id, name); }, \
	[](CKContext* ctx, ObjImpls::CKObject* obj) -> void { delete obj; }, \
	[]() -> CKSTRING { return strName; });

		EasyClassReg(ObjImpls::CKObject, CK_CLASSID::CKCID_OBJECT, CK_CLASSID::CKCID_OBJECT, "Basic Object");
		EasyClassReg(ObjImpls::CKSceneObject, CK_CLASSID::CKCID_SCENEOBJECT, CK_CLASSID::CKCID_OBJECT, "Scene Object");
		EasyClassReg(ObjImpls::CKBeObject, CK_CLASSID::CKCID_BEOBJECT, CK_CLASSID::CKCID_SCENEOBJECT, "Behavioral Object");
		EasyClassRegWithNotify(ObjImpls::CKGroup, CK_CLASSID::CKCID_GROUP, CK_CLASSID::CKCID_BEOBJECT, "Group", { CK_CLASSID::CKCID_BEOBJECT });
		EasyClassReg(ObjImpls::CKRenderObject, CK_CLASSID::CKCID_RENDEROBJECT, CK_CLASSID::CKCID_BEOBJECT, "Render Object");
		EasyClassRegWithNotify(ObjImpls::CK3dEntity, CK_CLASSID::CKCID_3DENTITY, CK_CLASSID::CKCID_RENDEROBJECT, "3D Entity", { CK_CLASSID::CKCID_MESH });
		EasyClassReg(ObjImpls::CK3dObject, CK_CLASSID::CKCID_3DOBJECT, CK_CLASSID::CKCID_3DENTITY, "3D Object");
		EasyClassReg(ObjImpls::CKTexture, CK_CLASSID::CKCID_TEXTURE, CK_CLASSID::CKCID_BEOBJECT, "Texture");
		EasyClassRegWithNotify(ObjImpls::CKMaterial, CK_CLASSID::CKCID_MATERIAL, CK_CLASSID::CKCID_BEOBJECT, "Material", { CK_CLASSID::CKCID_TEXTURE });
		EasyClassRegWithNotify(ObjImpls::CKMesh, CK_CLASSID::CKCID_MESH, CK_CLASSID::CKCID_BEOBJECT, "Mesh", { CK_CLASSID::CKCID_MATERIAL });

#undef EasyClassReg
#undef EasyClassRegWithNotify

		CKBuildClassHierarchyTable();

		return CKERROR::CKERR_OK;
	}

	CKERROR CKShutdown() {
		// free class indo
		g_CKClassInfo.clear();

		return CKERROR::CKERR_OK;
	}

#pragma endregion


}
