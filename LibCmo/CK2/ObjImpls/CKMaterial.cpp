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
		m_Textures{nullptr, nullptr, nullptr, nullptr},
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
				CK_ID objid;
				chunk->ReadObjectID(objid);
				CKObject* tex = m_Context->GetObject(objid);
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

		return true;
	}

}