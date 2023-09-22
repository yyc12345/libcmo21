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
		
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;

		CKBitmapData& GetUnderlyingData();

		bool IsUseMipmap() const;
		void UseMipmap(bool isUse);
		CKDWORD GetMipmapLevel() const;
		void SetMipmapLevel(CKDWORD level);
		VxMath::VxImageDescEx* GetMipmapLevelData(CKDWORD level);

		VxMath::VX_PIXELFORMAT GetVideoFormat() const;
		void SetVideoFormat(VxMath::VX_PIXELFORMAT fmt);

	protected:
		CKBitmapData m_ImageHost;
		VxMath::VX_PIXELFORMAT m_VideoFormat;
		bool m_UseMipMap;
		XContainer::XArray<VxMath::VxImageDescEx> m_MipmapImages;
	};

}
