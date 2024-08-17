#pragma once

#include "../../VTInternal.hpp"
#include "../CKBitmapData.hpp"
#include "CKBeObject.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKTexture : public CKBeObject {
	public:
		CKTexture(CKContext* ctx, CK_ID ckid, CKSTRING name);
		virtual ~CKTexture();
		YYCC_DEL_CLS_COPY_MOVE(CKTexture);

		virtual CK_CLASSID GetClassID(void) override { 
			return CK_CLASSID::CKCID_TEXTURE; 
		}
		
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags) override;
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file) override;

		CKBitmapData& GetUnderlyingData();

		/**
		 * @brief A wrapper of underlying CKBitmapData::LoadImage. Not only load image, but also set file name.
		 * @param filename[in] File name of loading image.
		 * @param slot[in] The slot that image will be loaded into.
		 * @return True if success.
		*/
		bool LoadImage(CKSTRING filename, CKDWORD slot);

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
