#pragma once

#include "../VTUtils.hpp"
#include "../CK2/CKTypes.hpp"
#include "VxEnums.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <cinttypes>

/**
 * @brief The VxMath part of LibCmo.
 * These classes are prefixed with Vx in original Virtools SDK.
*/
namespace LibCmo::VxMath {

	// ========== Type Definition ==========


	// ========== Class List ==========
	//--- Important classes

	class VxMemoryMappedFile;

	//---- Misc

	/**
	 * @brief Class representation of a Vector in 3 dimensions
	*/
	struct VxVector {
		float x, y, z;

		VxVector() : x(0.0f), y(0.0f), z(0.0f) {}
		VxVector(float f) : x(f), y(f), z(f) {}
		VxVector(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
		VxVector(const float f[3]) : x(f[0]), y(f[1]), z(f[2]) {}
	};

	/**
	 * @brief Class representation of a Vector in 2 dimensions
	*/
	struct Vx2DVector {
		float x, y;

		Vx2DVector() : x(0.0f), y(0.0f) {}
		Vx2DVector(float f) : x(f), y(f) {}
		Vx2DVector(float _x, float _y) : x(_x), y(_y) {}
		Vx2DVector(CK2::CKINT iX, CK2::CKINT iY) : x((float)iX), y((float)iY) {}
		Vx2DVector(const float f[2]) : x(f[0]), y(f[1]) {}
	};

	/**
	 * @brief Class representation of a Quaternion
	*/
	struct VxQuaternion {
		float x, y, z, w;

		VxQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
		VxQuaternion(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}
	};

	struct VxMatrix {
		float m_Data[4][4];

		VxMatrix() : m_Data() {
			std::memset(m_Data, 0, sizeof(m_Data));
			m_Data[0][0] = m_Data[1][1] = m_Data[2][2] = m_Data[3][3] = 1.0f;
		}
		VxMatrix(float m[4][4]) : m_Data() { std::memcpy(m_Data, m, sizeof(m_Data)); }
	};

	/**
	 * VxImageDescEx describe the height, width,
	 * and etc for image.
	 * Also it hold a pointer to raw image data.
	 * The image data must be 32bit format.
	 * Thus the size of Image must be 4 * Width * Height.
	*/
	class VxImageDescEx {
	public:
		VxImageDescEx(CK2::CKDWORD height, CK2::CKDWORD width) :
			m_Width(width), m_Height(height), 
			//m_RedMask(0), m_GreenMask(0), m_BlueMask(0), m_AlphaMask(0),
			m_Image(nullptr){
			m_Image = new CK2::CKBYTE[GetImageSize()];
		}
		~VxImageDescEx() {
			delete[] m_Image;
		}
		LIBCMO_DISABLE_COPY_MOVE(VxImageDescEx);

		CK2::CKDWORD GetImageSize() const {
			return static_cast<CK2::CKDWORD>(sizeof(uint32_t) * m_Width * m_Height);
		}
		const CK2::CKBYTE* GetImage() const {
			return m_Image;
		}
		CK2::CKBYTE* GetMutableImage() {
			return m_Image;
		}

		CK2::CKDWORD GetPixelCount() const {
			return static_cast<CK2::CKDWORD>(m_Width * m_Height);
		}
		const CK2::CKDWORD* GetPixels() const {
			return reinterpret_cast<CK2::CKDWORD*>(m_Image);
		}
		CK2::CKDWORD* GetMutablePixels() {
			return reinterpret_cast<CK2::CKDWORD*>(m_Image);
		}

		CK2::CKDWORD GetWidth() const {
			return m_Width;
		}
		CK2::CKDWORD GetHeight() const {
			return m_Height;
		}

		bool IsHWEqual(const VxImageDescEx& rhs) const {
			return (m_Width == rhs.m_Width && m_Height == rhs.m_Height);
		}
	//	bool IsMaskEqual(const VxImageDescEx& rhs) const {
	//		return (
	//			m_RedMask == rhs.m_RedMask &&
	//			m_GreenMask == rhs.m_GreenMask &&
	//			m_BlueMask == rhs.m_BlueMask &&
	//			m_AlphaMask == rhs.m_AlphaMask
	//			);
	//	}

	//public:
	//	CK2::CKDWORD m_RedMask;
	//	CK2::CKDWORD m_GreenMask;
	//	CK2::CKDWORD m_BlueMask;
	//	CK2::CKDWORD m_AlphaMask;

	protected:
		CK2::CKDWORD m_Width; /**< Width in pixel of the image */
		CK2::CKDWORD m_Height; /**< Height in pixel of the image */
		CK2::CKBYTE* m_Image; /**< A pointer point to current processing image */
	};

}
