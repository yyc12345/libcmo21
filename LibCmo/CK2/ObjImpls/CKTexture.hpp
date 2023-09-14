#pragma once

#include "../../VTAll.hpp"
#include "../CKBitmapData.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKTexture : public CKBeObject {
	public:
		CKTexture(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKTexture();
		LIBCMO_DISABLE_COPY_MOVE(CKTexture);

		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_TEXTURE; 
		}
		
		//virtual void PreSave(CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;
		//virtual void PostLoad() override;

	protected:
		CKBitmapData m_ImageHost;
		VxMath::VX_PIXELFORMAT m_VideoFormat;
		bool m_UseMipMap;
		XContainer::XArray<VxMath::VxImageDescEx> m_MipmapImages;
	};

}
