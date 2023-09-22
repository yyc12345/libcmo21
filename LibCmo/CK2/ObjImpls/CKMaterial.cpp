#include "CKMaterial.hpp"
#include "../CKStateChunk.hpp"
#include "../CKContext.hpp"
#include "CKTexture.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKMaterial::CKMaterial(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CKBeObject(ctx, ckid, name),
		// following init are gotten from SDK document.
		m_Diffuse(0.7f, 0.7f, 0.7f, 1.0f),
		m_Ambient(0.3f, 0.3f, 0.3f, 1.0f),
		m_Specular(0.5f, 0.5f, 0.5f, 1.0f), m_SpecularPower(0.0f),
		m_Emissive(0.0f, 0.0f, 0.0f, 1.0f),
		m_EnableTwoSided(false),
		m_Textures { nullptr, nullptr, nullptr, nullptr },
		m_TextureMinMode(VxMath::VXTEXTURE_FILTERMODE::VXTEXTUREFILTER_LINEAR), m_TextureMagMode(VxMath::VXTEXTURE_FILTERMODE::VXTEXTUREFILTER_LINEAR),
		m_SourceBlend(VxMath::VXBLEND_MODE::VXBLEND_ONE), m_DestBlend(VxMath::VXBLEND_MODE::VXBLEND_ZERO), m_EnableAlphaBlend(false),
		m_ShadeMode(VxMath::VXSHADE_MODE::VXSHADE_GOURAUD),
		m_FillMode(VxMath::VXFILL_MODE::VXFILL_SOLID),
		m_EnableAlphaTest(false),
		m_EnableZWrite(true),
		// following init are gotten from IDA.
		m_EnablePerspectiveCorrection(true),
		m_TextureBlendMode(VxMath::VXTEXTURE_BLENDMODE::VXTEXTUREBLEND_MODULATEALPHA),
		m_TextureAddressMode(VxMath::VXTEXTURE_ADDRESSMODE::VXTEXTURE_ADDRESSWRAP),
		m_ZFunc(VxMath::VXCMPFUNC::VXCMP_LESSEQUAL),
		m_AlphaFunc(VxMath::VXCMPFUNC::VXCMP_ALWAYS),
		m_Effect(VxMath::VX_EFFECT::VXEFFECT_NONE),
		m_TextureBorderColor(0),
		m_AlphaRef(0)
	{}

	CKMaterial::~CKMaterial() {}

	void CKMaterial::CheckPreDeletion() {
		CKBeObject::CheckPreDeletion();

		// check 4 textures
		for (auto& tex : m_Textures) {
			if (tex != nullptr && tex->IsToBeDeleted()) {
				tex = nullptr;
			}
		}
	}

	bool CKMaterial::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CKBeObject::Save(chunk, file, flags);
		if (!suc) return false;

		return true;
	}

	bool CKMaterial::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CKBeObject::Load(chunk, file);
		if (!suc) return false;

		// clear textures
		for (auto& tex : m_Textures) {
			tex = nullptr;
		}

		// read main data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MATERIAL::CK_STATESAVE_MATDATA)) {
			if (chunk->GetDataVersion() < CK_STATECHUNK_DATAVERSION::CHUNK_MAJORCHANGE_VERSION) {
				// MARK: old data process. i don't want to implement
				// because it is not related to my work. return false
				return false;
			} else {
				// 4 basic color and some power
				CKDWORD col;
				chunk->ReadStruct(col);
				m_Diffuse.FromARGB(col);
				chunk->ReadStruct(col);
				m_Ambient.FromARGB(col);
				chunk->ReadStruct(col);
				m_Specular.FromARGB(col);
				chunk->ReadStruct(col);
				m_Emissive.FromARGB(col);

				chunk->ReadStruct(m_SpecularPower);

				// main texture
				CKObject* tex = nullptr;
				chunk->ReadObjectPointer(tex);
				if (tex != nullptr && tex->GetClassID() == CK_CLASSID::CKCID_TEXTURE) {
					m_Textures[0] = static_cast<CKTexture*>(tex);
				}

				// misc data
				chunk->ReadStruct(m_TextureBorderColor);

				// mix data 1, including some blend enums
				// 32bit data. each 4 bit store a value. total 8 data.
				// HIGH >>> m_TextureAddressMode, m_FillMode, m_ShadeMode, m_DestBlend, m_SourceBlend, m_TextureMagMode, m_TextureMinMode, m_TextureBlendMode <<< LOW
				CKDWORD mixdata;
				chunk->ReadStruct(mixdata);
				m_TextureBlendMode = static_cast<VxMath::VXTEXTURE_BLENDMODE>(mixdata & 0xF);
				mixdata >>= 4;
				m_TextureMinMode = static_cast<VxMath::VXTEXTURE_FILTERMODE>(mixdata & 0xF);
				mixdata >>= 4;
				m_TextureMagMode = static_cast<VxMath::VXTEXTURE_FILTERMODE>(mixdata & 0xF);
				mixdata >>= 4;
				m_SourceBlend = static_cast<VxMath::VXBLEND_MODE>(mixdata & 0xF);
				mixdata >>= 4;
				m_DestBlend = static_cast<VxMath::VXBLEND_MODE>(mixdata & 0xF);
				mixdata >>= 4;
				m_ShadeMode = static_cast<VxMath::VXSHADE_MODE>(mixdata & 0xF);
				mixdata >>= 4;
				m_FillMode = static_cast<VxMath::VXFILL_MODE>(mixdata & 0xF);
				mixdata >>= 4;
				m_TextureAddressMode = static_cast<VxMath::VXTEXTURE_ADDRESSMODE>(mixdata & 0xF);

				// mix data 2, including enable flag and transparent data
				// 32bit data.
				// HIGH >>> 0xFFFF(m_AlphaRef) 0xFFFF(m_AlphaFunc) 0xFFFF(m_ZFunc) 0xFFFF(enable flags) <<< LOW
				// for enable flags, total 8 bit. only low 5 bit used.
				// HIGH >>> 0(not used) 0(not used) 0(not used) 1(m_EnableAlphaTest) 1(m_EnableAlphaBlend) 1(m_EnablePerspectiveCorrection) 1(m_EnableZWrite) 1(m_EnableTwoSided) <<< LOW
				chunk->ReadStruct(mixdata);
				m_EnableTwoSided = mixdata & 0b1;
				m_EnableZWrite = mixdata & 0b10;
				m_EnablePerspectiveCorrection = mixdata & 0b100;
				m_EnableAlphaBlend = mixdata & 0b1000;
				m_EnableAlphaTest = mixdata & 0b10000;
				mixdata >>= 8;
				m_ZFunc = static_cast<VxMath::VXCMPFUNC>(mixdata & 0xFF);
				mixdata >>= 8;
				m_AlphaFunc = static_cast<VxMath::VXCMPFUNC>(mixdata & 0xFF);
				mixdata >>= 8;
				m_AlphaRef = static_cast<CKBYTE>(mixdata & 0xFF);

			}
		}

		// extra texture data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MATERIAL::CK_STATESAVE_MATDATA2)) {
			// read 3 extra texture
			CKObject* tex = nullptr;

			for (size_t i = 1; i < 4; ++i) {
				chunk->ReadObjectPointer(tex);
				if (tex != nullptr && tex->GetClassID() == CK_CLASSID::CKCID_TEXTURE) {
					m_Textures[i] = static_cast<CKTexture*>(tex);
				}
			}
		}

		// single effect
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MATERIAL::CK_STATESAVE_MATDATA3)) {
			CKDWORD data;
			chunk->ReadStruct(data);
			m_Effect = static_cast<VxMath::VX_EFFECT>(data);
		}

		// effect with parameter
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_MATERIAL::CK_STATESAVE_MATDATA5)) {
			// MARK: i do not support CKParameter anymore.
			// so we downgrade it into single effect type.
			// hope this convertion will not break anything.

			// drop parameter id.
			CK_ID paramid;
			chunk->ReadObjectID(paramid);

			// read effect self
			CKDWORD data;
			chunk->ReadStruct(data);
			m_Effect = static_cast<VxMath::VX_EFFECT>(data);
		}

		return true;
	}

#pragma region Data Visitor

	const VxMath::VxColor& CKMaterial::GetDiffuse() const {
		return m_Diffuse;
	}
	void CKMaterial::SetDiffuse(const VxMath::VxColor& col) {
		m_Diffuse = col;
	}
	const VxMath::VxColor& CKMaterial::GetAmbient() const {
		return m_Ambient;
	}
	void CKMaterial::SetAmbient(const VxMath::VxColor& col) {
		m_Ambient = col;
	}
	const VxMath::VxColor& CKMaterial::GetSpecular() const {
		return m_Specular;
	}
	void CKMaterial::SetSpecular(const VxMath::VxColor& col) {
		m_Specular = col;
	}
	const VxMath::VxColor& CKMaterial::GetEmissive() const {
		return m_Emissive;
	}
	void CKMaterial::SetEmissive(const VxMath::VxColor& col) {
		m_Emissive = col;
	}
	CKFLOAT CKMaterial::GetSpecularPower() const {
		return m_SpecularPower;
	}
	void CKMaterial::SetSpecularPower(CKFLOAT val) {
		m_SpecularPower = val;
	}


	CKTexture* CKMaterial::GetTexture(CKDWORD idx) const {
		if (idx >= m_Textures.size()) return nullptr;
		return m_Textures[idx];
	}
	void CKMaterial::SetTexture(CKTexture* tex, CKDWORD idx) {
		if (idx >= m_Textures.size()) return;
		m_Textures[idx] = tex;
	}
	CKDWORD CKMaterial::GetTextureBorderColor() const {
		return m_TextureBorderColor;
	}
	void CKMaterial::SetTextureBorderColor(CKDWORD val) {
		m_TextureBorderColor = val;
	}


	VxMath::VXTEXTURE_BLENDMODE CKMaterial::GetTextureBlendMode() const {
		return m_TextureBlendMode;
	}
	void CKMaterial::SetTextureBlendMode(VxMath::VXTEXTURE_BLENDMODE val) {
		m_TextureBlendMode = val;
	}
	VxMath::VXTEXTURE_FILTERMODE CKMaterial::GetTextureMinMode() const {
		return m_TextureMinMode;
	}
	void CKMaterial::SetTextureMinMode(VxMath::VXTEXTURE_FILTERMODE val) {
		m_TextureMinMode = val;
	}
	VxMath::VXTEXTURE_FILTERMODE CKMaterial::GetTextureMagMode() const {
		return m_TextureMagMode;
	}
	void CKMaterial::SetTextureMagMode(VxMath::VXTEXTURE_FILTERMODE val) {
		m_TextureMagMode = val;
	}
	VxMath::VXTEXTURE_ADDRESSMODE CKMaterial::GetTextureAddressMode() const {
		return m_TextureAddressMode;
	}
	void CKMaterial::SetTextureAddressMode(VxMath::VXTEXTURE_ADDRESSMODE val) {
		m_TextureAddressMode = val;
	}


	VxMath::VXBLEND_MODE CKMaterial::GetSourceBlend() const {
		return m_SourceBlend;
	}
	void CKMaterial::SetSourceBlend(VxMath::VXBLEND_MODE val) {
		m_SourceBlend = val;
	}
	VxMath::VXBLEND_MODE CKMaterial::GetDestBlend() const {
		return m_DestBlend;
	}
	void CKMaterial::SetDestBlend(VxMath::VXBLEND_MODE val) {
		m_DestBlend = val;
	}
	VxMath::VXFILL_MODE CKMaterial::GetFillMode() const {
		return m_FillMode;
	}
	void CKMaterial::SetFillMode(VxMath::VXFILL_MODE val) {
		m_FillMode = val;
	}
	VxMath::VXSHADE_MODE CKMaterial::GetShadeMode() const {
		return m_ShadeMode;
	}
	void CKMaterial::SetShadeMode(VxMath::VXSHADE_MODE val) {
		m_ShadeMode = val;
	}


	bool CKMaterial::GetAlphaTestEnabled() const {
		return m_EnableAlphaTest;
	}
	void CKMaterial::SetAlphaTestEnabled(bool enabled) {
		m_EnableAlphaTest = enabled;
	}
	bool CKMaterial::GetAlphaBlendEnabled() const {
		return m_EnableAlphaBlend;
	}
	void CKMaterial::SetAlphaBlendEnabled(bool enabled) {
		m_EnableAlphaBlend = enabled;
	}
	bool CKMaterial::GetPerspectiveCorrectionEnabled() const {
		return m_EnablePerspectiveCorrection;
	}
	void CKMaterial::SetPerspectiveCorrectionEnabled(bool enabled) {
		m_EnablePerspectiveCorrection = enabled;
	}
	bool CKMaterial::GetZWriteEnabled() const {
		return m_EnableZWrite;
	}
	void CKMaterial::SetZWriteEnabled(bool enabled) {
		m_EnableZWrite = enabled;
	}
	bool CKMaterial::GetTwoSidedEnabled() const {
		return m_EnableTwoSided;
	}
	void CKMaterial::SetTwoSidedEnabled(bool enabled) {
		m_EnableTwoSided = enabled;
	}


	CKBYTE CKMaterial::GetAlphaRef() const {
		return m_AlphaRef;
	}
	void CKMaterial::SetAlphaRef(CKBYTE val) {
		m_AlphaRef = val;
	}
	VxMath::VXCMPFUNC CKMaterial::GetAlphaFunc() const {
		return m_AlphaFunc;
	}
	void CKMaterial::SetAlphaFunc(VxMath::VXCMPFUNC val) {
		m_AlphaFunc = val;
	}
	VxMath::VXCMPFUNC CKMaterial::GetZFunc() const {
		return m_ZFunc;
	}
	void CKMaterial::SetZFunc(VxMath::VXCMPFUNC val) {
		m_ZFunc = val;
	}
	VxMath::VX_EFFECT CKMaterial::GetEffect() const {
		return m_Effect;
	}
	void CKMaterial::SetEffect(VxMath::VX_EFFECT val) {
		m_Effect = val;
	}

#pragma endregion

}