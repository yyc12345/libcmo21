#include "Map.hpp"
#include <VTAll.hpp>
#include <filesystem>
#include <stdexcept>

namespace C = LibCmo::CK2;
namespace O = LibCmo::CK2::ObjImpls;

namespace BMapInspector::Map {

#pragma region Level

	Level::Level(const Cli::Args& args) : m_Context(nullptr), m_LoadStatus() {
		// Create contexy
		this->m_Context = new C::CKContext();
		// Callback for eating all output.
		this->m_Context->SetOutputCallback([](LibCmo::CKSTRING strl) -> void {});
		// Set temp folder
		auto pm = m_Context->GetPathManager();
		auto temp_dir = std::filesystem::temp_directory_path();
		auto our_temp_dir = temp_dir / u8"88797AB5-915E-39AD-4800-BF1B0F59E207";
		std::filesystem::create_directories(our_temp_dir);
		if (!pm->SetTempFolder(our_temp_dir.u8string().c_str())) {
			this->m_LoadStatus = std::unexpected(Error::BadTempDir);
			return;
		}
		// Set ballance textures directory
		auto ballance_dir = std::filesystem::path(args.ballance_path);
		auto textures_dir = ballance_dir / u8"Textures";
		if (!pm->AddPath(textures_dir.u8string().c_str())) {
			this->m_LoadStatus = std::unexpected(Error::BadBallance);
			return;
		}
		// Set encoding
		LibCmo::XContainer::XArray<LibCmo::XContainer::XString> encs;
		encs.emplace_back(args.encoding);
		this->m_Context->SetEncoding(encs);
		if (!this->m_Context->IsValidEncoding()) {
			this->m_LoadStatus = std::unexpected(Error::BadEncoding);
			return;
		}
		// set default texture save mode is external
		this->m_Context->SetGlobalImagesSaveOptions(C::CK_TEXTURE_SAVEOPTIONS::CKTEXTURE_EXTERNAL);
		// set default file write mode is whole compressed
		this->m_Context->SetFileWriteMode(C::CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED);

		// Create temp ckfile and load
		C::CKFileReader reader(m_Context);
		auto err = reader.DeepLoad(args.file_path.c_str());
		if (err != C::CKERROR::CKERR_OK) {
			// failed. clear document and return
			this->m_Context->ClearAll();
			this->m_LoadStatus = std::unexpected(Error::BadMap);
			return;
		}

		// Fill our list
		for (const auto& fileobj : reader.GetFileObjects()) {
			auto ptr = fileobj.ObjPtr;
			if (ptr == nullptr) continue;

			switch (fileobj.ObjectCid) {
				case LibCmo::CK2::CK_CLASSID::CKCID_GROUP:
					m_ObjGroups.emplace_back(static_cast<O::CKGroup*>(ptr));
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT:
					m_Obj3dObjects.emplace_back(static_cast<O::CK3dObject*>(ptr));
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_MESH:
					m_ObjMeshes.emplace_back(static_cast<O::CKMesh*>(ptr));
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL:
					m_ObjMaterials.emplace_back(static_cast<O::CKMaterial*>(ptr));
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE:
					m_ObjTextures.emplace_back(static_cast<O::CKTexture*>(ptr));
					break;
				case LibCmo::CK2::CK_CLASSID::CKCID_TARGETLIGHT:
					m_ObjTargetLights.emplace_back(static_cast<O::CKTargetLight*>(ptr));
					break;
				default:
					break; // skip unknow objects
			}
		}
		// Okey
		this->m_LoadStatus = {};
	}

	Level::~Level() {
		if (this->m_Context != nullptr) {
			delete this->m_Context;
		}
	}

	YYCC_IMPL_MOVE_CTOR(Level, rhs) : m_Context(rhs.m_Context) {
		rhs.m_Context = nullptr;
	}

	YYCC_IMPL_MOVE_OPER(Level, rhs) {
		this->m_Context = rhs.m_Context;
		rhs.m_Context = nullptr;
		return *this;
	}

#define CHECK_STATUS(this) \
	if (!this->m_LoadStatus.has_value()) throw std::logic_error("can not fetch anything from not loaded level");

	Result<void> Level::GetLoadStatus() const {
		return this->m_LoadStatus;
	}

	C::CKContext* Level::GetCKContext() const {
		CHECK_STATUS(this)
		return this->m_Context;
	}

	const std::vector<O::CKGroup*>& Level::GetGroups() const {
		CHECK_STATUS(this)
		return this->m_ObjGroups;
	}

	const std::vector<O::CK3dObject*>& Level::Get3dObjects() const {
		CHECK_STATUS(this)
		return this->m_Obj3dObjects;
	}

	const std::vector<O::CKMesh*>& Level::GetMeshes() const {
		CHECK_STATUS(this)
		return this->m_ObjMeshes;
	}

	const std::vector<O::CKMaterial*>& Level::GetMaterials() const {
		CHECK_STATUS(this)
		return this->m_ObjMaterials;
	}

	const std::vector<O::CKTexture*>& Level::GetTextures() const {
		CHECK_STATUS(this)
		return this->m_ObjTextures;
	}

	const std::vector<O::CKTargetLight*>& Level::GetTargetLights() const {
		CHECK_STATUS(this)
		return this->m_ObjTargetLights;
	}

#undef CHECK_STATUS

#pragma endregion

	Result<Level> load(const Cli::Args& args) {
		Level rv(args);
		auto status = rv.GetLoadStatus();
		if (status.has_value()) return rv;
		else return std::unexpected(status.error());
	}

} // namespace BMapInspector::Map
