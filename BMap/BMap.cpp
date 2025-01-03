#include "BMap.hpp"

namespace BMap {

#pragma region BMfile

	BMFile::BMFile(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, NakedOutputCallback raw_callback, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING* encodings, bool is_loader) :
		m_IsInitError(false), m_IsLoader(is_loader), m_HasLoaded(false), m_HasSaved(false), m_Context(nullptr) {
		m_Context = new LibCmo::CK2::CKContext();
		// binding callback with lambda wrapper.
		// check whether callback is nullptr.
		m_IsInitError = m_IsInitError || (raw_callback == nullptr);
		if (raw_callback != nullptr) {
			m_Context->SetOutputCallback([raw_callback](LibCmo::CKSTRING strl) -> void {
				raw_callback(strl);
				});
		}

		// set temp folder and texture folder
		auto pm = m_Context->GetPathManager();
		m_IsInitError = m_IsInitError || !pm->AddPath(texture_folder);
		m_IsInitError = m_IsInitError || !pm->SetTempFolder(temp_folder);

		// set encoding
		LibCmo::XContainer::XArray<LibCmo::XContainer::XString> cache;
		for (LibCmo::CKDWORD i = 0; i < encoding_count; ++i) {
			if (encodings[i] != nullptr)
				cache.emplace_back(encodings[i]);
		}
		m_Context->SetEncoding(cache);
		m_IsInitError = m_IsInitError || !m_Context->IsValidEncoding();

		// set default texture save mode is external
		m_Context->SetGlobalImagesSaveOptions(LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_EXTERNAL);
		// set default file write mode is whole compressed
		m_Context->SetFileWriteMode(LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED);
	}

	BMFile::~BMFile() {
		delete m_Context;
	}

#pragma region Safe Check Function

	bool BMFile::IsInitError() {
		return m_IsInitError;
	}

	bool BMFile::CanExecLoad() {
		// no error, is loader, no prev load
		return (!m_IsInitError && m_IsLoader && !m_HasLoaded);
	}
	bool BMFile::CanExecSave() {
		// no error, is saver, no prev save
		return (!m_IsInitError && !m_IsLoader && !m_HasSaved);
	}
	bool BMFile::CanExecLoaderVisitor() {
		// no error, is loader, has loaded
		return (!m_IsInitError && m_IsLoader && m_HasLoaded);
	}
	bool BMFile::CanExecSaverVisitor() {
		// no error, is saver, not saveed yet
		// same as CanExecSave
		return (!m_IsInitError && !m_IsLoader && !m_HasSaved);
	}

#pragma endregion

#pragma region Help Function

	bool BMFile::Load(LibCmo::CKSTRING filename) {
		if (!CanExecLoad()) return false;

		// create temp ckfile and load
		LibCmo::CK2::CKFileReader reader(m_Context);
		LibCmo::CK2::CKERROR err = reader.DeepLoad(filename);

		// detect error
		if (err != LibCmo::CK2::CKERROR::CKERR_OK) {
			// failed. clear document and return false
			m_Context->ClearAll();
			return false;
		}

		// sync data list to our list
		m_ObjGroups.clear();
		m_Obj3dObjects.clear();
		m_ObjMeshes.clear();
		m_ObjMaterials.clear();
		m_ObjTextures.clear();
		m_ObjTargetLights.clear();
		for (const auto& fileobj : reader.GetFileObjects()) {
			if (fileobj.ObjPtr == nullptr) continue;

			switch (fileobj.ObjectCid) {
				case LibCmo::CK2::CK_CLASSID::CKCID_GROUP:
					m_ObjGroups.emplace_back(fileobj.CreatedObjectId);
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT:
					m_Obj3dObjects.emplace_back(fileobj.CreatedObjectId);
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_MESH:
					m_ObjMeshes.emplace_back(fileobj.CreatedObjectId);
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL:
					m_ObjMaterials.emplace_back(fileobj.CreatedObjectId);
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE:
					m_ObjTextures.emplace_back(fileobj.CreatedObjectId);
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_TARGETLIGHT:
					m_ObjTargetLights.emplace_back(fileobj.CreatedObjectId);
					break;
				default:
					break; // skip unknow objects
			}
		}

		m_HasLoaded = true;
		return true;
	}

	bool BMFile::Save(LibCmo::CKSTRING filename, LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS texture_save_opt, bool use_compress, LibCmo::CKINT compress_level) {
		if (!CanExecSave()) return false;

		// create temp writer
		LibCmo::CK2::CKFileWriter writer(m_Context);

		// fill object data
		for (const auto& id : m_ObjGroups) {
			writer.AddSavedObject(m_Context->GetObject(id));
		}
		for (const auto& id : m_Obj3dObjects) {
			writer.AddSavedObject(m_Context->GetObject(id));
		}
		for (const auto& id : m_ObjMeshes) {
			writer.AddSavedObject(m_Context->GetObject(id));
		}
		for (const auto& id : m_ObjMaterials) {
			writer.AddSavedObject(m_Context->GetObject(id));
		}
		for (const auto& id : m_ObjTextures) {
			writer.AddSavedObject(m_Context->GetObject(id));
		}
		for (const auto& id :m_ObjTargetLights) {
			writer.AddSavedObject(m_Context->GetObject(id));
		}

		// set global texture save mode
		m_Context->SetGlobalImagesSaveOptions(texture_save_opt);
		// set compress level
		if (use_compress) {
			m_Context->SetFileWriteMode(LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED);
			m_Context->SetCompressionLevel(compress_level);
		} else {
			m_Context->SetFileWriteMode(LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_UNCOMPRESSED);
		}

		// save to file and detect error
		LibCmo::CK2::CKERROR err = writer.Save(filename);

		// return with error detect.
		m_HasSaved = true;
		return err == LibCmo::CK2::CKERROR::CKERR_OK;
	}

	LibCmo::CK2::ObjImpls::CKObject* BMFile::GetObjectPtr(LibCmo::CK2::CK_ID objid) {
		// we fetch object from CKContext to get better performance
		LibCmo::CK2::ObjImpls::CKObject* obj = m_Context->GetObject(objid);

		// however, we can not directly return the pointer fetched fron CKContext.
		// BMFile only provide limited type visiting, we must make sure it provided ID also is existed in out stored list.
		// so we check its type here. if type is not matched, we reset it to nullptr.
		if (obj != nullptr) {
			switch (obj->GetClassID()) {
				case LibCmo::CK2::CK_CLASSID::CKCID_GROUP:
				case LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT:
				case LibCmo::CK2::CK_CLASSID::CKCID_MESH:
				case LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL:
				case LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE:
				case LibCmo::CK2::CK_CLASSID::CKCID_TARGETLIGHT:
					break; // okey. do nothing
				default:
					// this object should not be exposed to outside, reset it to nullptr
					obj = nullptr;
					break;
			}
		}

		// return result
		return obj;
	}

#pragma endregion

#pragma region Visitor

	LibCmo::CKDWORD BMFile::CommonGetObjectCount(std::vector<LibCmo::CK2::CK_ID>& container) {
		// only available in loader
		if (!CanExecLoaderVisitor()) return 0;
		return static_cast<LibCmo::CKDWORD>(container.size());
	}
	LibCmo::CK2::CK_ID BMFile::CommonGetObject(std::vector<LibCmo::CK2::CK_ID>& container, LibCmo::CKDWORD idx) {
		// only available in loader
		if (!CanExecLoaderVisitor()) return 0;
		return container[idx];
	}
	LibCmo::CK2::CK_ID BMFile::CommonCreateObject(std::vector<LibCmo::CK2::CK_ID>& container, LibCmo::CK2::CK_CLASSID cid) {
		// only available in saver
		if (!CanExecSaverVisitor()) return 0;

		// try create object and get its pointer
		LibCmo::CK2::ObjImpls::CKObject* obj = m_Context->CreateObject(cid, nullptr);
		// check creation validation
		if (obj == nullptr) return 0;

		// if success, write its id and emplace its id into list
		LibCmo::CK2::CK_ID objid = obj->GetID();
		container.emplace_back(objid);
		return objid;
	}

	LibCmo::CKDWORD BMFile::GetGroupCount() { return CommonGetObjectCount(m_ObjGroups); }
	LibCmo::CK2::CK_ID BMFile::GetGroup(LibCmo::CKDWORD idx) { return CommonGetObject(m_ObjGroups, idx); }
	LibCmo::CK2::CK_ID BMFile::CreateGroup() { return CommonCreateObject(m_ObjGroups, LibCmo::CK2::CK_CLASSID::CKCID_GROUP); }
	LibCmo::CKDWORD BMFile::Get3dObjectCount() { return CommonGetObjectCount(m_Obj3dObjects); }
	LibCmo::CK2::CK_ID BMFile::Get3dObject(LibCmo::CKDWORD idx) { return CommonGetObject(m_Obj3dObjects, idx); }
	LibCmo::CK2::CK_ID BMFile::Create3dObject() { return CommonCreateObject(m_Obj3dObjects, LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT); }
	LibCmo::CKDWORD BMFile::GetMeshCount() { return CommonGetObjectCount(m_ObjMeshes); }
	LibCmo::CK2::CK_ID BMFile::GetMesh(LibCmo::CKDWORD idx) { return CommonGetObject(m_ObjMeshes, idx); }
	LibCmo::CK2::CK_ID BMFile::CreateMesh() { return CommonCreateObject(m_ObjMeshes, LibCmo::CK2::CK_CLASSID::CKCID_MESH); }
	LibCmo::CKDWORD BMFile::GetMaterialCount() { return CommonGetObjectCount(m_ObjMaterials); }
	LibCmo::CK2::CK_ID BMFile::GetMaterial(LibCmo::CKDWORD idx) { return CommonGetObject(m_ObjMaterials, idx); }
	LibCmo::CK2::CK_ID BMFile::CreateMaterial() { return CommonCreateObject(m_ObjMaterials, LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL); }
	LibCmo::CKDWORD BMFile::GetTextureCount() { return CommonGetObjectCount(m_ObjTextures); }
	LibCmo::CK2::CK_ID BMFile::GetTexture(LibCmo::CKDWORD idx) { return CommonGetObject(m_ObjTextures, idx); }
	LibCmo::CK2::CK_ID BMFile::CreateTexture() { return CommonCreateObject(m_ObjTextures, LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE); }
	LibCmo::CKDWORD BMFile::GetTargetLightCount() { return CommonGetObjectCount(m_ObjTargetLights); }
	LibCmo::CK2::CK_ID BMFile::GetTargetLight(LibCmo::CKDWORD idx) { return CommonGetObject(m_ObjTargetLights, idx); }
	LibCmo::CK2::CK_ID BMFile::CreateTargetLight() { return CommonCreateObject(m_ObjTargetLights, LibCmo::CK2::CK_CLASSID::CKCID_TARGETLIGHT); }

#pragma endregion

#pragma endregion

#pragma region BMMeshTransition

	BMMeshTransition::TransitionVertex::TransitionVertex(
		const LibCmo::VxMath::VxVector3& vec,
		const LibCmo::VxMath::VxVector3& norm,
		const LibCmo::VxMath::VxVector2& uv) :
		m_Vertex(vec), m_Norm(norm), m_UV(uv) {}

	BMMeshTransition::TransitionFace::TransitionFace(LibCmo::CKDWORD _i1, LibCmo::CKDWORD _i2, LibCmo::CKDWORD _i3, LibCmo::CKDWORD mtl_id) :
		m_Idx1(_i1), m_Idx2(_i2), m_Idx3(_i3), m_MtlSlotIdx(mtl_id) {}

	bool BMMeshTransition::TransitionVertexCompare::operator()(const TransitionVertex& lhs, const TransitionVertex& rhs) const {
		if (auto cmp = std::memcmp(&lhs.m_Vertex, &rhs.m_Vertex, sizeof(LibCmo::VxMath::VxVector3)); cmp != 0) return cmp < 0;
		if (auto cmp = std::memcmp(&lhs.m_Norm, &rhs.m_Norm, sizeof(LibCmo::VxMath::VxVector3)); cmp != 0) return cmp < 0;
		return std::memcmp(&lhs.m_UV, &rhs.m_UV, sizeof(LibCmo::VxMath::VxVector2)) < 0;
	}

	BMMeshTransition::BMMeshTransition() :
		m_IsVertexOK(false), m_IsNormalOK(false), m_IsUVOK(false), m_IsFaceOK(false), m_IsMtlSlotOK(false),
		m_IsParsed(false),
		m_Vertexs(), m_Normals(), m_UVs(),
		m_MtlSlots(),
		m_FaceVertexs(), m_FaceNormals(), m_FaceUVs(), m_FaceMtlSlotIdxs(),
		m_ProcVertexs(), m_ProcFaces(), m_ProcDupRemover() {}

	BMMeshTransition::~BMMeshTransition() {}

	bool BMMeshTransition::PrepareVertexCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return false;
		m_Vertexs.resize(count);
		m_IsVertexOK = true;
		return true;
	}

	LibCmo::VxMath::VxVector3* BMMeshTransition::PrepareVertex() {
		if (m_IsParsed || !m_IsVertexOK) return nullptr;
		return m_Vertexs.data();
	}

	bool BMMeshTransition::PrepareNormalCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return false;
		m_Normals.resize(count);
		m_IsNormalOK = true;
		return true;
	}

	LibCmo::VxMath::VxVector3* BMMeshTransition::PrepareNormal() {
		if (m_IsParsed || !m_IsNormalOK) return nullptr;
		return m_Normals.data();
	}

	bool BMMeshTransition::PrepareUVCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return false;
		m_UVs.resize(count);
		m_IsUVOK = true;
		return true;
	}

	LibCmo::VxMath::VxVector2* BMMeshTransition::PrepareUV() {
		if (m_IsParsed || !m_IsUVOK) return nullptr;
		return m_UVs.data();
	}

	bool BMMeshTransition::PrepareMtlSlotCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return false;
		m_MtlSlots.resize(count, 0);
		m_IsMtlSlotOK = true;
		return true;
	}

	LibCmo::CK2::CK_ID* BMMeshTransition::PrepareMtlSlot() {
		if (m_IsParsed || !m_IsMtlSlotOK) return nullptr;
		return m_MtlSlots.data();
	}

	bool BMMeshTransition::PrepareFaceCount(LibCmo::CKDWORD count) {
		if (m_IsParsed) return false;
		m_FaceVertexs.resize(count * 3);
		m_FaceNormals.resize(count * 3);
		m_FaceUVs.resize(count * 3);
		m_FaceMtlSlotIdxs.resize(count);
		m_IsFaceOK = true;
		return true;
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceVertexIndices() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceVertexs.data();
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceNormalIndices() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceNormals.data();
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceUVIndices() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceUVs.data();
	}

	LibCmo::CKDWORD* BMMeshTransition::PrepareFaceMtlSlot() {
		if (m_IsParsed || !m_IsFaceOK) return nullptr;
		return m_FaceMtlSlotIdxs.data();
	}

	bool BMMeshTransition::Parse(BMFile* bmfile, LibCmo::CK2::CK_ID mesh_id) {
		// check basic status
		if (m_IsParsed || bmfile == nullptr) return false;
		if (!m_IsVertexOK || !m_IsNormalOK || !m_IsUVOK || !m_IsFaceOK || !m_IsMtlSlotOK) return false;
		// check pointer assign
		LibCmo::CK2::ObjImpls::CKObject* writing_mesh = bmfile->GetObjectPtr(mesh_id);
		if (writing_mesh == nullptr || writing_mesh->GetClassID() != LibCmo::CK2::CK_CLASSID::CKCID_MESH) return false;

		// do parse
		m_IsParsed = true;
		DoRealParse();

		// check vertex overflow
		if (m_ProcVertexs.size() > std::numeric_limits<LibCmo::CKWORD>::max()) {
			return false;
		}
		// check mtl slot overflow
		if (m_MtlSlots.size() > std::numeric_limits<LibCmo::CKWORD>::max()) {
			return false;
		}

		// apply to mesh
		ApplyToMesh(bmfile, static_cast<LibCmo::CK2::ObjImpls::CKMesh*>(writing_mesh));

		return true;
	}

	void BMMeshTransition::DoRealParse() {
		// reserve vector to prevent extra mem alloc
		// use the most bad situation to reserve
		size_t face_size = m_FaceMtlSlotIdxs.size();
		m_ProcVertexs.reserve(face_size * 3);
		m_ProcFaces.reserve(face_size);

		// iterate face
		for (size_t faceid = 0; faceid < face_size; ++faceid) {
			LibCmo::CKDWORD idx[3] { 0, 0, 0 };
			for (int j = 0; j < 3; ++j) {
				// create one first
				TransitionVertex tvec(
					m_Vertexs[m_FaceVertexs[faceid * 3 + j]],
					m_Normals[m_FaceNormals[faceid * 3 + j]],
					m_UVs[m_FaceUVs[faceid * 3 + j]]
				);

				// try insert it
				auto insert_result = m_ProcDupRemover.try_emplace(tvec, static_cast<LibCmo::CKDWORD>(m_ProcVertexs.size()));
				// get the new inserted index or existed index.
				idx[j] = insert_result.first->second;
				// if insert successfully, append to proc vertexs
				if (insert_result.second) {
					m_ProcVertexs.emplace_back(tvec);
				}
			}

			// create face
			m_ProcFaces.emplace_back(idx[0], idx[1], idx[2], m_FaceMtlSlotIdxs[faceid]);
		}
	}

	void BMMeshTransition::ApplyToMesh(BMFile* bmfile, LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh) {
		LibCmo::CKDWORD vec_count = static_cast<LibCmo::CKDWORD>(m_ProcVertexs.size()),
			face_count = static_cast<LibCmo::CKDWORD>(m_ProcFaces.size()),
			mtl_count = static_cast<LibCmo::CKDWORD>(m_MtlSlots.size());
		write_into_mesh->CleanMesh();

		// write vertex
		write_into_mesh->SetVertexCount(vec_count);
		LibCmo::VxMath::VxCopyStructure(
			vec_count,
			write_into_mesh->GetVertexPositions(),
			CKSizeof(LibCmo::VxMath::VxVector3),
			CKSizeof(LibCmo::VxMath::VxVector3),
			&m_ProcVertexs.data()->m_Vertex,
			CKSizeof(TransitionVertex)
		);
		LibCmo::VxMath::VxCopyStructure(
			vec_count,
			write_into_mesh->GetVertexNormals(),
			CKSizeof(LibCmo::VxMath::VxVector3),
			CKSizeof(LibCmo::VxMath::VxVector3),
			&m_ProcVertexs.data()->m_Norm,
			CKSizeof(TransitionVertex)
		);
		LibCmo::VxMath::VxCopyStructure(
			vec_count,
			write_into_mesh->GetVertexUVs(),
			CKSizeof(LibCmo::VxMath::VxVector2),
			CKSizeof(LibCmo::VxMath::VxVector2),
			&m_ProcVertexs.data()->m_UV,
			CKSizeof(TransitionVertex)
		);

		// write face
		write_into_mesh->SetFaceCount(face_count);
		auto pIndices = write_into_mesh->GetFaceIndices();
		auto pMtlIdx = write_into_mesh->GetFaceMaterialSlotIndexs();
		for (LibCmo::CKDWORD i = 0; i < face_count; ++i) {
			*(pIndices++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_Idx1);
			*(pIndices++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_Idx2);
			*(pIndices++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_Idx3);

			*(pMtlIdx++) = static_cast<LibCmo::CKWORD>(m_ProcFaces[i].m_MtlSlotIdx);
		}

		// set mtl slot
		write_into_mesh->SetMaterialSlotCount(mtl_count);
		LibCmo::CK2::ObjImpls::CKMaterial** pMtlSlot = write_into_mesh->GetMaterialSlots();
		for (LibCmo::CKDWORD i = 0; i < mtl_count; ++i) {
			// convert id to CKMaterial* and check its type
			LibCmo::CK2::ObjImpls::CKObject* mtlptr = bmfile->GetObjectPtr(m_MtlSlots[i]);
			if (mtlptr != nullptr && mtlptr->GetClassID() == LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL) {
				*(pMtlSlot++) = static_cast<LibCmo::CK2::ObjImpls::CKMaterial*>(mtlptr);
			} else {
				*(pMtlSlot++) = nullptr;
			}
		}
	}


#pragma endregion

}
