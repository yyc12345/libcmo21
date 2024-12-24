#pragma once

#include "../../VTInternal.hpp"
#include "CK3dEntity.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKLight : public CK3dEntity {
	public:
		CKLight(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKLight();
		YYCC_DEL_CLS_COPY_MOVE(CKLight);

		virtual CK_CLASSID GetClassID(void) override {
			return CK_CLASSID::CKCID_LIGHT;
		}

		// 2 RW funcions
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;

		// Type
		VxMath::VXLIGHT_TYPE GetType() const;
		void SetType(VxMath::VXLIGHT_TYPE light_type);

		const VxMath::VxColor& GetColor() const;
		void SetColor(const VxMath::VxColor& c);

		CKFLOAT GetConstantAttenuation() const;
		CKFLOAT GetLinearAttenuation() const;
		CKFLOAT GetQuadraticAttenuation() const;
		void SetConstantAttenuation(CKFLOAT value);
		void SetLinearAttenuation(CKFLOAT value);
		void SetQuadraticAttenuation(CKFLOAT value);

		// Range
		CKFLOAT GetRange() const;
		void SetRange(CKFLOAT value);

		// Spotlight options
		CKFLOAT GetHotSpot() const;
		CKFLOAT GetFalloff() const;
		CKFLOAT GetFalloffShape() const;
		void SetHotSpot(CKFLOAT value);
		void SetFalloff(CKFLOAT value);
		void SetFalloffShape(CKFLOAT value);

		// Activity options
		bool GetActivity() const;
		void Active(bool active);

		bool GetSpecularFlag() const;
		void SetSpecularFlag(bool specular);

		// Target access
		virtual CK3dEntity* GetTarget() const;
		virtual void SetTarget(CK3dEntity* target);

		CKFLOAT GetLightPower() const;
		void SetLightPower(CKFLOAT power = 1.0f);

	protected:
		struct CKLightData {
			VxMath::VXLIGHT_TYPE m_Type;
			VxMath::VxColor m_Diffuse;
			VxMath::VxColor m_Specular;
			VxMath::VxColor m_Ambient;
			VxMath::VxVector3 m_Position;
			VxMath::VxVector3 m_Direction;
			CKFLOAT m_Range;
			CKFLOAT m_Falloff;
			CKFLOAT m_Attenuation0;
			CKFLOAT m_Attenuation1;
			CKFLOAT m_Attenuation2;
			CKFLOAT m_InnerSpotCone;
			CKFLOAT m_OuterSpotCone;
		};
		enum class LightFlags : CKDWORD {
			None = 0,
			Active = 0x100u, /**< if set, this light is active. */
			Specular = 0x200u, /**< if set, this light has specular flag. */
		};

		CKLightData m_LightData;
		// MARK: This variable is called in m_Flags in reverse code.
		LightFlags m_LightFlags;
		CKFLOAT m_LightPower;
	};

}
