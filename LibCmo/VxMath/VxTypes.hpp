#pragma once

#include "../VTUtils.hpp"
#include "../CK2/CKTypes.hpp"
#include "VxEnums.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <cinttypes>
#include <cmath>

/**
 * @brief The VxMath part of LibCmo.
 * These classes are prefixed with Vx in original Virtools SDK.
*/
namespace LibCmo::VxMath {

	// ========== Type Definition ==========


	// ========== Class List ==========
	// Important classes

	class VxMemoryMappedFile;

	// Misc

	/**
	 * @brief The representation of a Vector in 2 dimensions.
	 * @see VxVector3
	*/
	struct VxVector2 {
		CKFLOAT x, y;
		VxVector2() : x(0.0f), y(0.0f) {}
		VxVector2(CKFLOAT _x, CKFLOAT _y) : x(_x), y(_y) {}
		YYCC_DEF_CLS_COPY_MOVE(VxVector2);
		CKFLOAT& operator[](size_t i) {
			switch (i) {
				case 0: return x;
				case 1: return y;
				default: throw LogicException("Invalid index for VxVector2::operator[].");
			}
		}
		const CKFLOAT& operator[](size_t i) const {
			switch (i) {
				case 0: return x;
				case 1: return y;
				default: throw LogicException("Invalid index for VxVector2::operator[].");
			}
		}
		VxVector2& operator+=(const VxVector2& rhs) {
			x += rhs.x;
			y += rhs.y;
			return *this;
		}
		friend VxVector2 operator+(const VxVector2& lhs, const VxVector2& rhs) {
			return VxVector2(lhs.x + rhs.x, lhs.y + rhs.y);
		}
		VxVector2& operator-=(const VxVector2& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}
		friend VxVector2 operator-(const VxVector2& lhs, const VxVector2& rhs) {
			return VxVector2(lhs.x - rhs.x, lhs.y - rhs.y);
		}
		VxVector2& operator*=(CKFLOAT rhs) {
			x *= rhs;
			y *= rhs;
			return *this;
		}
		friend VxVector2 operator*(const VxVector2& lhs, CKFLOAT rhs) {
			return VxVector2(lhs.x * rhs, lhs.y * rhs);
		}
		friend VxVector2 operator*(CKFLOAT lhs, const VxVector2& rhs) {
			return VxVector2(lhs * rhs.x, lhs * rhs.y);
		}
		friend CKFLOAT operator*(const VxVector2& lhs, const VxVector2& rhs) {
			return (lhs.x * rhs.x + lhs.y * rhs.y);
		}
		VxVector2& operator/=(CKFLOAT rhs) {
			if (rhs == 0.0f) return *this;
			x /= rhs;
			y /= rhs;
			return *this;
		}
		friend VxVector2 operator/(const VxVector2& lhs, CKFLOAT rhs) {
			if (rhs == 0.0f) return VxVector2(0.0f, 0.0f);
			return VxVector2(lhs.x / rhs, lhs.y / rhs);
		}
		bool operator==(const VxVector2& rhs) const {
			return (x == rhs.x && y == rhs.y);
		}
		auto operator<=>(const VxVector2& rhs) const {
			if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
			return y <=> rhs.y;
		}
		CKFLOAT SquaredLength() const {
			return (x * x + y * y);
		}
		CKFLOAT Length() const {
			return std::sqrt(SquaredLength());
		}
		void Normalized() {
			CKFLOAT len = Length();
			if (len == 0.0f) return;
			x /= len;
			y /= len;
		}
		VxVector2 Normalize() const {
			CKFLOAT len = Length();
			if (len == 0.0f) return VxVector2();
			return VxVector2(x / len, y / len);
		}
	};

	/**
	 * @brief The representation of a Vector in 3 dimensions
	*/
	struct VxVector3 {
		CKFLOAT x, y, z;
		VxVector3() : x(0.0f), y(0.0f), z(0.0f) {}
		VxVector3(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z) : x(_x), y(_y), z(_z) {}
		YYCC_DEF_CLS_COPY_MOVE(VxVector3);
		CKFLOAT& operator[](size_t i) {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				default: throw LogicException("Invalid index for VxVector3::operator[].");
			}
		}
		const CKFLOAT& operator[](size_t i) const {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				default: throw LogicException("Invalid index for VxVector3::operator[].");
			}
		}
		VxVector3& operator+=(const VxVector3& rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			return *this;
		}
		friend VxVector3 operator+(const VxVector3& lhs, const VxVector3& rhs) {
			return VxVector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
		}
		VxVector3& operator-=(const VxVector3& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			return *this;
		}
		friend VxVector3 operator-(const VxVector3& lhs, const VxVector3& rhs) {
			return VxVector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
		}
		VxVector3& operator*=(CKFLOAT rhs) {
			x *= rhs;
			y *= rhs;
			z *= rhs;
			return *this;
		}
		friend VxVector3 operator*(const VxVector3& lhs, CKFLOAT rhs) {
			return VxVector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
		}
		friend VxVector3 operator*(CKFLOAT lhs, const VxVector3& rhs) {
			return VxVector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
		}
		friend CKFLOAT operator*(const VxVector3& lhs, const VxVector3& rhs) {
			return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
		}
		VxVector3& operator/=(CKFLOAT rhs) {
			if (rhs == 0.0f) return *this;
			x /= rhs;
			y /= rhs;
			z /= rhs;
			return *this;
		}
		friend VxVector3 operator/(const VxVector3& lhs, CKFLOAT rhs) {
			if (rhs == 0.0f) return VxVector3(0.0f, 0.0f, 0.0f);
			return VxVector3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
		}
		bool operator==(const VxVector3& rhs) const {
			return (x == rhs.x && y == rhs.y && z == rhs.z);
		}
		auto operator<=>(const VxVector3& rhs) const {
			if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
			if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
			return z <=> rhs.z;
		}
		CKFLOAT SquaredLength() const {
			return (x * x + y * y + z * z);
		}
		CKFLOAT Length() const {
			return std::sqrt(SquaredLength());
		}
		void Normalized() {
			CKFLOAT len = Length();
			if (len == 0.0f) return;
			x /= len;
			y /= len;
			z /= len;
		}
		VxVector3 Normalize() const {
			CKFLOAT len = Length();
			if (len == 0.0f) return VxVector3();
			return VxVector3(x / len, y / len, z / len);
		}
	};

	/**
	 * @brief The representation of a Vector of 4 elements (x, y, z, w)
	 * @details
	 * VxVector4 is used for 3D transformation when the w component is used for perspective information.
	 * Most of the methods available for a VxVector3 are also implemented for the VxVector4.
	 * @see VxVector3
	*/
	struct VxVector4 {
		CKFLOAT x, y, z, w;
		VxVector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		VxVector4(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z, CKFLOAT _w) : x(_x), y(_y), z(_z), w(_w) {}
		YYCC_DEF_CLS_COPY_MOVE(VxVector4);
		CKFLOAT& operator[](size_t i) {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
				default: throw LogicException("Invalid index for VxVector4::operator[].");
			}
		}
		const CKFLOAT& operator[](size_t i) const {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
				default: throw LogicException("Invalid index for VxVector4::operator[].");
			}
		}
		VxVector4& operator+=(const VxVector4& rhs) {
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
			return *this;
		}
		friend VxVector4 operator+(const VxVector4& lhs, const VxVector4& rhs) {
			return VxVector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
		}
		VxVector4& operator-=(const VxVector4& rhs) {
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
			return *this;
		}
		friend VxVector4 operator-(const VxVector4& lhs, const VxVector4& rhs) {
			return VxVector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
		}
		VxVector4& operator*=(CKFLOAT rhs) {
			x *= rhs;
			y *= rhs;
			z *= rhs;
			w *= rhs;
			return *this;
		}
		friend VxVector4 operator*(const VxVector4& lhs, CKFLOAT rhs) {
			return VxVector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
		}
		friend VxVector4 operator*(CKFLOAT lhs, const VxVector4& rhs) {
			return VxVector4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
		}
		friend CKFLOAT operator*(const VxVector4& lhs, const VxVector4& rhs) {
			return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w);
		}
		VxVector4& operator/=(CKFLOAT rhs) {
			if (rhs == 0.0f) return *this;
			x /= rhs;
			y /= rhs;
			z /= rhs;
			w /= rhs;
			return *this;
		}
		friend VxVector4 operator/(const VxVector4& lhs, CKFLOAT rhs) {
			if (rhs == 0.0f) return VxVector4(0.0f, 0.0f, 0.0f, 0.0f);
			return VxVector4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
		}
		bool operator==(const VxVector4& rhs) const {
			return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}
		auto operator<=>(const VxVector4& rhs) const {
			if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
			if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
			if (auto cmp = z <=> rhs.z; cmp != 0) return cmp;
			return w <=> rhs.w;
		}
		CKFLOAT SquaredLength() const {
			return (x * x + y * y + z * z + w * w);
		}
		CKFLOAT Length() const {
			return std::sqrt(SquaredLength());
		}
		void Normalized() {
			CKFLOAT len = Length();
			if (len == 0.0f) return;
			x /= len;
			y /= len;
			z /= len;
			w /= len;
		}
		VxVector4 Normalize() const {
			CKFLOAT len = Length();
			if (len == 0.0f) return VxVector4();
			return VxVector4(x / len, y / len, z / len, w / len);
		}
	};

	/**
	 * @brief The representation of a quaternion.
	 * @details
	 * A Quaternion is defined by 4 floats and is used to represents an orientation in space.
	 * Its common usage is for interpolation between two orientations through the Slerp() method.
	 * 
	 * Quaternions can be converted to VxMatrix or Euler Angles.
	 * @see VxMatrix, VxVector3
	*/
	struct VxQuaternion {
		CKFLOAT x, y, z, w;
		VxQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {} // set your custom init.
		VxQuaternion(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z, CKFLOAT _w) : x(_x), y(_y), z(_z), w(_w) {}
		YYCC_DEF_CLS_COPY_MOVE(VxQuaternion);
		CKFLOAT& operator[](size_t i) {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
				default: throw LogicException("Invalid index for VxQuaternion::operator[].");
			}
		}
		const CKFLOAT& operator[](size_t i) const {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
				default: throw LogicException("Invalid index for VxQuaternion::operator[].");
			}
		}
		bool operator==(const VxQuaternion& rhs) const {
			return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
		}
		auto operator<=>(const VxQuaternion& rhs) const {
			if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
			if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
			if (auto cmp = z <=> rhs.z; cmp != 0) return cmp;
			return w <=> rhs.w;
		}
	};

	/**
	 * @brief The representation of a color through 4 floats.
	 * @details
	 * Structure describing a color through 4 floats for each component Red, Green, Blue and Alpha.
	 * And each factor should be clamped between \c 0.0f and \c 1.0f.
	 * 
	 * Most methods are used to construct a VxColor or to convert it to a 32 bit ARGB format.
	*/
	struct VxColor {
		CKFLOAT r, g, b, a;
		VxColor() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {} // set your custom init.
		VxColor(CKFLOAT _r, CKFLOAT _g, CKFLOAT _b, CKFLOAT _a) : r(_r), g(_g), b(_b), a(_a) {}
		VxColor(CKDWORD argb) { FromARGB(argb); }
		YYCC_DEF_CLS_COPY_MOVE(VxColor);
		CKFLOAT& operator[](size_t i) {
			switch (i) {
				case 0: return r;
				case 1: return g;
				case 2: return b;
				case 3: return a;
				default: throw LogicException("Invalid index for VxColor::operator[].");
			}
		}
		const CKFLOAT& operator[](size_t i) const {
			switch (i) {
				case 0: return r;
				case 1: return g;
				case 2: return b;
				case 3: return a;
				default: throw LogicException("Invalid index for VxColor::operator[].");
			}
		}
		bool operator==(const VxColor& rhs) const {
			return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
		}
		auto operator<=>(const VxColor& rhs) const {
			if (auto cmp = r <=> rhs.r; cmp != 0) return cmp;
			if (auto cmp = g <=> rhs.g; cmp != 0) return cmp;
			if (auto cmp = b <=> rhs.b; cmp != 0) return cmp;
			return a <=> rhs.a;
		}

		VxColor(CKFLOAT _r, CKFLOAT _g, CKFLOAT _b) : r(_r), g(_g), b(_b), a(1.0f) {}
		void FromARGB(CKDWORD argb) {
			a = ((argb & 0xFF000000) >> 24) / 255.0f;
			r = ((argb & 0x00FF0000) >> 16) / 255.0f;
			g = ((argb & 0x0000FF00) >> 8) / 255.0f;
			b = ((argb & 0x000000FF) >> 0) / 255.0f;
		}
		CKDWORD ToARGB() const {
			CKDWORD argb = 0;
			argb |= static_cast<CKDWORD>(a * 255.0f);
			argb <<= 8;
			argb |= static_cast<CKDWORD>(r * 255.0f);
			argb <<= 8;
			argb |= static_cast<CKDWORD>(g * 255.0f);
			argb <<= 8;
			argb |= static_cast<CKDWORD>(b * 255.0f);
			return argb;
		}
		void Regulate() {
			if (r > 1.0f) r = 1.0f;
			else if (r < 0.0f) r = 0.0f;
			if (g > 1.0f) g = 1.0f;
			else if (g < 0.0f) g = 0.0f;
			if (b > 1.0f) b = 1.0f;
			else if (b < 0.0f) b = 0.0f;
			if (a > 1.0f) a = 1.0f;
			else if (a < 0.0f) a = 0.0f;
		}
	};

	/**
	 * @brief The representation of 4x4 matrix.
	 * @details
	 * A 4x4 matrix is defined by 4x4 floats and is used to represents the transformation in space.
	 * @see VxVector4, VxQuaternion
	*/
	struct VxMatrix {
	private:
		CKFLOAT m_Data[4][4];
	public:
		VxMatrix() : m_Data() { ResetToIdentity(); }
		VxMatrix(CKFLOAT m[4][4]) : m_Data() { std::memcpy(m_Data, m, sizeof(m_Data)); }
		YYCC_DEF_CLS_COPY_MOVE(VxMatrix);
		void ResetToIdentity() {
			std::memset(m_Data, 0, sizeof(m_Data));
			m_Data[0][0] = m_Data[1][1] = m_Data[2][2] = m_Data[3][3] = 1.0f;
		}
		VxVector4& operator[](size_t i) {
			if (i >= 4) throw LogicException("Invalid index for VxMatrix::operator[].");
			return *(reinterpret_cast<VxVector4*>(m_Data) + i);
		}
		const VxVector4& operator[](size_t i) const {
			if (i >= 4) throw LogicException("Invalid index for VxMatrix::operator[].");
			return *(reinterpret_cast<const VxVector4*>(m_Data) + i);
		}
		bool operator==(const VxMatrix& rhs) const {
			return std::memcmp(m_Data, rhs.m_Data, sizeof(m_Data)) == 0;
		}
	};

	/**
	 * @brief Structure for storage of strided data.
	 * @tparam _Ty The data pointer type this class stored.
	*/
	template<class _Ty, std::enable_if_t<std::is_pointer_v<_Ty>, int> = 0>
	class VxStridedData {
	public:
		/**
		 * @brief Create a new viewer for strided data.
		 * @param[in] ptr The pointer to first data. nullptr is allowed but not suggested.
		 * @param[in] stride The stride between 2 adjacent data.
		 * If you set stride to the size of underlying type of pointer,
		 * this class will degenerate to the visitor of a plain data array.
		*/
		VxStridedData(_Ty ptr, CKDWORD stride) :
			m_Ptr(reinterpret_cast<CKBYTE*>(ptr)),
			m_Stride(stride) {}
		~VxStridedData() {}

		/**
		 * @brief Visitr n-th data with given stride.
		 * @param[in] idx N-th
		 * @return The pointer to n-th data.
		*/
		_Ty operator[](size_t idx) {
			return reinterpret_cast<_Ty>(m_Ptr + (m_Stride * idx));
		}

	private:
		CKBYTE* m_Ptr; /**< The pointer to first data. */
		CKDWORD m_Stride; /**< The stride between adjacent data. */
	};

	/**
	 * @brief The description of image.
	 * @details
	 * VxImageDescEx describe the height, width, and etc for image.
	 * Also it hold a pointer to raw image data.
	 * The image data must be 32bit ARGB8888 format.
	 * Thus the size of Image must be 4 * Width * Height.
	 * And the image buffer must be in B, G, R, A order because little endian.
	*/
	class VxImageDescEx {
	public:
		static constexpr CKDWORD ColorFactorSize = 1u; /**< Single color factor (one of ARGB) occpied size in byte. */
		static constexpr CKDWORD PixelSize = ColorFactorSize * 4u; /**< Single pixel occpied size in byte. */
	public:
		/**
		 * @brief Create a blank (invalid) image.
		*/
		VxImageDescEx() :
			m_Width(0), m_Height(0), m_Image(nullptr) {}
		/**
		 * @brief Create a image with given width and height.
		 * @param[in] width The width of image.
		 * @param[in] height The height of image.
		*/
		VxImageDescEx(CKDWORD width, CKDWORD height) :
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

		/**
		 * @brief Create image with given width and height
		 * @param[in] Width The width of image.
		 * @param[in] Height The height of image.
		 * @remarks
		 * \li There is no initialization (fill with zero) for this new created image.
		 * \li Old image will be free first before creating.
		*/
		void CreateImage(CKDWORD Width, CKDWORD Height) {
			FreeImage();
			m_Width = Width;
			m_Height = Height;
			m_Image = new CKBYTE[GetImageSize()];
		}
		/**
		 * @brief Create image with given width, height and data.
		 * @param[in] Width The width of image.
		 * @param[in] Height The height of image.
		 * @param[in] dataptr The pointer to image data.
		 * @warning
		 * If the data length pointed by given image pointer is shorter than this image occupied,
		 * an undefined behavior is raised.
		 * @remarks Old image will be free first before creating.
		*/
		void CreateImage(CKDWORD Width, CKDWORD Height, const void* dataptr) {
			CreateImage(Width, Height);
			std::memcpy(m_Image, dataptr, GetImageSize());
		}
		/**
		 * @brief Free current image. Reset this to invalid status.
		*/
		void FreeImage() {
			m_Width = 0;
			m_Height = 0;
			if (m_Image != nullptr) {
				delete[] m_Image;
				m_Image = nullptr;
			}
		}

		/**
		 * @brief Get the allocated memory size of image.
		 * @return The allocated memory size of image.
		 * Basically it is image width * height * (single pixel size).
		*/
		CKDWORD GetImageSize() const {
			return static_cast<CKDWORD>(PixelSize * m_Width * m_Height);
		}
		/**
		 * @brief Get a constant pointer to image in memory unit for viewing.
		 * @return A constant pointer to image in memory unit.
		*/
		const CKBYTE* GetImage() const {
			return m_Image;
		}
		/**
		 * @brief Get a mutable pointer to image in memory unit for modifying.
		 * @return A mutable pointer to image in memory uint.
		*/
		CKBYTE* GetMutableImage() {
			return m_Image;
		}

		/**
		 * @brief Get the full count of pixel in image.
		 * @return The count of image. Basically it is image width * height.
		*/
		CKDWORD GetPixelCount() const {
			return static_cast<CKDWORD>(m_Width * m_Height);
		}
		/**
		 * @brief Get a constant pointer to image in pixel unit for viewing.
		 * @return A constant pointer to image in pixel unit.
		*/
		const CKDWORD* GetPixels() const {
			return reinterpret_cast<CKDWORD*>(m_Image);
		}
		/**
		 * @brief Get a mutable pointer to image in pixel uint for modifying.
		 * @return A mutable pointer to image in pixel uint.
		*/
		CKDWORD* GetMutablePixels() {
			return reinterpret_cast<CKDWORD*>(m_Image);
		}

		/**
		 * @brief Get the width of this image in pixel.
		 * @return The width of this image in pixel.
		*/
		CKDWORD GetWidth() const { return m_Width; }
		/**
		 * @brief Get the height of this image in pixel.
		 * @return The height of this image in pixel.
		*/
		CKDWORD GetHeight() const { return m_Height; }

		/**
		 * @brief Check whether this image is valid image for using.
		 * @details If one of width and height is zero, or underlying image pointer, this image is invalid.
		 * @return True if it is, otherwise false.
		*/
		bool IsValid() const {
			return (m_Width != 0u && m_Height != 0u && m_Image != nullptr);
		}
		/**
		 * @brief Check whether the width and height of this image are equal to another image.
		 * @param[in] rhs Another image for comparing.
		 * @return True if their width and height are equal, otherwise false.
		*/
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
		//	CKDWORD m_RedMask;
		//	CKDWORD m_GreenMask;
		//	CKDWORD m_BlueMask;
		//	CKDWORD m_AlphaMask;

	protected:
		CKDWORD m_Width; /**< Width in pixel of the image */
		CKDWORD m_Height; /**< Height in pixel of the image */
		CKBYTE* m_Image; /**< A pointer points to current image in memory */
	};

}
