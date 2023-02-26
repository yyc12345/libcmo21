#pragma once

#include <string>
#include <vector>

namespace LibCmo {
	namespace CK2 {

		// some important type define

		using CK_ID = uint32_t;

		using CKDWORD = uint32_t;
		using CKWORD = uint16_t;
		using CKBYTE = uint8_t;
		using CKBOOL = int32_t;
		using CKINT = int32_t;

		using CKMUTSTRING = char*;
		using CKSTRING = const char*;

		using XString = std::string;
		using XBitArray = std::vector<bool>;
		template<typename T>
		using XArray = std::vector<T>;
		using XIntArray = std::vector<int32_t>;
		template<typename T>
		using XClassArray = std::vector<T>;
		//using CKObjectArray = std::vector<CKObject*>;


		// forward decl for some CKObjects
		namespace CKObjectImplements {
			class CKObject;
			class CKParameterIn;
			class CKParameter;
			class CKParameterOut;
			class CKParameterLocal;
			class CKParameterOperation;
			class CKBehaviorLink;
			class CKBehaviorIO;

			class CKSceneObject;
			class CKBehavior;

			class CKBeObject;
			class CKScene;
			class CKLevel;
			class CKPlace;
			class CKGroup;
			class CKMaterial;
			class CKTexture;
			class CKMesh;
			class CKDataArray;
			class CKRenderObject;
			class CK3dEntity;
			class CK3dObject;
		}
		// forward decl for some CKManagers
		namespace CKManagerImplements {
			class CKBaseManager;
			class CKAttributeManager;
		}

		// forward decl for some important CK2 classes
		class CKMinContext;
		class CKStateChunk;
		class CKFile;
		namespace CKFileData {
			class ShallowDocument;
			class DeepDocument;
			class HybridDocument;
		}

		// useful struct define
		struct CKGUID {
			union {
				struct {
					CKDWORD d1, d2;
				};
				CKDWORD d[2];
			};
			CKGUID(CKDWORD gd1 = 0, CKDWORD gd2 = 0) { d[0] = gd1; d[1] = gd2; }
		};


	}

	namespace VxMath {

		// forward decl for some important VxMath classes
		class VxMemoryMappedFile;

		struct VxVector {
			union {
				struct {
					float x, y, z;
				};
				float v[3];
			};

			VxVector() : x(0.0f), y(0.0f), z(0.0f) { ; }
			VxVector(float f) : x(f), y(f), z(f) { ; }
			VxVector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { ; }
			VxVector(const float f[3]) : x(f[0]), y(f[1]), z(f[2]) { ; }
		};

		struct VxQuaternion {
			union {
				struct {
					VxVector axis;
					float angle;
				};
				struct {
					float x, y, z, w;
				};
				float v[4];
			};

			VxQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { ; }
			VxQuaternion(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { ; }
		};

		struct VxMatrix {
			float m_Data[4][4];

			VxMatrix() : m_Data() { 
				memset(m_Data, 0, sizeof(m_Data));
				m_Data[0][0] = m_Data[1][1] = m_Data[2][2] = m_Data[3][3] = 1.0f;
			}
			VxMatrix(float m[4][4]) : m_Data() { std::memcpy(m_Data, m, sizeof(m_Data)); }
		};

	}

}
