#pragma once

#include "CKDefines.hpp"
#include "CKEnums.hpp"

namespace LibCmo {

	// forward decl to rm recursive reference
	class CKMinContext;
	class CKStateChunk;

	namespace ObjsImpl {

		class CKObject {
		public:
			CKObject();
			CKObject(const CKObject&) = delete;
			CKObject& operator=(const CKObject&) = delete;
			virtual ~CKObject();

		private:

		};

		class CKSceneObject : public CKObject {
		public:
			CKSceneObject();
			CKSceneObject(const CKSceneObject&) = delete;
			CKSceneObject& operator=(const CKSceneObject&) = delete;
			virtual ~CKSceneObject();

		private:

		};

		class CKBeObject : public CKSceneObject {
		public:
			CKBeObject();
			CKBeObject(const CKBeObject&) = delete;
			CKBeObject& operator=(const CKBeObject&) = delete;
			virtual ~CKBeObject();

		private:

		};

		class CKGroup : public CKBeObject {
		public:
			CKGroup();
			CKGroup(const CKGroup&) = delete;
			CKGroup& operator=(const CKGroup&) = delete;
			virtual ~CKGroup();

		private:

		};

		class CKMesh : public CKBeObject {
		public:
			CKMesh();
			CKMesh(const CKMesh&) = delete;
			CKMesh& operator=(const CKMesh&) = delete;
			virtual ~CKMesh();

		private:

		};

		class CKTexture : public CKBeObject {
		public:
			CKTexture();
			CKTexture(const CKTexture&) = delete;
			CKTexture& operator=(const CKTexture&) = delete;
			virtual ~CKTexture();

		private:

		};

		class CKMaterial : public CKBeObject {
		public:
			CKMaterial();
			CKMaterial(const CKMaterial&) = delete;
			CKMaterial& operator=(const CKMaterial&) = delete;
			virtual ~CKMaterial();

		private:

		};

		class CKRenderObject : public CKBeObject {
		public:
			CKRenderObject();
			CKRenderObject(const CKRenderObject&) = delete;
			CKRenderObject& operator=(const CKRenderObject&) = delete;
			~CKRenderObject();

		private:

		};

		class CK3dEntity : public CKBeObject {
		public:
			CK3dEntity();
			CK3dEntity(const CK3dEntity&) = delete;
			CK3dEntity& operator=(const CK3dEntity&) = delete;
			virtual ~CK3dEntity();

		private:

		};

		class CK3dObject :public CK3dEntity {
		public:
			CK3dObject();
			CK3dObject(const CK3dObject&) = delete;
			CK3dObject& operator=(const CK3dObject&) = delete;
			~CK3dObject();

		private:

		};


	}
}
