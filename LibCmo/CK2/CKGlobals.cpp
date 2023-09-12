#include "../VTUtils.hpp"
#if defined(LIBCMO_OS_WIN32)
#define ZLIB_WINAPI
#endif
#include <zconf.h>

#include "../VTEncoding.hpp"
#include "CKGlobals.hpp"
#include <map>
#include <algorithm>
#include <zlib.h>

#include "ObjImpls/CKObject.hpp"
#include "ObjImpls/CKSceneObject.hpp"
#include "ObjImpls/CKBeObject.hpp"
#include "ObjImpls/CKGroup.hpp"
#include "ObjImpls/CKRenderObject.hpp"
#include "ObjImpls/CK3dEntity.hpp"
#include "ObjImpls/CK3dObject.hpp"
#include "ObjImpls/CKTexture.hpp"

namespace LibCmo::CK2 {

#pragma region Compression utilities

	void* CKPackData(const void* Data, CKDWORD size, CKDWORD& NewSize, CKINT compressionlevel) {
		uLong boundary = compressBound(static_cast<uLong>(size));
		char* DestBuffer = new char[boundary];

		uLongf _destLen = static_cast<uLongf>(boundary);
		if (compress2(
			reinterpret_cast<Bytef*>(DestBuffer), &_destLen,
			reinterpret_cast<const Bytef*>(Data), static_cast<uLong>(size),
			static_cast<int>(compressionlevel)) != Z_OK) {
			NewSize = 0;
			delete[] DestBuffer;
			return nullptr;
		}

		NewSize = static_cast<CKDWORD>(_destLen);
		return DestBuffer;
	}

	void* CKUnPackData(CKDWORD DestSize, const void* SrcBuffer, CKDWORD SrcSize) {
		char* DestBuffer = new char[DestSize];

		uLongf cache = DestSize;
		if (uncompress(
			reinterpret_cast<Bytef*>(DestBuffer), &cache,
			reinterpret_cast<const Bytef*>(SrcBuffer), static_cast<uLong>(SrcSize)) != Z_OK) {
			delete[] DestBuffer;
			return nullptr;
		}

		return DestBuffer;
	}

	CKDWORD CKComputeDataCRC(const void* data, CKDWORD size, CKDWORD PreviousCRC) {
		return static_cast<CKDWORD>(adler32(
			static_cast<uLong>(PreviousCRC),
			reinterpret_cast<const Bytef*>(data),
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

	CK_CLASSID CKClassGetNewIdentifier() {
		size_t classsize = g_CKClassInfo.size();
		if (classsize < static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID)) {
			return CK_CLASSID::CKCID_MAXCLASSID;
		} else {
			return static_cast<CK_CLASSID>(classsize);
		}
	}

	static void ComputeParentsTable(CKClassDesc& desc) {
		// if it has done, do not process it again.
		if (desc.Done) return;

		// find direct parent
		CKClassDesc& parent = g_CKClassInfo[static_cast<size_t>(desc.Parent)];
		if (!parent.IsValid) LIBPANIC("No such CK_CLASSID.");

		// if it is not self inheritance, call recursively
		if (desc.Self != desc.Parent) {
			ComputeParentsTable(parent);
		}

		// copy parent's parents
		desc.Parents = parent.Parents;
		// and set self as its parent
		desc.Parents[static_cast<size_t>(desc.Self)] = true;

		// set derivation level
		desc.DerivationLevel = parent.DerivationLevel + 1;
		
		// set done
		desc.Done = true;
	}
	static void CKBuildClassHierarchyTable() {
		// set Done to false and resize all XBitArray
		size_t classCount = g_CKClassInfo.size();
		for (auto& item : g_CKClassInfo) {
			if (!item.IsValid) continue;

			item.Done = false;
			item.Parents.resize(classCount, false);
			item.Children.resize(classCount, false);
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
				if (!g_CKClassInfo[idx].IsValid) continue;

				// if this idx is its parent,
				// add self to parent.
				if (item.Parents[idx]) {
					g_CKClassInfo[idx].Children[static_cast<size_t>(item.Self)] = true;
				}
			}
		}
	}
	void CKClassRegister(CK_CLASSID cid, CK_CLASSID parentCid,
		CKClassCreationFct createFct, CKClassReleaseFct relFct, CKClassNameFct nameFct) {

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
		desc.CreationFct = createFct;
		desc.ReleaseFct = relFct;
		desc.NameFct = nameFct;
		g_CKClassInfo[intcid] = std::move(desc);
	}

#pragma endregion

#pragma region Class Hierarchy Management

	static bool GetClassIdIndex(CK_CLASSID cid, size_t& intcid) {
		intcid = static_cast<size_t>(cid);
		if (intcid >= g_CKClassInfo.size()) return false;
		if (!g_CKClassInfo[intcid].IsValid) return false;
		return true;
	}

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

#pragma endregion

#pragma region Initializations functions

	CKERROR CKStartUp() {

		// reserve class info array.
		g_CKClassInfo.reserve(static_cast<size_t>(CK_CLASSID::CKCID_MAXCLASSID));

		// todo: add class type registrations
#define EasyClassReg(clsname, cid, parentCid, strName) \
CKClassRegister(cid, parentCid, \
	[](CKContext* ctx, CK_ID id, CKSTRING name) -> ObjImpls::CKObject* { return new clsname(ctx, id, name); }, \
	[](CKContext* ctx, ObjImpls::CKObject* obj) -> void { delete obj; }, \
	[]() -> CKSTRING { return strName; });

		EasyClassReg(ObjImpls::CKObject, CK_CLASSID::CKCID_OBJECT, CK_CLASSID::CKCID_OBJECT, "Basic Object");
		EasyClassReg(ObjImpls::CKSceneObject, CK_CLASSID::CKCID_SCENEOBJECT, CK_CLASSID::CKCID_OBJECT, "Scene Object");
		EasyClassReg(ObjImpls::CKBeObject, CK_CLASSID::CKCID_BEOBJECT, CK_CLASSID::CKCID_SCENEOBJECT, "Behavioral Object");
		EasyClassReg(ObjImpls::CKGroup, CK_CLASSID::CKCID_GROUP, CK_CLASSID::CKCID_BEOBJECT, "Group");
		EasyClassReg(ObjImpls::CKRenderObject, CK_CLASSID::CKCID_RENDEROBJECT, CK_CLASSID::CKCID_BEOBJECT, "Render Object");
		EasyClassReg(ObjImpls::CK3dEntity, CK_CLASSID::CKCID_3DENTITY, CK_CLASSID::CKCID_RENDEROBJECT, "3D Entity");
		EasyClassReg(ObjImpls::CK3dObject, CK_CLASSID::CKCID_3DOBJECT, CK_CLASSID::CKCID_3DENTITY, "3D Object");
		EasyClassReg(ObjImpls::CKTexture, CK_CLASSID::CKCID_TEXTURE, CK_CLASSID::CKCID_BEOBJECT, "Texture");

#undef EasyClassReg

		/*
				// register CKObjects
		m_ObjectsCreationMap{
			{CK_CLASSID::CKCID_OBJECT, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKObject(ctx, id, name); })},
			{CK_CLASSID::CKCID_SCENEOBJECT, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKSceneObject(ctx, id, name); })},
			{CK_CLASSID::CKCID_BEOBJECT, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKBeObject(ctx, id, name); })},

			{CK_CLASSID::CKCID_GROUP, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKGroup(ctx, id, name); })},
			{CK_CLASSID::CKCID_MESH, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKMesh(ctx, id, name); })},
			{CK_CLASSID::CKCID_TEXTURE, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKTexture(ctx, id, name); })},
			{CK_CLASSID::CKCID_MATERIAL, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKMaterial(ctx, id, name); })},
			{CK_CLASSID::CKCID_RENDEROBJECT, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKRenderObject(ctx, id, name); })},
			{CK_CLASSID::CKCID_3DENTITY, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CK3dEntity(ctx, id, name); })},

			{CK_CLASSID::CKCID_PARAMETERIN, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKParameterIn(ctx, id, name); })},
			{CK_CLASSID::CKCID_PARAMETER, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKParameter(ctx, id, name); })},
			{CK_CLASSID::CKCID_PARAMETEROUT, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKParameterOut(ctx, id, name); })},
			{CK_CLASSID::CKCID_PARAMETERLOCAL, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKParameterLocal(ctx, id, name); })},
			{CK_CLASSID::CKCID_PARAMETEROPERATION, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKParameterOperation(ctx, id, name); })},
			{CK_CLASSID::CKCID_BEHAVIORLINK, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKBehaviorLink(ctx, id, name); })},
			{CK_CLASSID::CKCID_BEHAVIORIO, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKBehaviorLink(ctx, id, name); })},
			{CK_CLASSID::CKCID_BEHAVIOR, ([](CKContext* ctx, CK_ID id, CKSTRING name) ->CKObjectImplements::CKObject* { return new(std::nothrow) CKObjectImplements::CKBehavior(ctx, id, name); })}
	},
		// register CKBaseManagers
		m_ManagersCreationMap{
			{ATTRIBUTE_MANAGER_GUID, ([](CKContext* ctx, CK_ID id) ->CKManagerImplements::CKBaseManager* { return new(std::nothrow) CKManagerImplements::CKAttributeManager(ctx, id); })},
	}
		*/

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
