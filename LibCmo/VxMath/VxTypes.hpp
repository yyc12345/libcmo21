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
	 * The image data must be 32bit ARGB8888 format.
	 * Thus the size of Image must be 4 * Width * Height.
	 * And the image buffer must is in B, G, R, A order because little endian.
	*/
	class VxImageDescEx {
	public:
		static constexpr CK2::CKDWORD ColorFactorSize = 1u;
		static constexpr CK2::CKDWORD PixelSize = ColorFactorSize * 4u;
	public:
		VxImageDescEx() :
			m_Width(0), m_Height(0), m_Image(nullptr) {}
		VxImageDescEx(CK2::CKDWORD width, CK2::CKDWORD height) :
			m_Width(width), m_Height(height), m_Image(nullptr) {
			CreateImage(width, height);
		}
		VxImageDescEx(const VxImageDescEx& rhs) :
			m_Width(rhs.m_Width), m_Height(rhs.m_Height), m_Image(nullptr) {
			// copy image
			if (rhs.m_Image != nullptr) {
				CreateImage(rhs.m_Width, rhs.m_Height, rhs.m_Image);
			}
		}
		VxImageDescEx(VxImageDescEx&& rhs) :
			m_Width(rhs.m_Width), m_Height(rhs.m_Height), m_Image(rhs.m_Image) {
			// move image
			rhs.m_Height = 0;
			rhs.m_Width = 0;
			rhs.m_Image = nullptr;
		}
		VxImageDescEx& operator=(const VxImageDescEx& rhs) {
			FreeImage();

			m_Width = rhs.m_Width;
			m_Height = rhs.m_Height;
			if (rhs.m_Image != nullptr) {
				CreateImage(rhs.m_Width, rhs.m_Height, rhs.m_Image);
			}

			return *this;
		}
		VxImageDescEx& operator=(VxImageDescEx&& rhs) {
			FreeImage();

			m_Height = rhs.m_Height;
			m_Width = rhs.m_Width;
			m_Image = rhs.m_Image;
			rhs.m_Height = 0;
			rhs.m_Width = 0;
			rhs.m_Image = nullptr;

			return *this;
		}
		~VxImageDescEx() {
			FreeImage();
		}

		void CreateImage(CK2::CKDWORD Width, CK2::CKDWORD Height) {
			FreeImage();
			m_Width = Width;
			m_Height = Height;
			m_Image = new CK2::CKBYTE[GetImageSize()];
		}
		void CreateImage(CK2::CKDWORD Width, CK2::CKDWORD Height, void* dataptr) {
			CreateImage(Width, Height);
			std::memcpy(m_Image, dataptr, GetImageSize());
		}
		void FreeImage() {
			m_Width = 0;
			m_Height = 0;
			if (m_Image != nullptr) {
				delete[] m_Image;
				m_Image = nullptr;
			}
		}

		CK2::CKDWORD GetImageSize() const {
			return static_cast<CK2::CKDWORD>(PixelSize * m_Width * m_Height);
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

		bool IsValid() const {
			return (
				m_Width != 0u &&
				m_Height != 0u &&
				m_Image != nullptr
			);
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
