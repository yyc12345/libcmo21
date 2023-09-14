#pragma once

#include "../../VTAll.hpp"
#include "../CKBitmapData.hpp"
#include "CKBeObject.hpp"
#include <array>

namespace LibCmo::CK2::ObjImpls {

	class CKMaterial : public CKBeObject {
	public:
		CKMaterial(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKMaterial();
		LIBCMO_DISABLE_COPY_MOVE(CKMaterial);
		
		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_MATERIAL; 
		}
		
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

	protected:
		VxMath::VxColor m_Diffuse;
		VxMath::VxColor m_Ambient;
		VxMath::VxColor m_Specular;
		VxMath::VxColor m_Emissive;
		float m_SpecularPower;

		std::array<CKTexture*, 4> m_Textures;
		CKDWORD m_TextureBorderColor;

		VxMath::VXTEXTURE_BLENDMODE m_TextureBlendMode;
		VxMath::VXTEXTURE_FILTERMODE m_TextureMinMode;
		VxMath::VXTEXTURE_FILTERMODE m_TextureMagMode;
		VxMath::VXTEXTURE_ADDRESSMODE m_TextureAddreddMode;

		VxMath::VXBLEND_MODE m_SourceBlend;
		VxMath::VXBLEND_MODE m_DestBlend;
		VxMath::VXFILL_MODE m_FillMode;
		VxMath::VXSHADE_MODE m_ShadeMode;

		bool m_EnableZWrite;
		VxMath::VXCMPFUNC m_ZFunc;

		bool m_EnableTwoSided;

		bool m_EnableAlphaTest;
		VxMath::VXCMPFUNC m_AlphaTestFunc;
		CKBYTE m_AlphaRef;

		VxMath::VX_EFFECT m_Effect;
	};


}
