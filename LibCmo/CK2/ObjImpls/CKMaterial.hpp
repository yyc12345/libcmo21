#pragma once

#include "../../VTInternal.hpp"
#include "CKBeObject.hpp"
#include <array>

namespace LibCmo::CK2::ObjImpls {

	class CKMaterial : public CKBeObject {
	public:
		CKMaterial(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKMaterial();
		YYCC_DEL_CLS_COPY_MOVE(CKMaterial);

		virtual CK_CLASSID GetClassID(void) override {
			return CK_CLASSID::CKCID_MATERIAL;
		}

		virtual void CheckPreDeletion() override;

		// 2 RW functions
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;

		const VxMath::VxColor& GetDiffuse() const;
		void SetDiffuse(const VxMath::VxColor& col);
		const VxMath::VxColor& GetAmbient() const;
		void SetAmbient(const VxMath::VxColor& col);
		const VxMath::VxColor& GetSpecular() const;
		void SetSpecular(const VxMath::VxColor& col);
		const VxMath::VxColor& GetEmissive() const;
		void SetEmissive(const VxMath::VxColor& col);
		CKFLOAT GetSpecularPower() const;
		void SetSpecularPower(CKFLOAT val);

		CKTexture* GetTexture(CKDWORD idx = 0) const;
		void SetTexture(CKTexture* tex, CKDWORD idx = 0);
		CKDWORD GetTextureBorderColor() const;
		void SetTextureBorderColor(CKDWORD val);

		VxMath::VXTEXTURE_BLENDMODE GetTextureBlendMode() const;
		void SetTextureBlendMode(VxMath::VXTEXTURE_BLENDMODE val);
		VxMath::VXTEXTURE_FILTERMODE GetTextureMinMode() const;
		void SetTextureMinMode(VxMath::VXTEXTURE_FILTERMODE val);
		VxMath::VXTEXTURE_FILTERMODE GetTextureMagMode() const;
		void SetTextureMagMode(VxMath::VXTEXTURE_FILTERMODE val);
		VxMath::VXTEXTURE_ADDRESSMODE GetTextureAddressMode() const;
		void SetTextureAddressMode(VxMath::VXTEXTURE_ADDRESSMODE val);

		VxMath::VXBLEND_MODE GetSourceBlend() const;
		void SetSourceBlend(VxMath::VXBLEND_MODE val);
		VxMath::VXBLEND_MODE GetDestBlend() const;
		void SetDestBlend(VxMath::VXBLEND_MODE val);
		VxMath::VXFILL_MODE GetFillMode() const;
		void SetFillMode(VxMath::VXFILL_MODE val);
		VxMath::VXSHADE_MODE GetShadeMode() const;
		void SetShadeMode(VxMath::VXSHADE_MODE val);

		bool GetAlphaTestEnabled() const;
		void SetAlphaTestEnabled(bool enabled);
		bool GetAlphaBlendEnabled() const;
		void SetAlphaBlendEnabled(bool enabled);
		bool GetPerspectiveCorrectionEnabled() const;
		void SetPerspectiveCorrectionEnabled(bool enabled);
		bool GetZWriteEnabled() const;
		void SetZWriteEnabled(bool enabled);
		bool GetTwoSidedEnabled() const;
		void SetTwoSidedEnabled(bool enabled);

		CKBYTE GetAlphaRef() const;
		void SetAlphaRef(CKBYTE val);
		VxMath::VXCMPFUNC GetAlphaFunc() const;
		void SetAlphaFunc(VxMath::VXCMPFUNC val);
		VxMath::VXCMPFUNC GetZFunc() const;
		void SetZFunc(VxMath::VXCMPFUNC val);
		VxMath::VX_EFFECT GetEffect() const;
		void SetEffect(VxMath::VX_EFFECT val);

	protected:
		VxMath::VxColor m_Diffuse;
		VxMath::VxColor m_Ambient;
		VxMath::VxColor m_Specular;
		VxMath::VxColor m_Emissive;
		CKFLOAT m_SpecularPower;

		std::array<CKTexture*, 4> m_Textures;
		CKDWORD m_TextureBorderColor;

		VxMath::VXTEXTURE_BLENDMODE m_TextureBlendMode;
		VxMath::VXTEXTURE_FILTERMODE m_TextureMinMode;
		VxMath::VXTEXTURE_FILTERMODE m_TextureMagMode;
		VxMath::VXTEXTURE_ADDRESSMODE m_TextureAddressMode;

		VxMath::VXBLEND_MODE m_SourceBlend;
		VxMath::VXBLEND_MODE m_DestBlend;
		VxMath::VXFILL_MODE m_FillMode;
		VxMath::VXSHADE_MODE m_ShadeMode;

		bool m_EnableAlphaTest;
		bool m_EnableAlphaBlend;
		bool m_EnablePerspectiveCorrection;
		bool m_EnableZWrite;
		bool m_EnableTwoSided;

		CKBYTE m_AlphaRef;
		VxMath::VXCMPFUNC m_AlphaFunc;
		VxMath::VXCMPFUNC m_ZFunc;
		VxMath::VX_EFFECT m_Effect;
	};


}
