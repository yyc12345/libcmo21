#pragma once

#include "../../VTAll.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKMesh : public CKBeObject {
	public:
		CKMesh(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKMesh();
		LIBCMO_DISABLE_COPY_MOVE(CKMesh);
		
		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_MESH; 
		}
		
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

		// it only have special Show method
		virtual void Show(CK_OBJECT_SHOWOPTION show = CK_OBJECT_SHOWOPTION::CKSHOW) override;

		// ===== Misc Section =====
	public:
		void CleanMesh();
	protected:
		void BuildNormals();
		void BuildFaceNormals();

		// ===== Line Section =====
	public:
		CKDWORD GetVertexCount();
		void SetVertexCount(CKDWORD count);
		VxMath::VxVector3* GetVertexPositions();
		VxMath::VxVector3* GetVertexNormals();
		VxMath::VxVector2* GetVertexUVs();
		CKDWORD* GetVertexColors();
		CKDWORD* GetVertexSpecularColors();
		CKFLOAT* GetVertexWeights();
	
		// ===== Material Slot Section =====
	public:
		CKDWORD GetMaterialSlotCount();
		void SetMaterialSlotCount(CKDWORD count);
		void SetMaterialSlot(CKMaterial* mtl, CKDWORD idx);

		// ===== Face Section =====
	public:
		CKDWORD GetFaceCount();
		void SetFaceCount(CKDWORD count);
		CKWORD* GetFaceIndices();
		CKWORD* GetFaceMaterialSlotIndexs();
		VxMath::VxVector3* GetFaceNormals(CKDWORD& stride);
		CKWORD* GetFaceChannelMasks(CKDWORD& stride);

		// ===== Line Section =====
	public:
		CKDWORD GetLineCount();
		void SetLineCount(CKDWORD count);
		CKWORD* GetLineIndices();
		
		// ===== Material Channel Section =====
	public:
		CKDWORD GetMtlChannelCount();
		void SetMtlChannelCount(CKDWORD count);
		CKMaterial** GetMtlChannelMaterials(CKDWORD& stride);
		VxMath::VXBLEND_MODE* GetMtlChannelSourceBlends(CKDWORD& stride);
		VxMath::VXBLEND_MODE* GetMtlChannelDestBlends(CKDWORD& stride);

		VxMath::VxVector2* GetMtlChannelCustomUVs(CKDWORD idx);
		VxMath::VXCHANNEL_FLAGS GetMtlChannelFlags(CKDWORD idx);
		void SetMtlChannelFlags(CKDWORD idx, VxMath::VXCHANNEL_FLAGS flags);
	protected:
		// 2 sync functions served for material channels.
		void SyncVertexCountToMtlChannel();	// setup material channel custom uv properly
		void SyncMtlChannelToFaceMask();	// request all face accept all material channels.

	protected:
		enum class VertexSaveFlags : CKDWORD {
			None = 0,
			SingleColor = 0x1u,	/**< if not set, the VertexColor is a list, otherwise a single global CKDWORD.*/
			SingleSpecularColor = 0x2u,	/**< if not set, the VertexSpecularColor is a list, otherwise a single global CKDWORD. */
			NoNormal = 0x4u,	/**< if set, there are no normal data for vertex. */
			SingleUV = 0x8u,	/**< if not set, the VertexUV is a list, otherwise a single global VxVertex2. */
			NoPos = 0x10u,	/**< if set, there are no position data for vertex. */
		};
		struct FaceData_t {
			FaceData_t() :
				m_Normal(),
				m_ChannelMask(0xFFFF)
			{}
			VxMath::VxVector3 m_Normal;
			CKWORD m_ChannelMask;
		};
		struct MaterialChannel_t {
			MaterialChannel_t() : 
				m_Material(nullptr),
				m_SourceBlend(VxMath::VXBLEND_MODE::VXBLEND_ZERO),
				m_DestBlend(VxMath::VXBLEND_MODE::VXBLEND_SRCCOLOR),
				m_CustomUV(),
				m_Flags(EnumsHelper::Merge({ VxMath::VXCHANNEL_FLAGS::VXCHANNEL_ACTIVE, VxMath::VXCHANNEL_FLAGS::VXCHANNEL_SAMEUV }))
			{}
			CKMaterial* m_Material;
			VxMath::VXBLEND_MODE m_SourceBlend;
			VxMath::VXBLEND_MODE m_DestBlend;
			XContainer::XArray<VxMath::VxVector2> m_CustomUV;
			VxMath::VXCHANNEL_FLAGS m_Flags;
		};

		VxMath::VXMESH_FLAGS m_Flags;
		CKDWORD m_VertexCount;
		CKDWORD m_LineCount;
		CKDWORD m_MtlSlotCount;
		CKDWORD m_FaceCount;
		CKDWORD m_MtlChannelCount;

		XContainer::XArray<VxMath::VxVector3> m_VertexPosition;
		XContainer::XArray<VxMath::VxVector3> m_VertexNormal;
		XContainer::XArray<VxMath::VxVector2> m_VertexUV;
		XContainer::XArray<CKDWORD> m_VertexColor;
		XContainer::XArray<CKDWORD> m_VertexSpecularColor;
		XContainer::XArray<CKFLOAT> m_VertexWeight;

		XContainer::XArray<CKMaterial*> m_MaterialSlot;

		XContainer::XArray<CKWORD> m_FaceIndices;
		XContainer::XArray<CKWORD> m_FaceMtlIndex;
		XContainer::XArray<FaceData_t> m_Faces;

		XContainer::XArray<CKWORD> m_LineIndices;

		XContainer::XArray<MaterialChannel_t> m_MaterialChannels;
	};

}
