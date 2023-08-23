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

namespace LibCmo::CK2 {

#pragma region Compression utilities

	void* CKPackData(const void* Data, CKINT size, CKINT& NewSize, CKINT compressionlevel) {
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

		NewSize = static_cast<CKINT>(_destLen);
		return DestBuffer;
	}

	void* CKUnPackData(CKINT DestSize, const void* SrcBuffer, CKINT SrcSize) {
		char* DestBuffer = new char[DestSize];

		uLongf cache = DestSize;
		if (uncompress(
			reinterpret_cast<Bytef*>(DestBuffer), &cache,
			reinterpret_cast<const Bytef*>(SrcBuffer), SrcSize) != Z_OK) {
			delete[] DestBuffer;
			return nullptr;
		}

		return DestBuffer;
	}

	CKDWORD CKComputeDataCRC(const void* data, CKINT size, CKDWORD PreviousCRC) {
		return static_cast<CKDWORD>(adler32(
			static_cast<uLong>(PreviousCRC),
			reinterpret_cast<const Bytef*>(data),
			static_cast<uInt>(size)
			));
	}

#pragma endregion

#pragma region CKClass Registration

	static XContainer::XArray<CKClassDesc> g_CKClassInfo;
	static std::map<CK_CLASSID, size_t> g_CKClassInfoId2Idx;
	static CK_CLASSID g_CKClassInfoMaxID = static_cast<CK_CLASSID>(0);

	static void ComputeParentsTable(CKClassDesc& desc) {
		// if it has done, do not process it again.
		if (desc.Done) return;

		// find direct parent
		auto finder = g_CKClassInfoId2Idx.find(desc.Parent);
		if (finder == g_CKClassInfoId2Idx.end()) std::abort();
		CKClassDesc& parent = g_CKClassInfo[finder->second];

		// if it is not self inheritance, call recursively
		if (desc.Self != desc.Parent) {
			ComputeParentsTable(parent);
		}

		// copy parent's parents
		desc.Parents = parent.Parents;
		// and set self as its parent
		finder = g_CKClassInfoId2Idx.find(desc.Self);
		if (finder == g_CKClassInfoId2Idx.end()) std::abort();
		desc.Parents[finder->second] = true;

		// set derivation level
		desc.DerivationLevel = parent.DerivationLevel + 1;
		
		// set done
		desc.Done = CKTRUE;
	}
	static void CKBuildClassHierarchyTable() {
		// set Done to false and resize all XBitArray
		size_t classCount = g_CKClassInfo.size();
		for (auto& item : g_CKClassInfo) {
			item.Done = CKFALSE;
			item.Parents.resize(classCount, false);
			item.Children.resize(classCount, false);
		}
		// compute parents
		for (auto& item : g_CKClassInfo) {
			ComputeParentsTable(item);
		}
		// compute children by parents table
		// iterate CKClassDesc::Parents and register it self to gotten parents
		for (auto& item : g_CKClassInfo) {
			auto finder = g_CKClassInfoId2Idx.find(item.Self);
			if (finder == g_CKClassInfoId2Idx.end()) std::abort();
			size_t selfidx = finder->second;

			for (size_t idx = 0; idx < classCount; ++idx) {
				// if this idx is its parent,
				// add self to parent.
				if (item.Parents[idx]) {
					g_CKClassInfo[idx].Children[selfidx] = true;
				}
			}
		}
	}
	void CKClassRegister(CK_CLASSID cid, CK_CLASSID parentCid,
		CKClassCreationFct createFct, CKClassReleaseFct relFct, CKClassNameFct nameFct) {

		CKClassDesc desc;
		desc.Self = cid;
		desc.Parent = parentCid;
		desc.CreationFct = createFct;
		desc.ReleaseFct = relFct;
		desc.NameFct = nameFct;
		g_CKClassInfoId2Idx.emplace(cid, g_CKClassInfo.size());
		g_CKClassInfo.emplace_back(std::move(desc));
		g_CKClassInfoMaxID = std::max(g_CKClassInfoMaxID, cid);
	}

#pragma endregion

#pragma region Class Hierarchy Management

	CKINT CKGetClassCount() {
		return g_CKClassInfo.size();
	}

	const CKClassDesc* CKGetClassDesc(CK_CLASSID cid) {
		auto finder = g_CKClassInfoId2Idx.find(cid);
		if (finder == g_CKClassInfoId2Idx.end()) return nullptr;
		return g_CKClassInfo.data() + finder->second;
	}

	CKSTRING CKClassIDToString(CK_CLASSID cid) {
		auto finder = g_CKClassInfoId2Idx.find(cid);
		if (finder == g_CKClassInfoId2Idx.end() || g_CKClassInfo[finder->second].NameFct == nullptr) return "Invalid Class Identifier";
		return g_CKClassInfo[finder->second].NameFct();
	}

	CKBOOL CKIsChildClassOf(CK_CLASSID child, CK_CLASSID parent) {
		// get corresponding index first
		auto finder = g_CKClassInfoId2Idx.find(child);
		if (finder == g_CKClassInfoId2Idx.end()) std::abort();
		size_t child_idx = finder->second;
		finder = g_CKClassInfoId2Idx.find(parent);
		if (finder == g_CKClassInfoId2Idx.end()) std::abort();
		size_t parent_idx = finder->second;

		return g_CKClassInfo[child_idx].Parents[parent_idx];
	}

	CK_CLASSID CKGetParentClassID(CK_CLASSID child) {
		auto finder = g_CKClassInfoId2Idx.find(child);
		if (finder == g_CKClassInfoId2Idx.end()) std::abort();
		return g_CKClassInfo[finder->second].Parent;
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
		// todo: add class type registrations

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

		return CKERROR::CKERR_OK;
	}

	CKERROR CKShutdown() {
		// free class indo
		g_CKClassInfo.clear();
		g_CKClassInfoId2Idx.clear();
		g_CKClassInfoMaxID = static_cast<CK_CLASSID>(0);

		return CKERROR::CKERR_OK;
	}

#pragma endregion


}
