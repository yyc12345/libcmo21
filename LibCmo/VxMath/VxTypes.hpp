#pragma once

#include <string>
#include <vector>
#include <cstring>
#include <cinttypes>
#include "../CK2/CKTypes.hpp"

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
	 * @brief Enhanced Image description
	 * @remark The VxImageDescEx holds basically an VxImageDesc with additionnal support for
	 * Colormap, Image pointer and is ready for future enhancements.
	*/
	struct VxImageDescEx {
		CK2::CKINT Size; ///< Size of the structure
		CK2::CKDWORD Flags; ///< Reserved for special formats (such as compressed ) 0 otherwise

		CK2::CKINT Width; ///< Width in pixel of the image
		CK2::CKINT Height; ///< Height in pixel of the image
		union {
			CK2::CKINT BytesPerLine; ///< Pitch (width in bytes) of the image
			CK2::CKINT TotalImageSize; ///< For compressed image (DXT1...) the total size of the image
		};
		CK2::CKINT BitsPerPixel; ///< Number of bits per pixel
		union {
			CK2::CKDWORD RedMask; ///< Mask for Red component
			CK2::CKDWORD BumpDuMask; ///< Mask for Bump Du component
		};
		union {
			CK2::CKDWORD GreenMask; ///< Mask for Green component	
			CK2::CKDWORD BumpDvMask; ///< Mask for Bump Dv component
		};
		union {
			CK2::CKDWORD BlueMask; ///< Mask for Blue component
			CK2::CKDWORD BumpLumMask; ///< Mask for Luminance component

		};
		CK2::CKDWORD AlphaMask; ///< Mask for Alpha component

		CK2::CKWORD BytesPerColorEntry; ///< ColorMap Stride
		CK2::CKWORD ColorMapEntries; ///< If other than 0 image is palletized

		CK2::CKBYTE* ColorMap; ///< Palette colors
		CK2::CKBYTE* Image; ///< Image
	};

}
