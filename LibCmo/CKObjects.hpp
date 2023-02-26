#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo::CK2::CKObjectImplements {

	class CKObject {
	public:
		CKObject();
		CKObject(const CKObject&) = delete;
		CKObject& operator=(const CKObject&) = delete;
		virtual ~CKObject();

		CK_ID GetID(void) { return this->m_ID; }
		CKSTRING GetName(void) { return this->m_Name.c_str(); }
		void SetName(CKSTRING u8_name) { this->m_Name = u8_name; }
		CK_OBJECT_FLAGS GetObjectFlags(void) { return this->m_ObjectFlags; }
		void SetObjectFlags(CK_OBJECT_FLAGS flags) { this->m_ObjectFlags = flags; }

		virtual CK_CLASSID GetClassID(void) { return CK_CLASSID::CKCID_OBJECT; }
		virtual CKERROR Load(CKStateChunk* chunk, CKFileData::ShallowDocument* doc);
		virtual CKStateChunk* Save(CKFileData::ShallowDocument* doc);
	protected:
		CK_ID m_ID;
		std::string m_Name;
		CK_OBJECT_FLAGS m_ObjectFlags;
		CKMinContext* m_Context;
	};

	class CKSceneObject : public CKObject {
	public:
		CKSceneObject();
		CKSceneObject(const CKSceneObject&) = delete;
		CKSceneObject& operator=(const CKSceneObject&) = delete;
		virtual ~CKSceneObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_SCENEOBJECT; }
	protected:
		XBitArray m_Scenes;
	};

	class CKBeObject : public CKSceneObject {
	public:
		CKBeObject();
		CKBeObject(const CKBeObject&) = delete;
		CKBeObject& operator=(const CKBeObject&) = delete;
		virtual ~CKBeObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEOBJECT; }
	protected:

	};

	class CKGroup : public CKBeObject {
	public:
		CKGroup();
		CKGroup(const CKGroup&) = delete;
		CKGroup& operator=(const CKGroup&) = delete;
		virtual ~CKGroup();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_GROUP; }
	protected:

	};

	class CKMesh : public CKBeObject {
	public:
		CKMesh();
		CKMesh(const CKMesh&) = delete;
		CKMesh& operator=(const CKMesh&) = delete;
		virtual ~CKMesh();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_MESH; }
	protected:

	};

	class CKTexture : public CKBeObject {
	public:
		CKTexture();
		CKTexture(const CKTexture&) = delete;
		CKTexture& operator=(const CKTexture&) = delete;
		virtual ~CKTexture();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_TEXTURE; }
	protected:

	};

	class CKMaterial : public CKBeObject {
	public:
		CKMaterial();
		CKMaterial(const CKMaterial&) = delete;
		CKMaterial& operator=(const CKMaterial&) = delete;
		virtual ~CKMaterial();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_MATERIAL; }
	protected:

	};

	class CKRenderObject : public CKBeObject {
	public:
		CKRenderObject();
		CKRenderObject(const CKRenderObject&) = delete;
		CKRenderObject& operator=(const CKRenderObject&) = delete;
		~CKRenderObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_RENDEROBJECT; }
	protected:

	};

	class CK3dEntity : public CKBeObject {
	public:
		CK3dEntity();
		CK3dEntity(const CK3dEntity&) = delete;
		CK3dEntity& operator=(const CK3dEntity&) = delete;
		virtual ~CK3dEntity();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_3DENTITY; }
	protected:

	};

	class CK3dObject :public CK3dEntity {
	public:
		CK3dObject();
		CK3dObject(const CK3dObject&) = delete;
		CK3dObject& operator=(const CK3dObject&) = delete;
		~CK3dObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_3DOBJECT; }
	protected:

	};


}
