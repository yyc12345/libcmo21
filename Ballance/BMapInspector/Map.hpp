#pragma once
#include "Cli.hpp"
#include <yycc.hpp>
#include <yycc/macro/class_copy_move.hpp>
#include <string>
#include <vector>
#include <optional>
#include <expected>

namespace LibCmo::CK2 {
	class CKContext;
	namespace ObjImpls {
		class CKGroup;
		class CK3dObject;
		class CKMesh;
		class CKMaterial;
		class CKTexture;
		class CKTargetLight;
	} // namespace ObjImpls
} // namespace LibCmo::CK2

namespace BMapInspector::Map {

	enum class Error {
		BadTempDir,  ///< Fail to set temporary directory.
		BadBallance, ///< Fail to set Ballance texture directory.
		BadEncoding, ///< Fail to set encoding for loading map.
		BadMap,      ///< Can not load given map.
	};

	template<typename T>
	using Result = std::expected<T, Error>;

	class Level {
	public:
		Level(const Cli::Args& args);
		~Level();
		YYCC_DELETE_COPY(Level)
		YYCC_DECL_MOVE(Level)

	public:
		Result<void> GetLoadStatus() const;
		LibCmo::CK2::CKContext* GetCKContext() const;

	private:
		LibCmo::CK2::CKContext* m_Context;
		Result<void> m_LoadStatus; ///< Nothing or error occurs when loading user given map.

	public:
		const std::vector<LibCmo::CK2::ObjImpls::CKGroup*>& GetGroups() const;
		const std::vector<LibCmo::CK2::ObjImpls::CK3dObject*>& Get3dObjects() const;
		const std::vector<LibCmo::CK2::ObjImpls::CKMesh*>& GetMeshes() const;
		const std::vector<LibCmo::CK2::ObjImpls::CKMaterial*>& GetMaterials() const;
		const std::vector<LibCmo::CK2::ObjImpls::CKTexture*>& GetTextures() const;
		const std::vector<LibCmo::CK2::ObjImpls::CKTargetLight*>& GetTargetLights() const;

	private:
		std::vector<LibCmo::CK2::ObjImpls::CKGroup*> m_ObjGroups;
		std::vector<LibCmo::CK2::ObjImpls::CK3dObject*> m_Obj3dObjects;
		std::vector<LibCmo::CK2::ObjImpls::CKMesh*> m_ObjMeshes;
		std::vector<LibCmo::CK2::ObjImpls::CKMaterial*> m_ObjMaterials;
		std::vector<LibCmo::CK2::ObjImpls::CKTexture*> m_ObjTextures;
		std::vector<LibCmo::CK2::ObjImpls::CKTargetLight*> m_ObjTargetLights;
	};

	Result<Level> load(const Cli::Args& args);

} // namespace BMapInspector::Map
