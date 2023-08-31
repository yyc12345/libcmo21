#pragma once

#include "../../VTAll.hpp"

namespace LibCmo::CK2::ObjImpls {

	class CKObject {
	public:
		CKObject(CKContext* ctx, CK_ID ckid, CKSTRING name) :
			m_ID(ckid),
			m_Name(name),
			m_Context(ctx),
			m_ObjectFlags(CK_OBJECT_FLAGS::CK_PARAMETERIN_DISABLED)
		{}
		virtual ~CKObject() {}
		LIBCMO_DISABLE_COPY_MOVE(CKObject);

		CK_ID GetID(void) {
			return m_ID;
		}
		CKSTRING GetName(void) {
			return m_Name.c_str();
		}
		void SetName(CKSTRING u8_name) {
			m_Name = u8_name;
		}
		CK_OBJECT_FLAGS GetObjectFlags(void) {
			return m_ObjectFlags;
		}
		void SetObjectFlags(CK_OBJECT_FLAGS flags) {
			m_ObjectFlags = flags;
		}
		CKContext* GetCKContext() {
			return m_Context;
		}

		virtual CK_CLASSID GetClassID(void) { 
			return CK_CLASSID::CKCID_OBJECT; 
		}
		virtual void PreSave(CKFileVisitor* file, CKDWORD flags);
		virtual bool Save(CKStateChunk* chunk, CKFileVisitor* file, CKDWORD flags);
		virtual bool Load(CKStateChunk* chunk, CKFileVisitor* file);
		virtual void PostLoad();

	protected:
		CK_ID m_ID;
		TypeHelper::MKString m_Name;
		CK_OBJECT_FLAGS m_ObjectFlags;
		CKContext* m_Context;
	};

	//class CKSceneObject : public CKObject {
	//public:
	//	CKSceneObject(CKContext* ctx, CK_ID ckid, CKSTRING name);
	//	CKSceneObject(const CKSceneObject&) = delete;
	//	CKSceneObject& operator=(const CKSceneObject&) = delete;
	//	virtual ~CKSceneObject();

	//	virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_SCENEOBJECT; }
	//protected:
	//	XBitArray m_Scenes;
	//};

	//class CKBeObject : public CKSceneObject {
	//public:
	//	CKBeObject(CKContext* ctx, CK_ID ckid, CKSTRING name);
	//	CKBeObject(const CKBeObject&) = delete;
	//	CKBeObject& operator=(const CKBeObject&) = delete;
	//	virtual ~CKBeObject();

	//	virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEOBJECT; }
	//protected:

	//};
//
//#pragma region Map Related
//
//	class CKGroup : public CKBeObject {
//	public:
//		CKGroup(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKGroup(const CKGroup&) = delete;
//		CKGroup& operator=(const CKGroup&) = delete;
//		virtual ~CKGroup();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_GROUP; }
//	protected:
//
//	};
//
//	class CKMesh : public CKBeObject {
//	public:
//		CKMesh(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKMesh(const CKMesh&) = delete;
//		CKMesh& operator=(const CKMesh&) = delete;
//		virtual ~CKMesh();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_MESH; }
//	protected:
//
//	};
//
//	class CKTexture : public CKBeObject {
//	public:
//		CKTexture(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKTexture(const CKTexture&) = delete;
//		CKTexture& operator=(const CKTexture&) = delete;
//		virtual ~CKTexture();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_TEXTURE; }
//	protected:
//
//	};
//
//	class CKMaterial : public CKBeObject {
//	public:
//		CKMaterial(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKMaterial(const CKMaterial&) = delete;
//		CKMaterial& operator=(const CKMaterial&) = delete;
//		virtual ~CKMaterial();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_MATERIAL; }
//	protected:
//
//	};
//
//	class CKRenderObject : public CKBeObject {
//	public:
//		CKRenderObject(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKRenderObject(const CKRenderObject&) = delete;
//		CKRenderObject& operator=(const CKRenderObject&) = delete;
//		~CKRenderObject();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_RENDEROBJECT; }
//	protected:
//
//	};
//
//	class CK3dEntity : public CKRenderObject {
//	public:
//		CK3dEntity(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CK3dEntity(const CK3dEntity&) = delete;
//		CK3dEntity& operator=(const CK3dEntity&) = delete;
//		virtual ~CK3dEntity();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_3DENTITY; }
//	protected:
//
//	};
//
//	class CK3dObject :public CK3dEntity {
//	public:
//		CK3dObject(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CK3dObject(const CK3dObject&) = delete;
//		CK3dObject& operator=(const CK3dObject&) = delete;
//		~CK3dObject();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_3DOBJECT; }
//	protected:
//
//	};
//
//#pragma endregion
//
//#pragma region Behavior Related
//
//	class CKParameterIn :public CKObject {
//	public:
//		CKParameterIn(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKParameterIn(const CKParameterIn&) = delete;
//		CKParameterIn& operator=(const CKParameterIn&) = delete;
//		~CKParameterIn();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETERIN; }
//	protected:
//
//	};
//
//	class CKParameter :public CKObject {
//	public:
//		CKParameter(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKParameter(const CKParameter&) = delete;
//		CKParameter& operator=(const CKParameter&) = delete;
//		~CKParameter();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETER; }
//	protected:
//
//	};
//
//	class CKParameterOut :public CKParameter {
//	public:
//		CKParameterOut(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKParameterOut(const CKParameterOut&) = delete;
//		CKParameterOut& operator=(const CKParameterOut&) = delete;
//		~CKParameterOut();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETEROUT; }
//	protected:
//
//	};
//
//	class CKParameterLocal :public CKParameter {
//	public:
//		CKParameterLocal(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKParameterLocal(const CKParameterLocal&) = delete;
//		CKParameterLocal& operator=(const CKParameterLocal&) = delete;
//		~CKParameterLocal();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETERLOCAL; }
//	protected:
//
//	};
//
//	class CKParameterOperation :public CKObject {
//	public:
//		CKParameterOperation(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKParameterOperation(const CKParameterOperation&) = delete;
//		CKParameterOperation& operator=(const CKParameterOperation&) = delete;
//		~CKParameterOperation();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_PARAMETEROPERATION; }
//	protected:
//
//	};
//
//	class CKBehaviorLink :public CKObject {
//	public:
//		CKBehaviorLink(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKBehaviorLink(const CKBehaviorLink&) = delete;
//		CKBehaviorLink& operator=(const CKBehaviorLink&) = delete;
//		~CKBehaviorLink();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEHAVIORLINK; }
//	protected:
//
//	};
//
//	class CKBehaviorIO :public CKObject {
//	public:
//		CKBehaviorIO(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKBehaviorIO(const CKBehaviorIO&) = delete;
//		CKBehaviorIO& operator=(const CKBehaviorIO&) = delete;
//		~CKBehaviorIO();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEHAVIORIO; }
//	protected:
//
//	};
//
//	class CKBehavior :public CKSceneObject {
//	public:
//		CKBehavior(CKContext* ctx, CK_ID ckid, CKSTRING name);
//		CKBehavior(const CKBehavior&) = delete;
//		CKBehavior& operator=(const CKBehavior&) = delete;
//		~CKBehavior();
//
//		virtual CK_CLASSID GetClassID(void) override { return CK_CLASSID::CKCID_BEHAVIOR; }
//	protected:
//
//	};
//
//#pragma endregion


}
