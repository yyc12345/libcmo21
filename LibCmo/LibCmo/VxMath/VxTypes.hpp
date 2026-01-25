#pragma once

#include "../VTUtils.hpp"
#include "../CK2/CKTypes.hpp"
#include <yycc/macro/class_copy_move.hpp>
#include "VxEnums.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <cinttypes>
#include <compare>

/**
 * @brief The VxMath part of LibCmo.
 * These classes are prefixed with Vx in original Virtools SDK.
*/
namespace LibCmo::VxMath {

	// ========== Type Definition ==========

	// ========== Class List ==========
	// Important classes

	class VxMemoryMappedFile;

	// ========== Vector-like Definition ==========

	/**
	 * @brief The representation of a Vector in 2 dimensions.
	 * @remarks In original Virtools SDK, it was named Vx2DVector.
	 * @see VxVector3
	*/
	struct VxVector2 {
		CKFLOAT x, y;
		VxVector2();
		VxVector2(CKFLOAT _x, CKFLOAT _y);
		YYCC_DEFAULT_COPY_MOVE(VxVector2)
		CKFLOAT& operator[](size_t i);
		const CKFLOAT& operator[](size_t i) const;
		bool operator==(const VxVector2& rhs) const;
		std::partial_ordering operator<=>(const VxVector2& rhs) const;
		VxVector2 operator+() const;
		VxVector2 operator-() const;
		VxVector2& operator+=(const VxVector2& rhs);
		friend VxVector2 operator+(const VxVector2& lhs, const VxVector2& rhs);
		VxVector2& operator-=(const VxVector2& rhs);
		friend VxVector2 operator-(const VxVector2& lhs, const VxVector2& rhs);
		VxVector2& operator*=(CKFLOAT rhs);
		friend VxVector2 operator*(const VxVector2& lhs, CKFLOAT rhs);
		friend VxVector2 operator*(CKFLOAT lhs, const VxVector2& rhs);
		friend CKFLOAT operator*(const VxVector2& lhs, const VxVector2& rhs);
		VxVector2& operator/=(CKFLOAT rhs);
		friend VxVector2 operator/(const VxVector2& lhs, CKFLOAT rhs);
		CKFLOAT SquaredLength() const;
		CKFLOAT Length() const;
		void Normalized();
		VxVector2 Normalize() const;

		/* ===== BEGIN USER CUSTOM ===== */
		/* =====  END  USER CUSTOM ===== */
	};

	/**
	 * @brief The representation of a Vector in 3 dimensions
	 * @remarks In original Virtools SDK, it was named VxVector.
	*/
	struct VxVector3 {
		CKFLOAT x, y, z;
		VxVector3();
		VxVector3(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z);
		YYCC_DEFAULT_COPY_MOVE(VxVector3)
		CKFLOAT& operator[](size_t i);
		const CKFLOAT& operator[](size_t i) const;
		bool operator==(const VxVector3& rhs) const;
		std::partial_ordering operator<=>(const VxVector3& rhs) const;
		VxVector3 operator+() const;
		VxVector3 operator-() const;
		VxVector3& operator+=(const VxVector3& rhs);
		friend VxVector3 operator+(const VxVector3& lhs, const VxVector3& rhs);
		VxVector3& operator-=(const VxVector3& rhs);
		friend VxVector3 operator-(const VxVector3& lhs, const VxVector3& rhs);
		VxVector3& operator*=(CKFLOAT rhs);
		friend VxVector3 operator*(const VxVector3& lhs, CKFLOAT rhs);
		friend VxVector3 operator*(CKFLOAT lhs, const VxVector3& rhs);
		friend CKFLOAT operator*(const VxVector3& lhs, const VxVector3& rhs);
		VxVector3& operator/=(CKFLOAT rhs);
		friend VxVector3 operator/(const VxVector3& lhs, CKFLOAT rhs);
		CKFLOAT SquaredLength() const;
		CKFLOAT Length() const;
		void Normalized();
		VxVector3 Normalize() const;

		/* ===== BEGIN USER CUSTOM ===== */
		/* =====  END  USER CUSTOM ===== */
	};

	/**
	 * @brief The representation of a Vector of 4 elements (x, y, z, w)
	 * @details
	 * VxVector4 is used for 3D transformation when the w component is used for perspective information.
	 * Most of the methods available for a VxVector3 are also implemented for the VxVector4.
	 * @remarks In original Virtools SDK, it was named VxVector4. Not changed.
	 * @see VxVector3
	*/
	struct VxVector4 {
		CKFLOAT x, y, z, w;
		VxVector4();
		VxVector4(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z, CKFLOAT _w);
		YYCC_DEFAULT_COPY_MOVE(VxVector4)
		CKFLOAT& operator[](size_t i);
		const CKFLOAT& operator[](size_t i) const;
		bool operator==(const VxVector4& rhs) const;
		std::partial_ordering operator<=>(const VxVector4& rhs) const;
		VxVector4 operator+() const;
		VxVector4 operator-() const;
		VxVector4& operator+=(const VxVector4& rhs);
		friend VxVector4 operator+(const VxVector4& lhs, const VxVector4& rhs);
		VxVector4& operator-=(const VxVector4& rhs);
		friend VxVector4 operator-(const VxVector4& lhs, const VxVector4& rhs);
		VxVector4& operator*=(CKFLOAT rhs);
		friend VxVector4 operator*(const VxVector4& lhs, CKFLOAT rhs);
		friend VxVector4 operator*(CKFLOAT lhs, const VxVector4& rhs);
		friend CKFLOAT operator*(const VxVector4& lhs, const VxVector4& rhs);
		VxVector4& operator/=(CKFLOAT rhs);
		friend VxVector4 operator/(const VxVector4& lhs, CKFLOAT rhs);
		CKFLOAT SquaredLength() const;
		CKFLOAT Length() const;
		void Normalized();
		VxVector4 Normalize() const;

		/* ===== BEGIN USER CUSTOM ===== */
		/* =====  END  USER CUSTOM ===== */
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
		VxQuaternion();
		VxQuaternion(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z, CKFLOAT _w);
		YYCC_DEFAULT_COPY_MOVE(VxQuaternion)
		CKFLOAT& operator[](size_t i);
		const CKFLOAT& operator[](size_t i) const;
		bool operator==(const VxQuaternion& rhs) const;
		std::partial_ordering operator<=>(const VxQuaternion& rhs) const;

		/* ===== BEGIN USER CUSTOM ===== */
		/* =====  END  USER CUSTOM ===== */
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
		VxColor();
		VxColor(CKFLOAT _r, CKFLOAT _g, CKFLOAT _b, CKFLOAT _a);
		YYCC_DEFAULT_COPY_MOVE(VxColor)
		CKFLOAT& operator[](size_t i);
		const CKFLOAT& operator[](size_t i) const;
		bool operator==(const VxColor& rhs) const;
		std::partial_ordering operator<=>(const VxColor& rhs) const;

		/* ===== BEGIN USER CUSTOM ===== */
		VxColor(CKDWORD argb);
		VxColor(CKFLOAT _r, CKFLOAT _g, CKFLOAT _b);
		void FromARGB(CKDWORD argb);
		CKDWORD ToARGB() const;
		void Regulate();
		/* =====  END  USER CUSTOM ===== */
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
		VxMatrix();
		VxMatrix(CKFLOAT m[4][4]);
		YYCC_DEFAULT_COPY_MOVE(VxMatrix)
		VxVector4& operator[](size_t i);
		const VxVector4& operator[](size_t i) const;
		bool operator==(const VxMatrix& rhs) const;
		std::partial_ordering operator<=>(const VxMatrix& rhs) const;

		/* ===== BEGIN USER CUSTOM ===== */
		void Clear();
		void SetIdentity();
		/**
		 * @brief Constructs a perspective projection matrix.
		 * @param[in] Fov Field of View.
		 * @param[in] Aspect Aspect ratio (Width/height)
		 * @param[in] Near_plane Distance of the near clipping plane.
		 * @param[in] Far_plane Distance of the far clipping plane.
		 * @remarks Sets Mat to
		 *
		 * 		A	=	Cos(Fov/2)/Sin(Fov/2)
		 * 		F	=	Far_plane
		 * 		N	=	Near_plane
		 *
		 * 				[ A			0			0			0]
		 * 				[ 0			A*Aspect	0			0]
		 * 		  MAT=	[ 0			0			F/F-N		1]
		 * 				[ 0			0			-F.N/F-N	0]
		 *
		 * @see PerspectiveRect, Orthographic, OrthographicRect
		 */
		void Perspective(CKFLOAT Fov, CKFLOAT Aspect, CKFLOAT Near_plane, CKFLOAT Far_plane);
		/**
		 * @brief Constructs a perspective projection matrix given a view rectangle.
		 * @param[in] Left Left clipping plane value.
		 * @param[in] Right Right clipping plane value.
		 * @param[in] Top top clipping plane value.
		 * @param[in] Bottom bottom clipping plane value.
		 * @param[in] Near_plane Distance of the near clipping plane.
		 * @param[in] Far_plane Distance of the far clipping plane.
		 * @remarks Sets Mat to
		 *
		 * 		F	=	Far_plane
		 * 		N	=	Near_plane
		 * 		R	=	Right
		 * 		L	=	Left
		 * 		T	=	Top
		 * 		B	=	Bottom
		 *
		 * 				[ 2/(R-L)		0			0			0]
		 * 				[ 0				-2/(T-B)	0			0]
		 * 		  MAT =	[ 0				0			1/F-N		0]
		 * 				[ -(L+R)/(R-L)	(T+B)/(T-B)	-N/F-N		1]
		 *
		 * @see Perspective, Orthographic, OrthographicRect
		 */
		void PerspectiveRect(CKFLOAT Left, CKFLOAT Right, CKFLOAT Top, CKFLOAT Bottom, CKFLOAT Near_plane, CKFLOAT Far_plane);
		/**
		 * @brief Constructs a orthographic projection matrix.
		 * @param[in] Zoom Zoom factor.
		 * @param[in] Aspect Aspect ratio (Width/height)
		 * @param[in] Near_plane Distance of the near clipping plane.
		 * @param[in] Far_plane Distance of the far clipping plane.
		 * @remarks Sets Mat to
		 *
		 * 		F	=	Far_plane
		 * 		N	=	Near_plane
		 *
		 * 					[ Zoom		0			0			0]
		 * 					[ 0			Zoom*Aspect	0			0]
		 * 			MAT =	[ 0			0			1/F-N		0]
		 * 					[ 0			0			-N/F-N		1]
		 *
		 * @see Perspective, OrthographicRect
		 */
		void Orthographic(CKFLOAT Zoom, CKFLOAT Aspect, CKFLOAT Near_plane, CKFLOAT Far_plane);
		/**
		 * @brief Constructs a orthographic projection matrix.
		 * @param[in] Left Left clipping plane value.
		 * @param[in] Right Right clipping plane value.
		 * @param[in] Top top clipping plane value.
		 * @param[in] Bottom bottom clipping plane value.
		 * @param[in] Near_plane Distance of the near clipping plane.
		 * @param[in] Far_plane Distance of the far clipping plane.
		 * @remarks Sets Mat to
		 *
		 *		F	=	Far_plane
		 *		N	=	Near_plane
		 *		R	=	Right
		 *		L	=	Left
		 *		T	=	Top
		 *		B	=	Bottom
		 *
		 * 				[ 2/(R-L)		0			0			0]
		 * 				[ 0				-2/(T-B)	0			0]
		 * 		  MAT =	[ 0				0			1/F-N		0]
		 * 				[ -(L+R)/(R-L)	(T+B)/(T-B)	-N/F-N		1]
		 *
		 * @see Perspective, Orthographic
		 */
		void OrthographicRect(CKFLOAT Left, CKFLOAT Right, CKFLOAT Top, CKFLOAT Bottom, CKFLOAT Near_plane, CKFLOAT Far_plane);
		/* =====  END  USER CUSTOM ===== */
	};

	// ========== Misc ==========

	/**
	 * @brief Structure for storage of strided data.
	 * @tparam _Ty The data pointer type this class stored.
	*/
	template<class T>
	    requires std::is_pointer_v<T>
	class VxStridedData {
	public:
		/**
		 * @brief Create a new viewer for strided data.
		 * @param[in] ptr The pointer to first data. nullptr is allowed but not suggested.
		 * @param[in] stride The stride between 2 adjacent data.
		 * If you set stride to the size of underlying type of pointer,
		 * this class will degenerate to the visitor of a plain data array.
		*/
		VxStridedData(T ptr, CKDWORD stride) : m_Ptr(reinterpret_cast<CKBYTE*>(ptr)), m_Stride(stride) {}
		~VxStridedData() {}

		/**
		 * @brief Visitr n-th data with given stride.
		 * @param[in] idx N-th
		 * @return The pointer to n-th data.
		*/
		T operator[](size_t idx) { return reinterpret_cast<T>(m_Ptr + (m_Stride * idx)); }

	private:
		CKBYTE* m_Ptr;    /**< The pointer to first data. */
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
		static constexpr CKDWORD FACTOR_SIZE = 1u;              /**< Single color factor (one of ARGB) occpied size in byte. */
		static constexpr CKDWORD PIXEL_SIZE = FACTOR_SIZE * 4u; /**< Single pixel occpied size in byte. */
	public:
		/**
		 * @brief Create a blank (invalid) image.
		*/
		VxImageDescEx() : m_Width(0), m_Height(0), m_Image(nullptr) {}
		/**
		 * @brief Create a image with given width and height.
		 * @param[in] width The width of image.
		 * @param[in] height The height of image.
		*/
		VxImageDescEx(CKDWORD width, CKDWORD height) : m_Width(width), m_Height(height), m_Image(nullptr) { CreateImage(width, height); }
		VxImageDescEx(const VxImageDescEx& rhs) : m_Width(rhs.m_Width), m_Height(rhs.m_Height), m_Image(nullptr) {
			// copy image
			if (rhs.m_Image != nullptr) {
				CreateImage(rhs.m_Width, rhs.m_Height, rhs.m_Image);
			}
		}
		VxImageDescEx(VxImageDescEx&& rhs) noexcept : m_Width(rhs.m_Width), m_Height(rhs.m_Height), m_Image(rhs.m_Image) {
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
		VxImageDescEx& operator=(VxImageDescEx&& rhs) noexcept {
			FreeImage();

			m_Height = rhs.m_Height;
			m_Width = rhs.m_Width;
			m_Image = rhs.m_Image;
			rhs.m_Height = 0;
			rhs.m_Width = 0;
			rhs.m_Image = nullptr;

			return *this;
		}
		~VxImageDescEx() { FreeImage(); }

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
		CKDWORD GetImageSize() const { return static_cast<CKDWORD>(PIXEL_SIZE * m_Width * m_Height); }
		/**
		 * @brief Get a constant pointer to image in memory unit for viewing.
		 * @return A constant pointer to image in memory unit.
		*/
		const CKBYTE* GetImage() const { return m_Image; }
		/**
		 * @brief Get a mutable pointer to image in memory unit for modifying.
		 * @return A mutable pointer to image in memory uint.
		*/
		CKBYTE* GetMutableImage() { return m_Image; }

		/**
		 * @brief Get the full count of pixel in image.
		 * @return The count of image. Basically it is image width * height.
		*/
		CKDWORD GetPixelCount() const { return static_cast<CKDWORD>(m_Width * m_Height); }
		/**
		 * @brief Get a constant pointer to image in pixel unit for viewing.
		 * @return A constant pointer to image in pixel unit.
		*/
		const CKDWORD* GetPixels() const { return reinterpret_cast<CKDWORD*>(m_Image); }
		/**
		 * @brief Get a mutable pointer to image in pixel uint for modifying.
		 * @return A mutable pointer to image in pixel uint.
		*/
		CKDWORD* GetMutablePixels() { return reinterpret_cast<CKDWORD*>(m_Image); }

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
		bool IsValid() const { return (m_Width != 0u && m_Height != 0u && m_Image != nullptr); }
		/**
		 * @brief Check whether the width and height of this image are equal to another image.
		 * @param[in] rhs Another image for comparing.
		 * @return True if their width and height are equal, otherwise false.
		*/
		bool IsHWEqual(const VxImageDescEx& rhs) const { return (m_Width == rhs.m_Width && m_Height == rhs.m_Height); }
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
		CKDWORD m_Width;  /**< Width in pixel of the image */
		CKDWORD m_Height; /**< Height in pixel of the image */
		CKBYTE* m_Image;  /**< A pointer points to current image in memory */
	};

	// ========== Patch Section ==========

	/**
	 * @brief The patch namespace for VxVector-like classes
	 * @details This namespace provides VxVector-like classes member functions which presented in original Virtools SDK.
	 * These functions are put in public namespace in original Virtools SDK.
	 * We just organise them into an unique namespace.
	*/
	namespace NSVxVector {

		/**
		 * @brief Dot product 2 2d vectors.
		 * @param[in] lhs The left side vector of dot product symbol.
		 * @param[in] rhs The right side vector of dot product symbol.
		 * @return The float pointing result of dot product.
		*/
		CKFLOAT DotProduct(const VxVector2& lhs, const VxVector2& rhs);
		/**
		 * @brief Dot product 2 3d vectors.
		 * @param[in] lhs The left side vector of dot product symbol.
		 * @param[in] rhs The right side vector of dot product symbol.
		 * @return The float pointing result of dot product.
		*/
		CKFLOAT DotProduct(const VxVector3& lhs, const VxVector3& rhs);
		/**
		 * @brief Dot product 2 4d vectors.
		 * @param[in] lhs The left side vector of dot product symbol.
		 * @param[in] rhs The right side vector of dot product symbol.
		 * @return The float pointing result of dot product.
		*/
		CKFLOAT DotProduct(const VxVector4& lhs, const VxVector4& rhs);

		/**
		 * @brief Cross product 2 3d vectors.
		 * @param[in] lhs The left side vector of cross product symbol.
		 * @param[in] rhs The right side vector of cross product symbol.
		 * @return The 3d vector result of cross product.
		*/
		VxVector3 CrossProduct(const VxVector3& lhs, const VxVector3& rhs);

		/**
		 * @brief Set all factor in vector to its absolute value.
		 * @param[in,out] lhs The vector for processing.
		 * @remarks This function is rarely used.
		 * Please note this function is not calculate the absolute value of vector.
		*/
		void Absolute(VxVector3& lhs);

	} // namespace NSVxVector

	/**
	 * @brief The patch namespace for VxMatrix classes
	 * @details Like NXVxVector, these functions located in this namespace
	 * are exposed in public namespace in original Virtools SDK.
	 * And I re-organise them in there.
	*/
	namespace NSVxMatrix {

		//void Vx3DMatrixIdentity(VxMatrix& Mat);

		//void Vx3DMultiplyMatrixVector(VxVector *ResultVector,const VxMatrix& Mat,const VxVector *Vector);
		//void Vx3DMultiplyMatrixVectorMany(VxVector *ResultVectors,const VxMatrix& Mat,const  VxVector *Vectors,int count,int stride);
		//void Vx3DMultiplyMatrixVector4(VxVector4 *ResultVector,const VxMatrix& Mat,const  VxVector4 *Vector);
		//void Vx3DMultiplyMatrixVector4(VxVector4 *ResultVector,const VxMatrix& Mat,const  VxVector *Vector); // w=1
		//void Vx3DRotateVector(VxVector *ResultVector,const VxMatrix& Mat,const VxVector *Vector);
		//void Vx3DRotateVectorMany(VxVector *ResultVector,const VxMatrix& Mat,const VxVector *Vector,int count,int stride);
		//void Vx3DMultiplyMatrix(VxMatrix& ResultMat,const VxMatrix& MatA,const VxMatrix& MatB);
		//void Vx3DMultiplyMatrix4(VxMatrix& ResultMat,const VxMatrix& MatA,const VxMatrix& MatB);
		//void Vx3DInverseMatrix(VxMatrix& InverseMat,const VxMatrix& Mat);
		//float Vx3DMatrixDeterminant(const  VxMatrix& Mat);
		//void Vx3DMatrixFromRotation(VxMatrix& ResultMat,const VxVector& Vector, float Angle);
		//void Vx3DMatrixFromRotationAndOrigin(VxMatrix& ResultMat,const VxVector& Vector,const VxVector& Origin, float Angle);
		//void Vx3DMatrixFromEulerAngles(VxMatrix& Mat,float eax,float eay,float eaz);
		//void Vx3DMatrixToEulerAngles(const VxMatrix& Mat,float *eax,float* eay,float* eaz);
		//void Vx3DInterpolateMatrix(float step,VxMatrix& Res,const VxMatrix& A, const VxMatrix& B);
		//void Vx3DInterpolateMatrixNoScale(float step,VxMatrix& Res,const VxMatrix& A, const VxMatrix& B);

		//void Vx3DMultiplyMatrixVectorStrided(VxStridedData* Dest,VxStridedData* Src,const VxMatrix& Mat,int count);
		//void Vx3DMultiplyMatrixVector4Strided(VxStridedData* Dest,VxStridedData* Src,const VxMatrix& Mat,int count);
		//void Vx3DRotateVectorStrided(VxStridedData* Dest,VxStridedData* Src,const VxMatrix& Mat,int count);

		//void Vx3DTransposeMatrix(VxMatrix& Result,const VxMatrix& A);

		//void Vx3DDecomposeMatrix(const VxMatrix& A, VxQuaternion &Quat,VxVector &Pos,VxVector &Scale);
		//float Vx3DDecomposeMatrixTotal(const VxMatrix& A, VxQuaternion &Quat,VxVector &Pos,VxVector &Scale,VxQuaternion &URot);
		//float Vx3DDecomposeMatrixTotalPtr(const VxMatrix& A, VxQuaternion* Quat,VxVector* Pos,VxVector* Scale,VxQuaternion* URot);

		//void VxInverseProject(const VxMatrix& iProjection, const Vx2DVector& i2D, const float iZ, VxVector* o3D);

	} // namespace NSVxMatrix

} // namespace LibCmo::VxMath
