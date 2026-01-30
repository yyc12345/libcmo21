#pragma once

#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <vector>
#include <cstdint>
#include <cinttypes>
#include <type_traits>

namespace BMap {
	
	using NakedOutputCallback = void(*)(LibCmo::CKSTRING);

	class BMFile {
	public:
		BMFile(LibCmo::CKSTRING temp_folder, LibCmo::CKSTRING texture_folder, NakedOutputCallback raw_callback, LibCmo::CKDWORD encoding_count, LibCmo::CKSTRING* encodings, bool is_reader);
		~BMFile();
		YYCC_DELETE_COPY_MOVE(BMFile)

		// ===== Safe Check Function ===== 

		/*
		Safe Check Function will make sure this class is visited in safe mode.
		Some of them are exposed to outside to report current status of this class, for example, whether there is a issue when initialize this class.
		And some of them are used by internal functions to make sure there is a safe environment to execute corresponding functions.
		For example, #Load function will use #CanExecLoad to detect whether it can execute loading process.
		*/
		
	public:
		/**
		 * @brief Check whether there is an error when initializing this class.
		 * @details
		 * This class is exposed for outside code to check.
		 * Internal code should use one of following 4 private check functions to check environment.
		 * @return True if there is an error when initializing this class.
		*/
		bool IsInitError();

	private:
		/**
		 * @brief Check whether it's okey to execute #Load function.
		 * @return True if it is okey.
		*/
		bool CanExecLoad();
		/**
		 * @brief Check whether it's okey to execute #Save function.
		 * @return True if it is okey.
		*/
		bool CanExecSave();
		/**
		 * @brief Check whether it's okey to execute Loader-related function.
		 * @details
		 * Due to implementation, saving file and loading file are use the same class, BMFile to represent.
		 * So obviously you can visit loader-related function in a saver.
		 * This operation is illegal. So we need block these operation.
		 * This is what this function does. Provide the condition which raise blocking.
		 * @return True if it is okey.
		*/
		bool CanExecLoaderVisitor();
		/**
		 * @brief Check whether it's okey to execute Saver-related function.
		 * @return True if it is okey.
		 * @see CanExecLoaderVisitor
		*/
		bool CanExecSaverVisitor();

	private:
		bool m_IsInitError; /**< True if an error occurs when initializing this class. */
		bool m_IsLoader; /**< True if this class is a reader. */
		bool m_HasLoaded; /**< True if this class has read. It's undefined behavior when visiting this variable if this class is not reader. */
		bool m_HasSaved; /**< True if this class has written. It's undefined behavior when visiting this variable if this class is not writer. */

		// ===== Help Function ===== 

	public:
		/**
		 * @brief Load document to this class.
		 * @param[in] filename The path to file.
		 * @return True if no error, otherwise false.
		*/
		bool Load(LibCmo::CKSTRING filename);
		/**
		 * @brief Save current class into document.
		 * @param[in] filename The path to file.
		 * @param[in] texture_save_opt Global texture save option
		 * @param[in] use_compress True if use compression when saving.
		 * @param[in] compress_level The compress level if you choose using compression in file.
		 * @return 
		*/
		bool Save(LibCmo::CKSTRING filename, LibCmo::CK2::CK_TEXTURE_SAVEOPTIONS texture_save_opt, bool use_compress, LibCmo::CKINT compress_level);

		/**
		 * @brief Get object pointer from given ID.
		 * @details
		 * This function is specially exposed to outside for detecting whether given ID is valid in BMFile.
		 * Also used by BMMeshTransition to get essential objects.
		 * @param[in] objid The ID of object.
		 * @return The pointer to given ID represented object. nullptr if not found.
		*/
		LibCmo::CK2::ObjImpls::CKObject* GetObjectPtr(LibCmo::CK2::CK_ID objid);

		// ===== Visitor ===== 

	private:
		LibCmo::CKDWORD CommonGetObjectCount(std::vector<LibCmo::CK2::CK_ID>& container);
		LibCmo::CK2::CK_ID CommonGetObject(std::vector<LibCmo::CK2::CK_ID>& container, LibCmo::CKDWORD idx);
		LibCmo::CK2::CK_ID CommonCreateObject(std::vector<LibCmo::CK2::CK_ID>& container, LibCmo::CK2::CK_CLASSID cid);

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
		LibCmo::CKDWORD GetTargetLightCount();
		LibCmo::CK2::CK_ID GetTargetLight(LibCmo::CKDWORD idx);
		LibCmo::CK2::CK_ID CreateTargetLight();

	private:
		LibCmo::CK2::CKContext* m_Context;

		std::vector<LibCmo::CK2::CK_ID> m_ObjGroups;
		std::vector<LibCmo::CK2::CK_ID> m_Obj3dObjects;
		std::vector<LibCmo::CK2::CK_ID> m_ObjMeshes;
		std::vector<LibCmo::CK2::CK_ID> m_ObjMaterials;
		std::vector<LibCmo::CK2::CK_ID> m_ObjTextures;
		std::vector<LibCmo::CK2::CK_ID> m_ObjTargetLights;

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
		YYCC_DELETE_COPY_MOVE(BMMeshTransition)

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
