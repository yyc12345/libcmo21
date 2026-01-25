#include "VxTypes.hpp"
#include <cmath>

namespace LibCmo::VxMath {

#pragma region VxVector2

	VxVector2::VxVector2() : x(0.0f), y(0.0f) {}
	VxVector2::VxVector2(CKFLOAT _x, CKFLOAT _y) : x(_x), y(_y) {}
	CKFLOAT& VxVector2::operator[](size_t i) {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			default:
				throw LogicException("Invalid index for VxVector2::operator[].");
		}
	}
	const CKFLOAT& VxVector2::operator[](size_t i) const {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			default:
				throw LogicException("Invalid index for VxVector2::operator[].");
		}
	}
	bool VxVector2::operator==(const VxVector2& rhs) const {
		return (x == rhs.x && y == rhs.y);
	}
	std::partial_ordering VxVector2::operator<=>(const VxVector2& rhs) const {
		if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
		return y <=> rhs.y;
	}
	VxVector2 VxVector2::operator+() const {
		return *this;
	}
	VxVector2 VxVector2::operator-() const {
		return VxVector2(-x, -y);
	}
	VxVector2& VxVector2::operator+=(const VxVector2& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	VxVector2 operator+(const VxVector2& lhs, const VxVector2& rhs) {
		return VxVector2(lhs.x + rhs.x, lhs.y + rhs.y);
	}
	VxVector2& VxVector2::operator-=(const VxVector2& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}
	VxVector2 operator-(const VxVector2& lhs, const VxVector2& rhs) {
		return VxVector2(lhs.x - rhs.x, lhs.y - rhs.y);
	}
	VxVector2& VxVector2::operator*=(CKFLOAT rhs) {
		x *= rhs;
		y *= rhs;
		return *this;
	}
	VxVector2 operator*(const VxVector2& lhs, CKFLOAT rhs) {
		return VxVector2(lhs.x * rhs, lhs.y * rhs);
	}
	VxVector2 operator*(CKFLOAT lhs, const VxVector2& rhs) {
		return VxVector2(lhs * rhs.x, lhs * rhs.y);
	}
	CKFLOAT operator*(const VxVector2& lhs, const VxVector2& rhs) {
		return (lhs.x * rhs.x + lhs.y * rhs.y);
	}
	VxVector2& VxVector2::operator/=(CKFLOAT rhs) {
		if (rhs == 0.0f) return *this;
		x /= rhs;
		y /= rhs;
		return *this;
	}
	VxVector2 operator/(const VxVector2& lhs, CKFLOAT rhs) {
		if (rhs == 0.0f) return VxVector2();
		else return VxVector2(lhs.x / rhs, lhs.y / rhs);
	}
	CKFLOAT VxVector2::SquaredLength() const {
		return (x * x + y * y);
	}
	CKFLOAT VxVector2::Length() const {
		return std::sqrt(SquaredLength());
	}
	void VxVector2::Normalized() {
		CKFLOAT len = Length();
		if (len == 0.0f) return;
		x /= len;
		y /= len;
	}
	VxVector2 VxVector2::Normalize() const {
		CKFLOAT len = Length();
		if (len == 0.0f) return VxVector2();
		else return VxVector2(x / len, y / len);
	}

	/* ===== BEGIN USER CUSTOM ===== */
	/* =====  END  USER CUSTOM ===== */

#pragma endregion

#pragma region VxVector3

	VxVector3::VxVector3() : x(0.0f), y(0.0f), z(0.0f) {}
	VxVector3::VxVector3(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z) : x(_x), y(_y), z(_z) {}
	CKFLOAT& VxVector3::operator[](size_t i) {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				throw LogicException("Invalid index for VxVector3::operator[].");
		}
	}
	const CKFLOAT& VxVector3::operator[](size_t i) const {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				throw LogicException("Invalid index for VxVector3::operator[].");
		}
	}
	bool VxVector3::operator==(const VxVector3& rhs) const {
		return (x == rhs.x && y == rhs.y && z == rhs.z);
	}
	std::partial_ordering VxVector3::operator<=>(const VxVector3& rhs) const {
		if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
		if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
		return z <=> rhs.z;
	}
	VxVector3 VxVector3::operator+() const {
		return *this;
	}
	VxVector3 VxVector3::operator-() const {
		return VxVector3(-x, -y, -z);
	}
	VxVector3& VxVector3::operator+=(const VxVector3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}
	VxVector3 operator+(const VxVector3& lhs, const VxVector3& rhs) {
		return VxVector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}
	VxVector3& VxVector3::operator-=(const VxVector3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}
	VxVector3 operator-(const VxVector3& lhs, const VxVector3& rhs) {
		return VxVector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}
	VxVector3& VxVector3::operator*=(CKFLOAT rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}
	VxVector3 operator*(const VxVector3& lhs, CKFLOAT rhs) {
		return VxVector3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
	}
	VxVector3 operator*(CKFLOAT lhs, const VxVector3& rhs) {
		return VxVector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
	}
	CKFLOAT operator*(const VxVector3& lhs, const VxVector3& rhs) {
		return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
	}
	VxVector3& VxVector3::operator/=(CKFLOAT rhs) {
		if (rhs == 0.0f) return *this;
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}
	VxVector3 operator/(const VxVector3& lhs, CKFLOAT rhs) {
		if (rhs == 0.0f) return VxVector3();
		else return VxVector3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
	}
	CKFLOAT VxVector3::SquaredLength() const {
		return (x * x + y * y + z * z);
	}
	CKFLOAT VxVector3::Length() const {
		return std::sqrt(SquaredLength());
	}
	void VxVector3::Normalized() {
		CKFLOAT len = Length();
		if (len == 0.0f) return;
		x /= len;
		y /= len;
		z /= len;
	}
	VxVector3 VxVector3::Normalize() const {
		CKFLOAT len = Length();
		if (len == 0.0f) return VxVector3();
		else return VxVector3(x / len, y / len, z / len);
	}

	/* ===== BEGIN USER CUSTOM ===== */
	/* =====  END  USER CUSTOM ===== */

#pragma endregion

#pragma region VxVector4

	VxVector4::VxVector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	VxVector4::VxVector4(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z, CKFLOAT _w) : x(_x), y(_y), z(_z), w(_w) {}
	CKFLOAT& VxVector4::operator[](size_t i) {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				throw LogicException("Invalid index for VxVector4::operator[].");
		}
	}
	const CKFLOAT& VxVector4::operator[](size_t i) const {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				throw LogicException("Invalid index for VxVector4::operator[].");
		}
	}
	bool VxVector4::operator==(const VxVector4& rhs) const {
		return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
	}
	std::partial_ordering VxVector4::operator<=>(const VxVector4& rhs) const {
		if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
		if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
		if (auto cmp = z <=> rhs.z; cmp != 0) return cmp;
		return w <=> rhs.w;
	}
	VxVector4 VxVector4::operator+() const {
		return *this;
	}
	VxVector4 VxVector4::operator-() const {
		return VxVector4(-x, -y, -z, -w);
	}
	VxVector4& VxVector4::operator+=(const VxVector4& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}
	VxVector4 operator+(const VxVector4& lhs, const VxVector4& rhs) {
		return VxVector4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
	}
	VxVector4& VxVector4::operator-=(const VxVector4& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}
	VxVector4 operator-(const VxVector4& lhs, const VxVector4& rhs) {
		return VxVector4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
	}
	VxVector4& VxVector4::operator*=(CKFLOAT rhs) {
		x *= rhs;
		y *= rhs;
		z *= rhs;
		w *= rhs;
		return *this;
	}
	VxVector4 operator*(const VxVector4& lhs, CKFLOAT rhs) {
		return VxVector4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
	}
	VxVector4 operator*(CKFLOAT lhs, const VxVector4& rhs) {
		return VxVector4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
	}
	CKFLOAT operator*(const VxVector4& lhs, const VxVector4& rhs) {
		return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w);
	}
	VxVector4& VxVector4::operator/=(CKFLOAT rhs) {
		if (rhs == 0.0f) return *this;
		x /= rhs;
		y /= rhs;
		z /= rhs;
		w /= rhs;
		return *this;
	}
	VxVector4 operator/(const VxVector4& lhs, CKFLOAT rhs) {
		if (rhs == 0.0f) return VxVector4();
		else return VxVector4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
	}
	CKFLOAT VxVector4::SquaredLength() const {
		return (x * x + y * y + z * z + w * w);
	}
	CKFLOAT VxVector4::Length() const {
		return std::sqrt(SquaredLength());
	}
	void VxVector4::Normalized() {
		CKFLOAT len = Length();
		if (len == 0.0f) return;
		x /= len;
		y /= len;
		z /= len;
		w /= len;
	}
	VxVector4 VxVector4::Normalize() const {
		CKFLOAT len = Length();
		if (len == 0.0f) return VxVector4();
		else return VxVector4(x / len, y / len, z / len, w / len);
	}

	/* ===== BEGIN USER CUSTOM ===== */
	/* =====  END  USER CUSTOM ===== */

#pragma endregion

#pragma region VxQuaternion

	VxQuaternion::VxQuaternion() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {} // SET YOUR CUSTOM INIT
	VxQuaternion::VxQuaternion(CKFLOAT _x, CKFLOAT _y, CKFLOAT _z, CKFLOAT _w) : x(_x), y(_y), z(_z), w(_w) {}
	CKFLOAT& VxQuaternion::operator[](size_t i) {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				throw LogicException("Invalid index for VxQuaternion::operator[].");
		}
	}
	const CKFLOAT& VxQuaternion::operator[](size_t i) const {
		switch (i) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				throw LogicException("Invalid index for VxQuaternion::operator[].");
		}
	}
	bool VxQuaternion::operator==(const VxQuaternion& rhs) const {
		return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
	}
	std::partial_ordering VxQuaternion::operator<=>(const VxQuaternion& rhs) const {
		if (auto cmp = x <=> rhs.x; cmp != 0) return cmp;
		if (auto cmp = y <=> rhs.y; cmp != 0) return cmp;
		if (auto cmp = z <=> rhs.z; cmp != 0) return cmp;
		return w <=> rhs.w;
	}

	/* ===== BEGIN USER CUSTOM ===== */
	/* =====  END  USER CUSTOM ===== */

#pragma endregion

#pragma region VxColor

	VxColor::VxColor() : r(0.0f), g(0.0f), b(0.0f), a(0.0f) {} // SET YOUR CUSTOM INIT
	VxColor::VxColor(CKFLOAT _r, CKFLOAT _g, CKFLOAT _b, CKFLOAT _a) : r(_r), g(_g), b(_b), a(_a) {}
	CKFLOAT& VxColor::operator[](size_t i) {
		switch (i) {
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			case 3:
				return a;
			default:
				throw LogicException("Invalid index for VxColor::operator[].");
		}
	}
	const CKFLOAT& VxColor::operator[](size_t i) const {
		switch (i) {
			case 0:
				return r;
			case 1:
				return g;
			case 2:
				return b;
			case 3:
				return a;
			default:
				throw LogicException("Invalid index for VxColor::operator[].");
		}
	}
	bool VxColor::operator==(const VxColor& rhs) const {
		return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
	}
	std::partial_ordering VxColor::operator<=>(const VxColor& rhs) const {
		if (auto cmp = r <=> rhs.r; cmp != 0) return cmp;
		if (auto cmp = g <=> rhs.g; cmp != 0) return cmp;
		if (auto cmp = b <=> rhs.b; cmp != 0) return cmp;
		return a <=> rhs.a;
	}

	/* ===== BEGIN USER CUSTOM ===== */
	VxColor::VxColor(CKDWORD argb) {
		FromARGB(argb);
	}
	VxColor::VxColor(CKFLOAT _r, CKFLOAT _g, CKFLOAT _b) : r(_r), g(_g), b(_b), a(1.0f) {}
	void VxColor::FromARGB(CKDWORD argb) {
		a = ((argb & 0xFF000000) >> 24) / 255.0f;
		r = ((argb & 0x00FF0000) >> 16) / 255.0f;
		g = ((argb & 0x0000FF00) >> 8) / 255.0f;
		b = ((argb & 0x000000FF) >> 0) / 255.0f;
	}
	CKDWORD VxColor::ToARGB() const {
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
	void VxColor::Regulate() {
		if (r > 1.0f) r = 1.0f;
		else if (r < 0.0f) r = 0.0f;
		if (g > 1.0f) g = 1.0f;
		else if (g < 0.0f) g = 0.0f;
		if (b > 1.0f) b = 1.0f;
		else if (b < 0.0f) b = 0.0f;
		if (a > 1.0f) a = 1.0f;
		else if (a < 0.0f) a = 0.0f;
	}
	/* =====  END  USER CUSTOM ===== */

#pragma endregion

#pragma region VxMatrix

	VxMatrix::VxMatrix() : m_Data() {
		SetIdentity();
	}
	VxMatrix::VxMatrix(CKFLOAT m[4][4]) : m_Data() {
		std::memcpy(m_Data, m, sizeof(m_Data));
	}
	VxVector4& VxMatrix::operator[](size_t i) {
		if (i >= 4) throw LogicException("Invalid index for VxMatrix::operator[].");
		return *(reinterpret_cast<VxVector4*>(m_Data) + i);
	}
	const VxVector4& VxMatrix::operator[](size_t i) const {
		if (i >= 4) throw LogicException("Invalid index for VxMatrix::operator[].");
		return *(reinterpret_cast<const VxVector4*>(m_Data) + i);
	}
	bool VxMatrix::operator==(const VxMatrix& rhs) const {
		return ((*this)[0] == rhs[0] && (*this)[1] == rhs[1] && (*this)[2] == rhs[2] && (*this)[3] == rhs[3]);
	}
	std::partial_ordering VxMatrix::operator<=>(const VxMatrix& rhs) const {
		if (auto cmp = (*this)[0] <=> rhs[0]; cmp != 0) return cmp;
		if (auto cmp = (*this)[1] <=> rhs[1]; cmp != 0) return cmp;
		if (auto cmp = (*this)[2] <=> rhs[2]; cmp != 0) return cmp;
		return (*this)[3] <=> rhs[3];
	}

	/* ===== BEGIN USER CUSTOM ===== */
	void VxMatrix::Clear() {
		std::memset(m_Data, 0, sizeof(m_Data));
	}
	void VxMatrix::SetIdentity() {
		Clear();
		m_Data[0][0] = m_Data[1][1] = m_Data[2][2] = m_Data[3][3] = 1.0f;
	}
	void VxMatrix::Perspective(CKFLOAT Fov, CKFLOAT Aspect, CKFLOAT Near_plane, CKFLOAT Far_plane) {
		Clear();
		m_Data[0][0] = std::cos(Fov * 0.5f) / std::sin(Fov * 0.5f);
		m_Data[1][1] = m_Data[0][0] * Aspect;
		m_Data[2][2] = Far_plane / (Far_plane - Near_plane);
		m_Data[3][2] = -m_Data[2][2] * Near_plane;
		m_Data[2][3] = 1;
	}
	void VxMatrix::PerspectiveRect(CKFLOAT Left, CKFLOAT Right, CKFLOAT Top, CKFLOAT Bottom, CKFLOAT Near_plane, CKFLOAT Far_plane) {
		Clear();
		CKFLOAT RL = 1.0f / (Right - Left);
		CKFLOAT TB = 1.0f / (Top - Bottom);
		m_Data[0][0] = 2.0f * Near_plane * RL;
		m_Data[1][1] = 2.0f * Near_plane * TB;
		m_Data[2][0] = -(Right + Left) * RL;
		m_Data[2][1] = -(Top + Bottom) * TB;
		m_Data[2][2] = Far_plane / (Far_plane - Near_plane);
		m_Data[3][2] = -m_Data[2][2] * Near_plane;
		m_Data[2][3] = 1;
	}
	void VxMatrix::Orthographic(CKFLOAT Zoom, CKFLOAT Aspect, CKFLOAT Near_plane, CKFLOAT Far_plane) {
		Clear();
		CKFLOAT iz = 1.0f / (Far_plane - Near_plane);
		m_Data[0][0] = Zoom;
		m_Data[1][1] = Zoom * Aspect;
		m_Data[2][2] = iz;
		m_Data[3][2] = -Near_plane * iz;
		m_Data[3][3] = 1.0f;
	}
	void VxMatrix::OrthographicRect(CKFLOAT Left, CKFLOAT Right, CKFLOAT Top, CKFLOAT Bottom, CKFLOAT Near_plane, CKFLOAT Far_plane) {
		Clear();
		CKFLOAT ix = 1.0f / (Right - Left);
		CKFLOAT iy = 1.0f / (Top - Bottom);
		CKFLOAT iz = 1.0f / (Far_plane - Near_plane);
		m_Data[0][0] = 2.0f * ix;
		m_Data[1][1] = -2.0f * iy;
		m_Data[2][2] = iz;
		m_Data[3][0] = -(Left + Right) * ix;
		m_Data[3][1] = (Top + Bottom) * iy;
		m_Data[3][2] = -Near_plane * iz;
		m_Data[3][3] = 1.0f;
	}
	/* =====  END  USER CUSTOM ===== */

#pragma endregion

#pragma region Patched

	namespace NSVxVector {

		CKFLOAT DotProduct(const VxVector2& lhs, const VxVector2& rhs) {
			return lhs * rhs;
		}

		CKFLOAT DotProduct(const VxVector3& lhs, const VxVector3& rhs) {
			return lhs * rhs;
		}

		CKFLOAT DotProduct(const VxVector4& lhs, const VxVector4& rhs) {
			return lhs * rhs;
		}

		VxVector3 CrossProduct(const VxVector3& lhs, const VxVector3& rhs) {
			return VxVector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
		}

		void Absolute(VxVector3& lhs) {
			lhs.x = std::fabs(lhs.x);
			lhs.y = std::fabs(lhs.y);
			lhs.z = std::fabs(lhs.z);
		}

	} // namespace NSVxVector

	namespace NSVxMatrix {}

#pragma endregion

} // namespace LibCmo::VxMath
