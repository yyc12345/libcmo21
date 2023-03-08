#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo::CK2::CKObjectImplements {

	class CKObject {
	public:
		CK_ID m_ID;
		std::string m_Name;
		CK_OBJECT_FLAGS m_ObjectFlags;
		CKMinContext* m_Context;

		CKObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKObject(const CKObject&) = delete;
		CKObject& operator=(const CKObject&) = delete;
		virtual ~CKObject();

		CK_ID GetID(void) { return this->m_ID; }
		CKSTRING GetName(void) { return (this->m_Name.empty() ? nullptr : this->m_Name.c_str()); }
		void SetName(CKSTRING u8_name) { this->m_Name = (u8_name == nullptr ? "" : u8_name); }
		CK_OBJECT_FLAGS GetObjectFlags(void) { return this->m_ObjectFlags; }
		void SetObjectFlags(CK_OBJECT_FLAGS flags) { this->m_ObjectFlags = flags; }

		virtual CK_CLASSID GetClassID(void) { return CK_CLASSID::CKCID_OBJECT; }
		virtual bool Load(CKStateChunk* chunk, const CKFileDocument* doc);
		virtual CKStateChunk* Save(const CKFileDocument* doc);
	};

	class CKSceneObject : public CKObject {
	public:
		CKSceneObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKSceneObject(const CKSceneObject&) = delete;
		CKSceneObject& operator=(const CKSceneObject&) = delete;
		virtual ~CKSceneObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_SCENEOBJECT; }
	protected:
		XBitArray m_Scenes;
	};

	class CKBeObject : public CKSceneObject {
	public:
		CKBeObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKBeObject(const CKBeObject&) = delete;
		CKBeObject& operator=(const CKBeObject&) = delete;
		virtual ~CKBeObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEOBJECT; }
	protected:

	};

#pragma region Map Related

	class CKGroup : public CKBeObject {
	public:
		CKGroup(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKGroup(const CKGroup&) = delete;
		CKGroup& operator=(const CKGroup&) = delete;
		virtual ~CKGroup();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_GROUP; }
	protected:

	};

	class CKMesh : public CKBeObject {
	public:
		CKMesh(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKMesh(const CKMesh&) = delete;
		CKMesh& operator=(const CKMesh&) = delete;
		virtual ~CKMesh();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_MESH; }
	protected:

	};

	class CKTexture : public CKBeObject {
	public:
		CKTexture(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKTexture(const CKTexture&) = delete;
		CKTexture& operator=(const CKTexture&) = delete;
		virtual ~CKTexture();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_TEXTURE; }
	protected:

	};

	class CKMaterial : public CKBeObject {
	public:
		CKMaterial(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKMaterial(const CKMaterial&) = delete;
		CKMaterial& operator=(const CKMaterial&) = delete;
		virtual ~CKMaterial();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_MATERIAL; }
	protected:

	};

	class CKRenderObject : public CKBeObject {
	public:
		CKRenderObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKRenderObject(const CKRenderObject&) = delete;
		CKRenderObject& operator=(const CKRenderObject&) = delete;
		~CKRenderObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_RENDEROBJECT; }
	protected:

	};

	class CK3dEntity : public CKRenderObject {
	public:
		CK3dEntity(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CK3dEntity(const CK3dEntity&) = delete;
		CK3dEntity& operator=(const CK3dEntity&) = delete;
		virtual ~CK3dEntity();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_3DENTITY; }
	protected:

	};

	class CK3dObject :public CK3dEntity {
	public:
		CK3dObject(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CK3dObject(const CK3dObject&) = delete;
		CK3dObject& operator=(const CK3dObject&) = delete;
		~CK3dObject();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_3DOBJECT; }
	protected:

	};

#pragma endregion

#pragma region Behavior Related

	class CKParameterIn :public CKObject {
	public:
		CKParameterIn(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKParameterIn(const CKParameterIn&) = delete;
		CKParameterIn& operator=(const CKParameterIn&) = delete;
		~CKParameterIn();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETERIN; }
	protected:

	};

	class CKParameter :public CKObject {
	public:
		CKParameter(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKParameter(const CKParameter&) = delete;
		CKParameter& operator=(const CKParameter&) = delete;
		~CKParameter();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETER; }
	protected:

	};

	class CKParameterOut :public CKParameter {
	public:
		CKParameterOut(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKParameterOut(const CKParameterOut&) = delete;
		CKParameterOut& operator=(const CKParameterOut&) = delete;
		~CKParameterOut();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETEROUT; }
	protected:

	};

	class CKParameterLocal :public CKParameter {
	public:
		CKParameterLocal(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKParameterLocal(const CKParameterLocal&) = delete;
		CKParameterLocal& operator=(const CKParameterLocal&) = delete;
		~CKParameterLocal();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETERLOCAL; }
	protected:

	};

	class CKParameterOperation :public CKObject {
	public:
		CKParameterOperation(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKParameterOperation(const CKParameterOperation&) = delete;
		CKParameterOperation& operator=(const CKParameterOperation&) = delete;
		~CKParameterOperation();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETEROPERATION; }
	protected:

	};

	class CKBehaviorLink :public CKObject {
	public:
		CKBehaviorLink(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKBehaviorLink(const CKBehaviorLink&) = delete;
		CKBehaviorLink& operator=(const CKBehaviorLink&) = delete;
		~CKBehaviorLink();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEHAVIORLINK; }
	protected:

	};

	class CKBehaviorIO :public CKObject {
	public:
		CKBehaviorIO(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKBehaviorIO(const CKBehaviorIO&) = delete;
		CKBehaviorIO& operator=(const CKBehaviorIO&) = delete;
		~CKBehaviorIO();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEHAVIORIO; }
	protected:

	};

	class CKBehavior :public CKSceneObject {
	public:
		CKBehavior(CKMinContext* ctx, CK_ID ckid, CKSTRING name);
		CKBehavior(const CKBehavior&) = delete;
		CKBehavior& operator=(const CKBehavior&) = delete;
		~CKBehavior();

		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEHAVIOR; }
	protected:

	};

#pragma endregion


}
