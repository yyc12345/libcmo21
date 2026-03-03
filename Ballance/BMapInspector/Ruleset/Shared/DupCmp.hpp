#pragma once
#include <VTAll.hpp>
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <yycc/macro/ptr_size_detector.hpp>
#include <utility>

#define BMAPINSP_L LibCmo
#define BMAPINSP_C LibCmo::CK2
#define BMAPINSP_O LibCmo::CK2::ObjImpls

namespace BMapInspector::Ruleset::Shared::DupCmp {

#pragma region Hash Combiner

	/**
	 * @brief FNV-1a Hash Combiner
	 */
	class Hasher {
	public:
		using ValueType = size_t;

	private:
#if defined(YYCC_PTRSIZE_32)
		static constexpr ValueType FNV_OFFSET_BASIS = 2166136261U;
		static constexpr ValueType FNV_PRIME = 16777619U;
#else
		static constexpr ValueType FNV_OFFSET_BASIS = 14695981039346656037ULL;
		static constexpr ValueType FNV_PRIME = 1099511628211ULL;
#endif

	public:
		Hasher();
		~Hasher();
		YYCC_DEFAULT_COPY_MOVE(Hasher)

	private:
		/**
		 * @brief Update this hash combiner with new hash.
		 * @param h 
		 */
		void combine(ValueType h);

	public:
		/**
		 * @brief Get final produced hash.
		 * @return 
		 */
		[[nodiscard]] ValueType finish() const noexcept { return this->seed; }
		template<typename T>
		void update(const T& v) {
			std::hash<T> hasher;
			combine(hasher(v));
		}
		template<typename T>
		void update_array(const T* addr, size_t cnt) {
			std::hash<T> hasher;
			for (size_t i = 0; i < cnt; ++i) {
				combine(hasher(addr[i]));
			}
		}

	private:
		ValueType seed;
	};

#pragma endregion

} // namespace BMapInspector::Ruleset::Shared::DupCmp

namespace BMapInspector::Ruleset::Shared::DupCmp {

#pragma region CKObject Hash and Equal

	struct CKTextureHash {
		[[nodiscard]] size_t operator()(const BMAPINSP_O::CKTexture* tex) const noexcept;
	};

	struct CKMaterialHash {
		[[nodiscard]] size_t operator()(const BMAPINSP_O::CKMaterial* mtl) const noexcept;
	};

	struct CKMeshHash {
		[[nodiscard]] size_t operator()(const BMAPINSP_O::CKMesh* _mesh) const noexcept;
	};

	struct CKTextureEqualTo {
		[[nodiscard]] bool operator()(const BMAPINSP_O::CKTexture* lhs, const BMAPINSP_O::CKTexture* rhs) const;
	};

	struct CKMaterialEqualTo {
		[[nodiscard]] bool operator()(const BMAPINSP_O::CKMaterial* lhs, const BMAPINSP_O::CKMaterial* rhs) const;
	};

	struct CKMeshEqualTo {
		[[nodiscard]] bool operator()(const BMAPINSP_O::CKMesh* _lhs, const BMAPINSP_O::CKMesh* _rhs) const;
	};

#pragma endregion

#pragma region CKObject Wrapper

	class CKTextureWrapper {
	public:
		CKTextureWrapper(BMAPINSP_O::CKTexture* texture);
		~CKTextureWrapper();
		YYCC_DEFAULT_COPY_MOVE(CKTextureWrapper)

	public:
		BMAPINSP_O::CKTexture* GetTexture() const;
		size_t GetHash() const;

	private:
		BMAPINSP_O::CKTexture* texture;
		CKTextureHash hasher;
		mutable std::optional<size_t> hash;
	};

	class CKMaterialWrapper {
	public:
		CKMaterialWrapper(BMAPINSP_O::CKMaterial* material);
		~CKMaterialWrapper();
		YYCC_DEFAULT_COPY_MOVE(CKMaterialWrapper)

	public:
		BMAPINSP_O::CKMaterial* GetMaterial() const;
		size_t GetHash() const;

	private:
		BMAPINSP_O::CKMaterial* material;
		CKMaterialHash hasher;
		mutable std::optional<size_t> hash;
	};

	class CKMeshWrapper {
	public:
		CKMeshWrapper(BMAPINSP_O::CKMesh* mesh);
		~CKMeshWrapper();
		YYCC_DEFAULT_COPY_MOVE(CKMeshWrapper)

	public:
		BMAPINSP_O::CKMesh* GetMesh() const;
		size_t GetHash() const;

	private:
		BMAPINSP_O::CKMesh* mesh;
		CKMeshHash hasher;
		mutable std::optional<size_t> hash;
	};

#pragma endregion

#pragma region CKObject Wrapper Hash and Equal

	struct CKTextureWrapperHash {
		[[nodiscard]] size_t operator()(const CKTextureWrapper& tex) const noexcept;
	};

	struct CKMaterialWrapperHash {
		[[nodiscard]] size_t operator()(const CKMaterialWrapper& mtl) const noexcept;
	};

	struct CKMeshWrapperHash {
		[[nodiscard]] size_t operator()(const CKMeshWrapper& mesh) const noexcept;
	};

	struct CKTextureWrapperEqualTo {
		CKTextureEqualTo equal_to;
		[[nodiscard]] bool operator()(const CKTextureWrapper& lhs, const CKTextureWrapper& rhs) const;
	};

	struct CKMaterialWrapperEqualTo {
		CKMaterialEqualTo equal_to;
		[[nodiscard]] bool operator()(const CKMaterialWrapper& lhs, const CKMaterialWrapper& rhs) const;
	};

	struct CKMeshWrapperEqualTo {
		CKMeshEqualTo equal_to;
		[[nodiscard]] bool operator()(const CKMeshWrapper& lhs, const CKMeshWrapper& rhs) const;
	};

#pragma endregion

} // namespace BMapInspector::Ruleset::Shared::DupCmp
