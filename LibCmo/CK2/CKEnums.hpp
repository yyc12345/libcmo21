#pragma once

#include "../VTUtils.hpp"
#include "CKTypes.hpp"
#include <cinttypes>
#include <cstdint>

namespace LibCmo::CK2 {

#pragma region Preregistred Managers

	// Virtools Managers GUID second data is 0

	constexpr const CKDWORD OBJECT_MANAGER_GUID1 = 0x7cbb3b91;
	constexpr const CKDWORD ATTRIBUTE_MANAGER_GUID1 = 0x3d242466;
	constexpr const CKDWORD MESSAGE_MANAGER_GUID1 = 0x466a0fac;
	constexpr const CKDWORD FLOOR_MANAGER_GUID1 = 0x420936f9;
	constexpr const CKDWORD COLLISION_MANAGER_GUID1 = 0x38244712;
	constexpr const CKDWORD GRID_MANAGER_GUID1 = 0x7f004791;
	constexpr const CKDWORD TIME_MANAGER_GUID1 = 0x89ce7b32;
	constexpr const CKDWORD BEHAVIOR_MANAGER_GUID1 = 0x58d621ae;
	constexpr const CKDWORD INPUT_MANAGER_GUID1 = 0xf787c904;
	constexpr const CKDWORD SOUND_MANAGER_GUID1 = 0xdce135f6;
	constexpr const CKDWORD MIDI_MANAGER_GUID1 = 0x594154a6;
	constexpr const CKDWORD INTERFACE_MANAGER_GUID1 = 0x9a4b8e3d;
	constexpr const CKDWORD RENDER_MANAGER_GUID1 = 0xa213c8d5;
	constexpr const CKDWORD PARAMETER_MANAGER_GUID1 = 0x9ce57ab6;
	constexpr const CKDWORD PATH_MANAGER_GUID1 = 0x15fd54b9;
	constexpr const CKDWORD VARIABLE_MANAGER_GUID1 = 0x98cc3cc9;

	constexpr const CKGUID OBJECT_MANAGER_GUID{ OBJECT_MANAGER_GUID1 ,0 };
	constexpr const CKGUID ATTRIBUTE_MANAGER_GUID{ ATTRIBUTE_MANAGER_GUID1, 0 };
	constexpr const CKGUID MESSAGE_MANAGER_GUID{ MESSAGE_MANAGER_GUID1 ,0 };
	constexpr const CKGUID TIME_MANAGER_GUID{ TIME_MANAGER_GUID1 ,0 };
	constexpr const CKGUID SOUND_MANAGER_GUID{ SOUND_MANAGER_GUID1 ,0 };
	constexpr const CKGUID MIDI_MANAGER_GUID{ MIDI_MANAGER_GUID1 ,0 };
	constexpr const CKGUID INPUT_MANAGER_GUID{ INPUT_MANAGER_GUID1 ,0 };
	constexpr const CKGUID BEHAVIOR_MANAGER_GUID{ BEHAVIOR_MANAGER_GUID1 ,0 };
	constexpr const CKGUID FLOOR_MANAGER_GUID{ FLOOR_MANAGER_GUID1 ,0 };
	constexpr const CKGUID COLLISION_MANAGER_GUID{ COLLISION_MANAGER_GUID1 ,0 };
	constexpr const CKGUID GRID_MANAGER_GUID{ GRID_MANAGER_GUID1 ,0 };
	constexpr const CKGUID INTERFACE_MANAGER_GUID{ INTERFACE_MANAGER_GUID1 ,0 };
	constexpr const CKGUID RENDER_MANAGER_GUID{ RENDER_MANAGER_GUID1 ,0 };
	constexpr const CKGUID PARAMETER_MANAGER_GUID{ PARAMETER_MANAGER_GUID1 ,0 };
	constexpr const CKGUID PATH_MANAGER_GUID{ PATH_MANAGER_GUID1 ,0 };
	constexpr const CKGUID VARIABLE_MANAGER_GUID{ VARIABLE_MANAGER_GUID1 ,0 };

#pragma endregion

	/**
	Specify the way files are saved to disk (compression)
	@remark
		+ File write mode controls the format of a Virtools file when saved. More specifically it
		controls whether compression is enabled and also if the Virtools Dev Interface specific data
		should be stored in the file (if CKFILE_FORVIEWER flag is set , no interface data is saved)

	@see CKContext::SetFileWriteMode, CKContext::GetFileWriteMode, CKContext::SetCompressionLevel,
	CKContext::SetGlobalImagesSaveOptions, CKContext::SetGlobalSoundsSaveOptions
	 */
	enum class CK_FILE_WRITEMODE : uint32_t {
		CKFILE_UNCOMPRESSED = 0,	/**< Save data uncompressed  */
		CKFILE_CHUNKCOMPRESSED_OLD = 1,	/**< Obsolete  */
		CKFILE_EXTERNALTEXTURES_OLD = 2,	/**< Obsolete : use CKContext::SetGlobalImagesSaveOptions instead.  */
		CKFILE_FORVIEWER = 4,	/**< Don't save Interface Data within the file, the level won't be editable anymore in the interface  */
		CKFILE_WHOLECOMPRESSED = 8,	/**< Compress the whole file  */
	};
	/**
	Load Options.
	@remark
		+ This options apply when loading a Virtools file
		or a importing a 3D Model file.
		+ They defines whether object geometry,only animations
		or only behaviors should be loaded.
		+ One can specify (using the CK_LOAD_AS_DYNAMIC_OBJECT) if
		created CKObjects should be created as dynamic (See also Dynamic Objects)
	See also : CKContext::Load, CKContext::CKSave
	 */
	enum class CK_LOAD_FLAGS : uint32_t {
		CK_LOAD_ANIMATION = 1 << 0,	/**< Load animations  */
		CK_LOAD_GEOMETRY = 1 << 1,	/**< Load geometry.  */
		CK_LOAD_DEFAULT = CK_LOAD_GEOMETRY | CK_LOAD_ANIMATION,	/**< Load animations & geometry  */
		CK_LOAD_ASCHARACTER = 1 << 2,	/**< Load all the objects and create a character that contains them all .  */
		CK_LOAD_DODIALOG = 1 << 3,	/**< Check object name unicity and warns the user with a dialog box when duplicate names are found.  */
		CK_LOAD_AS_DYNAMIC_OBJECT = 1 << 4,	/**< Objects loaded from this file may be deleted at run-time or are temporary  */
		CK_LOAD_AUTOMATICMODE = 1 << 5,	/**< Check object name unicity and automatically rename or replace according to the options specified in CKContext::SetAutomaticLoadMode  */
		CK_LOAD_CHECKDUPLICATES = 1 << 6,	/**< Check object name unicity (The list of duplicates is stored in the CKFile class after a OpenFile call  */
		CK_LOAD_CHECKDEPENDENCIES = 1 << 7,	/**< Check if every plugins needed are availables  */
		CK_LOAD_ONLYBEHAVIORS = 1 << 8,	/**<   */
	};
	/**
	Options that will be used to create this object...
	 */
	enum class CK_FO_OPTIONS : uint32_t {
		CK_FO_DEFAULT = 0,	/**< Default behavior : a new object will be created with the name stored in CKFileObject  */
		CK_FO_RENAMEOBJECT,	/**< Renaming : a new object will be created with the name stored in CKFileObject + a integer value XXX to ensure its uniqueness  */
		CK_FO_REPLACEOBJECT,	/**< Do not create a new object, instead use an existing one which CK_ID is given by CreatedObject to load the chunk on  */
		CK_FO_DONTLOADOBJECT,	/**< Object chunk will not be read either because it is a reference or because the loaded object already exist in the current level and the user choose to keep the existing one.  */
	};
	/**
	@brief Specify the way an object just loaded should be handled when it already exists in the level.
	 */
	enum class CK_LOADMODE : int32_t {
		CKLOAD_INVALID = -1, /**< Use the existing object instead of loading   */
		CKLOAD_OK = 0, /**< Ignore ( Name unicity is broken )  */
		CKLOAD_REPLACE = 1, /**< Replace the existing object (Not yet implemented)  */
		CKLOAD_RENAME = 2, /**< Rename the loaded object  */
		CKLOAD_USECURRENT = 3,/**< Use the existing object instead of loading   */
	};
	using CK_CREATIONMODE = CK_LOADMODE;
	/**
	Specify the way an object is created through CKCreateObject.
	@remark
		+ These flag controls the way an object is created, the most important of these flags
		being CK_OBJECTCREATION_DYNAMIC which, if set in CKCreateObject, make the newly created object
		dynamic.
	@see CKContext::CreateObject
	 */
	enum class CK_OBJECTCREATION_OPTIONS : uint32_t {
		CK_OBJECTCREATION_NONAMECHECK = 0, /**< Do not test for name unicity (may be overriden in special case)  */
		CK_OBJECTCREATION_REPLACE = 1, /**< Replace the current object by the object being loaded  */
		CK_OBJECTCREATION_RENAME = 2, /**< Rename the created object to ensure its uniqueness  */
		CK_OBJECTCREATION_USECURRENT = 3, /**< Do not create a new object, use the one with the same name instead  */
		CK_OBJECTCREATION_ASK = 4, /**< If a duplicate name if found, opens a dialog box to ask the useror use automatic load mode if any.  */
		CK_OBJECTCREATION_FLAGSMASK = 0x0000000F, /**< Mask for previous values  */
		CK_OBJECTCREATION_DYNAMIC = 0x00000010, /**< The object must be created dynamic  */
		CK_OBJECTCREATION_ACTIVATE = 0x00000020, /**< The object will be copied/created active  */
		CK_OBJECTCREATION_NONAMECOPY = 0x00000040 /**< The object will take control of the string given to it directly, without copying it  */
	};
	/**
	Type identifier for a Virtools plugin.
	@remark
		+ Each plugin must be given a type.
		+ This enumeration is used to identify a specific catagory
		of plugin when using the CKPluginManager.

	@see CKPluginManager
	 */
	enum class CK_PLUGIN_TYPE : uint32_t {
		CKPLUGIN_BITMAP_READER = 0,	/**< The plugin is bitmap (textures,sprites) loader  */
		CKPLUGIN_SOUND_READER = 1,	/**< Sound Reader Plugin  */
		CKPLUGIN_MODEL_READER = 2,	/**< 3D Model Reader  */
		CKPLUGIN_MANAGER_DLL = 3,	/**< The plugin implements a Manager  */
		CKPLUGIN_BEHAVIOR_DLL = 4,	/**< The plugin implements one or more behaviors  */
		CKPLUGIN_RENDERENGINE_DLL = 5,	/**< Render Engine plugin  */
		CKPLUGIN_MOVIE_READER = 6,	/**< Movie (AVI,Mpeg) reader  */
		CKPLUGIN_EXTENSION_DLL = 7,	/**< Generic extension (definition of new parameter types or operations for ex.)  */
	};
	/**
	@remark CHUNK_OPTIONS in original Virtools header.
	 */
	enum class CK_STATECHUNK_CHUNKOPTIONS : uint32_t {
		CHNK_OPTION_IDS = 0x01,	/**< IDS are stored inside chunk  */
		CHNK_OPTION_MAN = 0x02,	/**< Managers ints are store inside chunk  */
		CHNK_OPTION_CHN = 0x04,	/**< Sub chunk are stored	inside chunk  */
		CHNK_OPTION_FILE = 0x08,	/**< Chunk was written with indices relative to a file....  */
		CHNK_OPTION_ALLOWDYN = 0x10,	/**< Dynamic object can be written in the chunk  */
		CHNK_OPTION_LISTBIG = 0x20,	/**< List are store in big Endian ?  */
		CHNK_DONTDELETE_PTR = 0x40,	/**< Data buffer stored in m_Buffer is not owned by CKStateChunk , it must not be deleted...  */
		CHNK_DONTDELETE_PARSER = 0x80,	/**< m_Parser Ptr is not owned by CKStateChunk , it must not be deleted...  */
	};
	enum class CK_STATECHUNK_CHUNKVERSION : uint32_t {
		CHUNK_VERSIONBASE = 0,
		CHUNK_VERSION1 = 4,	/**< equal to file version : WriteObjectID => table  */
		CHUNK_VERSION2 = 5,	/**< add Manager Data  */
		CHUNK_VERSION3 = 6,	/**< New ConvertToBuffer / ReadFromBuffer (file system changed to reflect this )  */
		CHUNK_VERSION4 = 7,	/**< New WriteObjectID when saving to a file  */
	};
	enum class CK_STATECHUNK_DATAVERSION : uint32_t {
		CHUNKDATA_OLDVERSION = 0,	/**< Before any version was saved  */
		CHUNKDATA_BASEVERSION = 1,	/**< First version  */
		CHUNK_WAVESOUND_VERSION2 = 2,	/**< Changes in wavesound format  */
		CHUNK_WAVESOUND_VERSION3 = 3,	/**< Changes in wavesound format  */
		CHUNK_MATERIAL_VERSION_ZTEST = 4,	/**< Change in material save format  */
		CHUNK_MAJORCHANGE_VERSION = 5,	/**< Optimisations on many save functions  */
		CHUNK_MACCHANGE_VERSION = 6,	/**< Misc new Statechunk functions for macintosh (Big-Endian <-> Little Endian conversion )  */
		CHUNK_WAVESOUND_VERSION4 = 7,	/**< Changes in wavesound format (Added sound length)  */
		CHUNK_SCENECHANGE_VERSION = 8,	/**< Changes in sceneObjectDesc format (Remove lasttimevalue)  */
		CHUNK_MESHCHANGE_VERSION = 9,	/**< Changes in Mesh save format (primitives)  */
		CHUNK_DEV_2_1 = 10,	/**< Changes in wavesound reading of inside, outside angles  */
		CHUNKDATA_CURRENTVERSION = CHUNK_DEV_2_1,
	};
	/**
	CKObject Flags
	@remark
		+ Flags specifying special settings for basic objects.
		+ Some of this flags are shared with sub-classes such as CKParameterIn,CKParameterOut and CKBehaviorIO.
		+ You rarely need to modify directly this flags through CKObject::SetFlags or CKObject::ModifyObjectFlags instead
		you should always use the specific acces function (given between ()) which may need to perform additionnal operations.
	@see CKObject, CKObject::GetObjectFlags, CKObject::ModifyObjectFlags
	 */
	enum class CK_OBJECT_FLAGS : uint32_t {
		CK_OBJECT_INTERFACEOBJ = 0x00000001,	/**< Reserved for Inteface Use  */
		CK_OBJECT_PRIVATE = 0x00000002,	/**< The object must not be displayed in interface (Lists,Level view,etc...),nor should it be saved. (CKObject::IsPrivate()  */
		CK_OBJECT_INTERFACEMARK = 0x00000004,
		CK_OBJECT_FREEID = 0x00000008,	/**< ID of this object can be released safely and is free to be reused by future CKobjects.  */
		CK_OBJECT_TOBEDELETED = 0x00000010,	/**< This object is being deleted  */
		CK_OBJECT_NOTTOBESAVED = 0x00000020,	/**< This object must not be saved  */
		CK_OBJECT_VISIBLE = 0x00000040,	/**< This object is visible (CKObject::Show)  */
		CK_OBJECT_NAMESHARED = 0x00000080,	/**< This object has its name from another entity  */
		CK_OBJECT_DYNAMIC = 0x00000108,	/**< This object may be created or deleted at run-time, it also contails CK_OBJECT_FREEID. (CKObject::IsDynamic,CKContext::CreateObject)  */
		CK_OBJECT_HIERACHICALHIDE = 0x00000200,	/**< This object hides all its hierarchy (CKObject::Show)  */
		CK_OBJECT_UPTODATE = 0x00000400,	/**< (Camera,etc..)  */
		CK_OBJECT_TEMPMARKER = 0x00000800,
		CK_OBJECT_ONLYFORFILEREFERENCE = 0x00001000,
		CK_OBJECT_NOTTOBEDELETED = 0x00002000,	/**< This object must not be deleted in a clear all  */
		CK_OBJECT_APPDATA = 0x00004000,	/**< This object has app data  */
		CK_OBJECT_SINGLEACTIVITY = 0x00008000,	/**< this object has an information of single activity  */
		CK_OBJECT_LOADSKIPBEOBJECT = 0x00010000,	/**< When loading this object the CKBeObject part should be skipped  */
		CK_OBJECT_NOTTOBELISTEDANDSAVED = 0x00000023,	/**< Combination of Private and Not To Be Saved The following flags are specific to parameters (they are stored here for object's size purposes )  */
		CK_PARAMETEROUT_SETTINGS = 0x00400000,
		CK_PARAMETEROUT_PARAMOP = 0x00800000,	/**< This parameter is the output of a CKParameterOperation (Automatically set by Engine)  */
		CK_PARAMETERIN_DISABLED = 0x01000000,	/**< Parameter In or Out is disabled (CKBehavior::EnableInputParameter,CKBehavior::DisableInputParameter)  */
		CK_PARAMETERIN_THIS = 0x02000000,	/**< Special parameter type : its value and type are always equal to its owner (CKParameter::SetAsMyselfParameter)  */
		CK_PARAMETERIN_SHARED = 0x04000000,
		CK_PARAMETEROUT_DELETEAFTERUSE = 0x08000000,	/**< When adding parameters to CKMessage, they can be automatically deleted when message is released (CKMessage::AddParameter)  */
		CK_OBJECT_PARAMMASK = 0x0FC00000,	/**< Mask for options specific to parameters The Following flags are specific	for Behavior ios (CKBehaviorIO)  */
		CK_BEHAVIORIO_IN = 0x10000000,	/**< This BehaviorIO is a behavior input  (CKBehaviorIO::SetType}  */
		CK_BEHAVIORIO_OUT = 0x20000000,	/**< This BehaviorIO is a behavior output (CKBehaviorIO::SetType)  */
		CK_BEHAVIORIO_ACTIVE = 0x40000000,	/**< This BehaviorIO is a currently active (CKBehaviorIO::Activate}  */
		CK_OBJECT_IOTYPEMASK = 0x30000000,
		CK_OBJECT_IOMASK = 0xF0000000,	/**< The Following flags are specific	for Behavior ios (CKBehaviorIO)  */
		CKBEHAVIORLINK_RESERVED = 0x10000000,	/**< This BehaviorIO is a behavior input  (CKBehaviorIO::SetType}  */
		CKBEHAVIORLINK_ACTIVATEDLASTFRAME = 0x20000000,	/**< This link had been activated last frame  */
		CK_OBJECT_BEHAVIORLINKMASK = 0x30000000,
	};
	
	/**
	@brief 3dEntity Flags
	@remark
		+ Flags give user and engine more information about the 3dEntity.
	@see CK3dEntity::SetFlags,CK3dEntity::GetFlags
	 */
	enum class CK_3DENTITY_FLAGS : uint32_t {
		CK_3DENTITY_DUMMY = 0x00000001,	/**< Entity is a dummy used to represent a position  */
		CK_3DENTITY_FRAME = 0x00000002,	/**< Entity is a frame used to represent an orientation  */
		CK_3DENTITY_RESERVED0 = 0x00000020,	/**< Obsolete Flag  */
		CK_3DENTITY_TARGETLIGHT = 0x00000100,	/**< Entity is a target of a light  */
		CK_3DENTITY_TARGETCAMERA = 0x00000200,	/**< Entity is a target of a camera  */
		CK_3DENTITY_IGNOREANIMATION = 0x00000400,	/**< Animation using this entity can't modify it  */
		CK_3DENTITY_HIERARCHICALOBSTACLE = 0x00000800,	/**< Used by the Collision Manager  */
		CK_3DENTITY_UPDATELASTFRAME = 0x00001000,	/**< Store the last world matrix for this Entity after each render  */
		CK_3DENTITY_CAMERAIGNOREASPECT = 0x00002000,	/**< Ignore aspect ratio setting for cameras  */
		CK_3DENTITY_DISABLESKINPROCESS = 0x00004000,	/**< Force skin processing to be disabled  */
		CK_3DENTITY_ENABLESKINOFFSET = 0x00008000,	/**< If not set the skin stay attached to the bones the vertices are linked to, otherwise the skin can be freely rotated,translated or scaled according to its owner entity matrix.  */
		CK_3DENTITY_PLACEVALID = 0x00010000,	/**< Used internally when saving  */
		CK_3DENTITY_PARENTVALID = 0x00020000,	/**< Used internally when saving  */
		CK_3DENTITY_IKJOINTVALID = 0x00040000,	/**< Special flag for Bodyparts : IK Joint data is valid  */
		CK_3DENTITY_PORTAL = 0x00080000,	/**< The 3dEntity is a portal  */
		CK_3DENTITY_ZORDERVALID = 0x00100000,	/**< The 3dEntity has a non-zero ZOrder  */
		CK_3DENTITY_CHARACTERDOPROCESS = 0x80000000,	/**< Special flag for Characters : Automatic process of animation  */
	};
	/**
	@brief 3dEntity additionnal flags Options
	@remark
		+ The VX_MOVEABLE_FLAGS is used by CK3dEntity::SetMoveableFlags to specify different hints to the render engine about the entity.
		+ The (Engine) flags are set by the render engine and should not be modified by user. They can be checked with the CK3dEntity::GetMoveableFlags method.
		+ The (User) flags are to be set by the user or can be set by a specific method of CK3dEntity.
	@see CK3dEntity::SetMoveableFlags
	 */
	enum class VX_MOVEABLE_FLAGS : uint32_t {
		VX_MOVEABLE_PICKABLE = 0x00000001,	/**< (User)If not set this entity cannot be returned by CKRenderContext::Pick() or CKRenderContext::RectPict() functions.  */
		VX_MOVEABLE_VISIBLE = 0x00000002,	/**< (Engine) See CKObject::Show,CK3dEntity::IsVisible  */
		VX_MOVEABLE_UPTODATE = 0x00000004,	/**< (Engine) Used to Notify change in the data of the entity.  */
		VX_MOVEABLE_RENDERCHANNELS = 0x00000008,	/**< (User) If not set, additional material channels on the mesh used by this entity won't be rendered (CK3dEntity::SetRenderChannels)  */
		VX_MOVEABLE_USERBOX = 0x00000010,	/**< (Engine) When CK3dEntity::SetBoundingBox is called with a user box, this flag is set.  */
		VX_MOVEABLE_EXTENTSUPTODATE = 0x00000020,	/**< (Engine) Indicate that object 2D extents are up to date  */
		VX_MOVEABLE_BOXVALID = 0x00004000,	/**< (Engine) If not set the moveable has no mesh associated so its bounding box is irrelevant (a point).  */
		VX_MOVEABLE_RENDERLAST = 0x00010000,	/**< (User) If set the moveable will be rendered with the transparent objects (i.e in last) (CK3dEntity::SetRenderAsTransparent)  */
		VX_MOVEABLE_HASMOVED = 0x00020000,	/**< (Engine) Set when its position or orientation has changed. (Reset every frame when rendering starts)  */
		VX_MOVEABLE_WORLDALIGNED = 0x00040000,	/**< (User) Hint for render engine : this object is aligned with world position and orientation.  */
		VX_MOVEABLE_NOZBUFFERWRITE = 0x00080000,	/**< (User) Set by the user to warn Render Engine that this object must not write information to Z buffer  */
		VX_MOVEABLE_RENDERFIRST = 0x00100000,	/**< (User) If set the moveable will be rendered within the firsts objects  */
		VX_MOVEABLE_NOZBUFFERTEST = 0x00200000,	/**< (User) Set by the user to warn Render Engine that this object must not test against Z buffer (This override settings of all materials used by this Entity)  */
		VX_MOVEABLE_INVERSEWORLDMATVALID = 0x00400000,	/**< (Engine) Inverse world matrix is not up to date and should be recomputed  */
		VX_MOVEABLE_DONTUPDATEFROMPARENT = 0x00800000,	/**< (User) This object will not be updated by parent (neither World nor Local matrix wil be updated) . This flags can be used by physic engine for example in which hierarchy is not relevant for physicalised objects  */
		VX_MOVEABLE_INDIRECTMATRIX = 0x01000000,	/**< (User/Engine) Set by the engine at load time  : The object matrix is in left hand referential, culling needs to be inverted  */
		VX_MOVEABLE_ZBUFONLY = 0x02000000,	/**< (User) The object will only be rendered in depth buffer  */
		VX_MOVEABLE_STENCILONLY = 0x04000000,	/**< (User) The object will only be rendered in stencil buffer  */
		VX_MOVEABLE_HIERARCHICALHIDE = 0x10000000,	/**< (Engine) If Object has this flags and is hidden its children won't be rendered  */
		VX_MOVEABLE_CHARACTERRENDERED = 0x20000000,	/**< (Engine) Set if a character was rendered last frame...  */
		VX_MOVEABLE_RESERVED2 = 0x40000000,	/**< (Engine)  */
	};
	/**
	****************************************************************
	@briefMesh Flags Options
	@remark
		+ The VXMESH_FLAGS is used by CKMesh::SetFlags to specify different hints to the render engine about the mesh.
		+ Most of this flags can be set or asked using the appropriate method of CKMesh (given between () in the members documentation).
	@see CKMesh,CKMesh::SetFlags
	*************************************************************** 
	 */
	enum class VXMESH_FLAGS : uint32_t {
		VXMESH_BOUNDINGUPTODATE = 0x00000001,	/**< If set the bounding box is up to date (internal).  */
		VXMESH_VISIBLE = 0x00000002,	/**< If not set the mesh will not be rendered (CKMesh::Show)  */
		VXMESH_OPTIMIZED = 0x00000004,	/**< Set by the render engine if the mesh is optimized for rendering. Unset it to force to recreate optimized structures (when changing materials or face organization ) (CKMesh::VertexMove)  */
		VXMESH_RENDERCHANNELS = 0x00000008,	/**< If not set  Additional material channels won't be rendered.  */
		VXMESH_HASTRANSPARENCY = 0x00000010,	/**< If set indicates that one or more of the faces of this mesh use a transparent material (internal)  */
		VXMESH_PRELITMODE = 0x00000080,	/**< If set, no lightning should occur for this mesh, vertex color should be used instead (CKMesh::SetLitMode)  */
		VXMESH_WRAPU = 0x00000100,	/**< Texture coordinates wrapping among u texture coordinates. (CKMesh::SetWrapMode)  */
		VXMESH_WRAPV = 0x00000200,	/**< Texture coordinates wrapping among v texture coordinates. (CKMesh::SetWrapMode)  */
		VXMESH_FORCETRANSPARENCY = 0x00001000,	/**< Forces this mesh to be considered as transparent even if no material is tranparent. (CKMesh::SetTransparent)  */
		VXMESH_TRANSPARENCYUPTODATE = 0x00002000,	/**< If set, the flags VXMESH_HASTRANSPARENCY is up to date. (internal)  */
		VXMESH_UV_CHANGED = 0x00004000,	/**< Must be set if texture coordinates changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::UVChanged)  */
		VXMESH_NORMAL_CHANGED = 0x00008000,	/**< Must be set if normal coordinates changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::NormalChanged)  */
		VXMESH_COLOR_CHANGED = 0x00010000,	/**< Must be set if colors changed to enable the render engine to reconstruct potential display lists or vertex buffers.	(CKMesh::ColorChanged)  */
		VXMESH_POS_CHANGED = 0x00020000,	/**< Must be set if vertex position changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::VertexMove)  */
		VXMESH_HINTDYNAMIC = 0x00040000,	/**< Hint for render engine : Mesh geometry is updated frequently  */
		VXMESH_GENNORMALS = 0x00080000,	/**< Hint : Normals were generated by BuildNormals : Do not save	(internal)  */
		VXMESH_PROCEDURALUV = 0x00100000,	/**< Hint : UVs are generated : Do not save (internal)  */
		VXMESH_PROCEDURALPOS = 0x00200000,	/**< Hint : Vertices postions are generated : Do not save (internal)  */
		VXMESH_STRIPIFY = 0x00400000,	/**< If set the mesh will be stripified.  */
		VXMESH_MONOMATERIAL = 0x00800000,	/**< Set by the render engine if the mesh use only one material.  */
		VXMESH_PM_BUILDNORM = 0x01000000,	/**< Build normals when performing progressive meshing : Do not save (internal)  */
		VXMESH_BWEIGHTS_CHANGED = 0x02000000,	/**< Must be set if vertex blend weights have changed to enable the render engine to reconstruct potential display lists or vertex buffers. (CKMesh::VertexMove)  */
		VXMESH_ALLFLAGS = 0x007FF39F,
	};
	/**
	************************************************
	@brief Specify the way textures or sprites will be saved
	@remark
		+ Textures can be stored inside Virtools files or kept as references to external files.
		+ These options can be used for a specific texture (or sprite) or as a global setting.
	@see CKBitmapData::SetSaveOptions,CKSprite::SetSaveOptions,CKContext::SetGlobalImagesSaveOptions
	********************************************** 
	 */
	enum class CK_TEXTURE_SAVEOPTIONS : uint32_t {
		CKTEXTURE_RAWDATA = 0,	/**< Save raw data inside file. The bitmap is saved in a raw 32 bit per pixel format.  */
		CKTEXTURE_EXTERNAL = 1,	/**< Store only the file name for the texture. The bitmap file must be present in the bitmap paths when loading the composition.  */
		CKTEXTURE_IMAGEFORMAT = 2,	/**< Save using format specified. The bitmap data will be converted to the specified format by the correspondant bitmap plugin and saved inside file.  */
		CKTEXTURE_USEGLOBAL = 3,	/**< Use Global settings, that is the settings given with CKContext::SetGlobalImagesSaveOptions. (Not valid when using CKContext::SetImagesSaveOptions).  */
		CKTEXTURE_INCLUDEORIGINALFILE = 4,	/**< Insert original image file inside CMO file. The bitmap file that was used originally for the texture or sprite will be append to the composition file and extracted when the file is loaded.  */
	};
	/**
	************************************************
	@brief Specify the way sounds will be saved
	@remark
		+ Sounds can kept as references to external files or the original sound file can
		be appended to the composition file.
		+ These options can be used for a specific sound or as a global setting.
	@see CKSound::SetSaveOptions,CKContext::SetGlobalSoundSaveOptions
	********************************************** 
	 */
	enum class CK_SOUND_SAVEOPTIONS : uint32_t {
		CKSOUND_EXTERNAL = 0,	/**< Store only the file name for the sound. The sound file must be present in one of the sound paths when the composition is loaded.  */
		CKSOUND_INCLUDEORIGINALFILE = 1,	/**< Insert original sound file inside the CMO file. The sound file that was used originally will be append to  the composition file and extracted when the file is loaded.  */
		CKSOUND_USEGLOBAL = 2,	/**< Use Global settings. This flag is only valid for the CKSound::SetSaveOptions method.  */
	};

	enum class CK_BITMAPDATA_FLAGS : uint32_t {
		CKBITMAPDATA_INVALID = 1,
		CKBITMAPDATA_TRANSPARENT = 2,
		CKBITMAPDATA_FORCERESTORE = 4,
		CKBITMAPDATA_CLAMPUPTODATE = 8,
		CKBITMAPDATA_CUBEMAP = 16,
		CKBITMAPDATA_FREEVIDEOMEMORY = 32,
		CKBITMAPDATA_DYNAMIC = 64,
	};

}
