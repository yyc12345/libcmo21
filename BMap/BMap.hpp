#pragma once

#include <VTUserAll.hpp>
#include <vector>
#include <cstdint>
#include <cinttypes>
#include <type_traits>

namespace BMap {
	
	class BMFile {
	public:
		BMFile(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING* encodings, bool is_reader);
		~BMFile();
		LIBCMO_DISABLE_COPY_MOVE(BMFile);

		// ===== safe visit functions ===== 

		/**
		Safe Visit Function will make sure this class is visited with safe mode.
		These function will block all other functions if this class init failed.
		Or, block any more operations if this class has loaded or saved once. In this time you only can free this class
		*/
		
	public:
		bool IsInitError() {
			return m_IsInitError;
		}

	private:
		bool CanExecLoad() {
			// no error, is loader, no prev load
			return (!m_IsInitError && m_IsLoader && !m_HasLoaded);
		}
		bool CanExecSave() {
			// no error, is saver, no prev save
			return (!m_IsInitError && !m_IsLoader && !m_HasSaved);
		}
		bool CanExecLoaderVisitor() {
			// no error, is loader, has loaded
			return (!m_IsInitError && m_IsLoader && m_HasLoaded);
		}
		bool CanExecSaverVisitor() {
			// no error, is saver, not saveed yet
			// same as CanExecSave
			return (!m_IsInitError && !m_IsLoader && !m_HasSaved);
		}

	private:
		/**
		 * @brief True if an error occurs when initializing this class.
		*/
		bool m_IsInitError;
		/**
		 * @brief True if this class is a reader.
		*/
		bool m_IsLoader;
		/**
		 * @brief True if this class has read. Only valid when this class is reader.
		*/
		bool m_HasLoaded;
		/**
		 * @brief True if this class has written. Only valid when this class is writer.
		*/
		bool m_HasSaved;

		// ===== help functions ===== 

	public:
		bool Load(LibCmo::CKSTRING filename);
		bool Save(LibCmo::CKSTRING filename, LibCmo::CKINT compress_level);

		LibCmo::CK2::ObjImpls::CKObject* GetObjectPtr(LibCmo::CK2::CK_ID objid) {
			return m_Context->GetObject(objid);;
		}

		// ===== visitors ===== 

	private:
		LibCmo::CKDWORD CommonGetObjectCount(std::vector<LibCmo::CK2::CK_ID>& container) {
			// only available in loader
			if (!CanExecLoaderVisitor()) return 0;
			return static_cast<LibCmo::CKDWORD>(container.size());
		}
		LibCmo::CK2::CK_ID CommonGetObject(std::vector<LibCmo::CK2::CK_ID>& container, LibCmo::CKDWORD idx) {
			// only available in loader
			if (!CanExecLoaderVisitor()) return 0;
			return container[idx];
		}
		LibCmo::CK2::CK_ID CommonCreateObject(std::vector<LibCmo::CK2::CK_ID>& container, LibCmo::CK2::CK_CLASSID cid) {
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
	public:
		LibCmo::CKDWORD GetGroupCount();
		LibCmo::CK2::CK_ID GetGroup(LibCmo::CKDWORD idx);
		LibCmo::CK2::CK_ID CreateGroup();
		LibCmo::CKDWORD Get3dObjectCount();
		LibCmo::CK2::CK_ID Get3dObject(LibCmo::CKDWORD idx);
		LibCmo::CK2::CK_ID Create3dObject();
		LibCmo::CKDWORD GetMeshCount();
		LibCmo::CK2::CK_ID GetMesh(LibCmo::CKDWORD idx);
		LibCmo::CK2::CK_ID CreateMesh();
		LibCmo::CKDWORD GetMaterialCount();
		LibCmo::CK2::CK_ID GetMaterial(LibCmo::CKDWORD idx);
		LibCmo::CK2::CK_ID CreateMaterial();
		LibCmo::CKDWORD GetTextureCount();
		LibCmo::CK2::CK_ID GetTexture(LibCmo::CKDWORD idx);
		LibCmo::CK2::CK_ID CreateTexture();

	private:
		LibCmo::CK2::CKContext* m_Context;

		std::vector<LibCmo::CK2::CK_ID> m_ObjGroups;
		std::vector<LibCmo::CK2::CK_ID> m_Obj3dObjects;
		std::vector<LibCmo::CK2::CK_ID> m_ObjMeshs;
		std::vector<LibCmo::CK2::CK_ID> m_ObjMaterials;
		std::vector<LibCmo::CK2::CK_ID> m_ObjTextures;

	};
	
	class BMMeshTransition {
	private:
		struct TransitionVertex {
			TransitionVertex(
				const LibCmo::VxMath::VxVector3& vec,
				const LibCmo::VxMath::VxVector3& norm,
				const LibCmo::VxMath::VxVector2& uv);
			LibCmo::VxMath::VxVector3 m_Vertex;
			LibCmo::VxMath::VxVector3 m_Norm;
			LibCmo::VxMath::VxVector2 m_UV;
		};
		struct TransitionFace {
			TransitionFace(LibCmo::CKDWORD _i1, LibCmo::CKDWORD _i2, LibCmo::CKDWORD _i3, LibCmo::CKDWORD mtl_id);
			LibCmo::CKDWORD m_Idx1, m_Idx2, m_Idx3;
			LibCmo::CKDWORD m_MtlSlotIdx;
		};
		struct TransitionVertexCompare {
			bool operator()(const TransitionVertex& lhs, const TransitionVertex& rhs) const;
		};

	public:
		BMMeshTransition();
		~BMMeshTransition();
		LIBCMO_DISABLE_COPY_MOVE(BMMeshTransition);

		bool PrepareVertexCount(LibCmo::CKDWORD count);
		LibCmo::VxMath::VxVector3* PrepareVertex();
		bool PrepareNormalCount(LibCmo::CKDWORD count);
		LibCmo::VxMath::VxVector3* PrepareNormal();
		bool PrepareUVCount(LibCmo::CKDWORD count);
		LibCmo::VxMath::VxVector2* PrepareUV();
		bool PrepareMtlSlotCount(LibCmo::CKDWORD count);
		LibCmo::CK2::CK_ID* PrepareMtlSlot();
		bool PrepareFaceCount(LibCmo::CKDWORD count);
		LibCmo::CKDWORD* PrepareFaceVertexIndices();
		LibCmo::CKDWORD* PrepareFaceNormalIndices();
		LibCmo::CKDWORD* PrepareFaceUVIndices();
		LibCmo::CKDWORD* PrepareFaceMtlSlot();

		bool Parse(BMFile* bmfile, LibCmo::CK2::CK_ID mesh_id);

	private:
		void DoRealParse();
		void ApplyToMesh(BMFile* bmfile, LibCmo::CK2::ObjImpls::CKMesh* write_into_mesh);

		bool m_IsVertexOK, m_IsNormalOK, m_IsUVOK, m_IsFaceOK, m_IsMtlSlotOK;
		bool m_IsParsed;

		std::vector<LibCmo::VxMath::VxVector3> m_Vertexs, m_Normals;
		std::vector<LibCmo::VxMath::VxVector2> m_UVs;
		std::vector<LibCmo::CKDWORD> m_FaceVertexs, m_FaceNormals, m_FaceUVs;
		std::vector<LibCmo::CKDWORD> m_FaceMtlSlotIdxs;
		std::vector<LibCmo::CK2::CK_ID> m_MtlSlots;

		std::vector<TransitionVertex> m_ProcVertexs;
		std::vector<TransitionFace> m_ProcFaces;
		/**
		@brief The core duplication vertex remover.
		@remark
		unordered_map have performance problem when dealing with massive data (in this case, big mesh).
		so we use map to get stable time cost.
		*/
		std::map<TransitionVertex, LibCmo::CKDWORD, TransitionVertexCompare> m_ProcDupRemover;
	};

}
