#include "CKLight.hpp"
#include "../CKStateChunk.hpp"

namespace LibCmo::CK2::ObjImpls {

	CKLight::CKLight(CKContext* ctx, CK_ID ckid, CKSTRING name) :
		CK3dEntity(ctx, ckid, name),
		m_LightData(), m_LightFlags(LightFlags::Active), m_LightPower(1.0f) {
		// Setup light data
		m_LightData.m_Type = VxMath::VXLIGHT_TYPE::VX_LIGHTPOINT;
		m_LightData.m_Diffuse = VxMath::VxColor(1.0f, 1.0f, 1.0f);
		m_LightData.m_Specular = VxMath::VxColor(0);
		m_LightData.m_Ambient = VxMath::VxColor(0.0f, 0.0f, 0.0f);
		m_LightData.m_Range = 5000.0f;
		m_LightData.m_Falloff = 1.0f;
		m_LightData.m_Attenuation0 = 1.0f;
		m_LightData.m_Attenuation1 = 0.0f;
		m_LightData.m_Attenuation2 = 0.0f;
		m_LightData.m_InnerSpotCone = 0.69813174f; // MARK: Perhaps 40 deg in rad.
		m_LightData.m_OuterSpotCone = 0.78539819f; // MARK: Perhaps 45 deg in rad.
	}

	CKLight::~CKLight() {}

	bool CKLight::Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) {
		bool suc = CK3dEntity::Save(chunk, file, flags);
		if (!suc) return false;
		
		// Save main data
		{
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_LIGHT::CK_STATESAVE_LIGHTDATA);

			// Combine light type and flags data.
			CKDWORD light_type_and_flags = static_cast<CKDWORD>(m_LightFlags) & 0xFFFFFF00u;
			light_type_and_flags |= static_cast<CKDWORD>(m_LightData.m_Type) & 0xFFu;
			chunk->WriteStruct(light_type_and_flags);
			
			// Save diffuse color with constant 1.0 alpha factor.
			chunk->WriteStruct(m_LightData.m_Diffuse.ToARGB() | 0xFF000000u);

			chunk->WriteStruct(m_LightData.m_Attenuation0);
			chunk->WriteStruct(m_LightData.m_Attenuation1);
			chunk->WriteStruct(m_LightData.m_Attenuation2);

			chunk->WriteStruct(m_LightData.m_Range);

			if (m_LightData.m_Type == VxMath::VXLIGHT_TYPE::VX_LIGHTSPOT) {
				chunk->WriteStruct(m_LightData.m_OuterSpotCone);
				chunk->WriteStruct(m_LightData.m_InnerSpotCone);
				chunk->WriteStruct(m_LightData.m_Falloff);
			}

		}

		// Save light power
		if (m_LightPower != 1.0f) {
			chunk->WriteIdentifier(CK_STATESAVEFLAGS_LIGHT::CK_STATESAVE_LIGHTDATA2);
			chunk->WriteStruct(m_LightPower);
		}

		chunk->SetClassId(CK_CLASSID::CKCID_LIGHT);
		return true;
	}

	bool CKLight::Load(CKStateChunk* chunk, CKFileVisitor* file) {
		bool suc = CK3dEntity::Load(chunk, file);
		if (!suc) return false;
		
		// MARK: I drop the read process for too low version.
		// return false anyway.
		if (chunk->GetDataVersion() < CK_STATECHUNK_DATAVERSION::CHUNK_MAJORCHANGE_VERSION) {
			return false;
		}

		// Read main data
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_LIGHT::CK_STATESAVE_LIGHTDATA)) {
			// Read a DWORD storing light type and flags
			// Because the lowest byte in light flags always is 0x00,
			// so Virtools use it to store light type.
			// After removing light type component, the rest of data is light flags.
			// (do not need any SHIFT! It's okey that just set the lowest byte to zero.)
			CKDWORD light_type_and_flags;
			chunk->ReadStruct(light_type_and_flags);
			m_LightData.m_Type = static_cast<VxMath::VXLIGHT_TYPE>(light_type_and_flags & 0xFFu);
			m_LightFlags = static_cast<LightFlags>(light_type_and_flags & 0xFFFFFF00u);

			CKDWORD dword_diffuse;
			chunk->ReadStruct(dword_diffuse);
			m_LightData.m_Diffuse = VxMath::VxColor(dword_diffuse);

			chunk->ReadStruct(m_LightData.m_Attenuation0);
			chunk->ReadStruct(m_LightData.m_Attenuation1);
			chunk->ReadStruct(m_LightData.m_Attenuation2);

			chunk->ReadStruct(m_LightData.m_Range);

			if (m_LightData.m_Type == VxMath::VXLIGHT_TYPE::VX_LIGHTSPOT) {
				chunk->ReadStruct(m_LightData.m_OuterSpotCone);
				chunk->ReadStruct(m_LightData.m_InnerSpotCone);
				chunk->ReadStruct(m_LightData.m_Falloff);
			}

		}

		// Read light power
		if (chunk->SeekIdentifier(CK_STATESAVEFLAGS_LIGHT::CK_STATESAVE_LIGHTDATA2)) {
			chunk->ReadStruct(m_LightPower);
		} else {
			m_LightPower = 1.0f;
		}

		// Correct light type to prevent accident out of range value.
		switch (m_LightData.m_Type) {
			case VxMath::VXLIGHT_TYPE::VX_LIGHTPOINT:
			case VxMath::VXLIGHT_TYPE::VX_LIGHTSPOT:
			case VxMath::VXLIGHT_TYPE::VX_LIGHTDIREC:
			case VxMath::VXLIGHT_TYPE::VX_LIGHTPARA:
				// do nothing
				break;
			default:
				// reset it to point
				m_LightData.m_Type = VxMath::VXLIGHT_TYPE::VX_LIGHTPOINT;
				break;
		}

		return true;
	}

#pragma region Class Operations

	VxMath::VXLIGHT_TYPE CKLight::GetType() const {
		return m_LightData.m_Type;
	}
	void CKLight::SetType(VxMath::VXLIGHT_TYPE light_type) {
		m_LightData.m_Type = light_type;
	}

	const VxMath::VxColor& CKLight::GetColor() const {
		return m_LightData.m_Diffuse;
	}
	void CKLight::SetColor(const VxMath::VxColor& c) {
		m_LightData.m_Diffuse = c;
	}

	CKFLOAT CKLight::GetConstantAttenuation() const {
		return m_LightData.m_Attenuation0;
	}
	CKFLOAT CKLight::GetLinearAttenuation() const {
		return m_LightData.m_Attenuation1;
	}
	CKFLOAT CKLight::GetQuadraticAttenuation() const {
		return m_LightData.m_Attenuation2;
	}
	void CKLight::SetConstantAttenuation(CKFLOAT value) {
		m_LightData.m_Attenuation0 = value;
	}
	void CKLight::SetLinearAttenuation(CKFLOAT value) {
		m_LightData.m_Attenuation1 = value;
	}
	void CKLight::SetQuadraticAttenuation(CKFLOAT value) {
		m_LightData.m_Attenuation2 = value;
	}

	CKFLOAT CKLight::GetRange() const {
		return m_LightData.m_Range;
	}
	void CKLight::SetRange(CKFLOAT value) {
		m_LightData.m_Range = value;
	}

	CKFLOAT CKLight::GetHotSpot() const {
		return m_LightData.m_InnerSpotCone;
	}
	CKFLOAT CKLight::GetFalloff() const {
		return m_LightData.m_OuterSpotCone;
	}
	CKFLOAT CKLight::GetFalloffShape() const {
		return m_LightData.m_Falloff;
	}
	void CKLight::SetHotSpot(CKFLOAT value) {
		m_LightData.m_InnerSpotCone = value;
	}
	void CKLight::SetFalloff(CKFLOAT value) {
		m_LightData.m_OuterSpotCone = value;
	}
	void CKLight::SetFalloffShape(CKFLOAT value) {
		m_LightData.m_Falloff = value;
	}

	bool CKLight::GetActivity() const {
		return YYCC::EnumHelper::Has(m_LightFlags, LightFlags::Active);
	}
	void CKLight::Active(bool active) {
		if (active) {
			YYCC::EnumHelper::Add(m_LightFlags, LightFlags::Active);
		} else {
			YYCC::EnumHelper::Remove(m_LightFlags, LightFlags::Active);
		}
	}

	bool CKLight::GetSpecularFlag() const {
		return YYCC::EnumHelper::Has(m_LightFlags, LightFlags::Specular);
	}
	void CKLight::SetSpecularFlag(bool specular) {
		if (specular) {
			YYCC::EnumHelper::Add(m_LightFlags, LightFlags::Specular);
		} else {
			YYCC::EnumHelper::Remove(m_LightFlags, LightFlags::Specular);
		}
	}

	CK3dEntity* CKLight::GetTarget() const {
		// Normal light do not support target.
		// So it always return nullptr.
		return nullptr;
	}
	void CKLight::SetTarget(CK3dEntity* target) {
		// Normal light do not support target.
		// So, do nothing.
	}

	CKFLOAT CKLight::GetLightPower() const {
		return m_LightPower;
	}
	void CKLight::SetLightPower(CKFLOAT power) {
		m_LightPower = power;
	}

#pragma endregion

}
