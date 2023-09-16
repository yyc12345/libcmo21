#pragma once

#include "../VTUtils.hpp"
#include "CKTypes.hpp"
#include <cinttypes>
#include <cstdint>

namespace LibCmo::CK2 {

	/**
	Specify the way files are saved to disk (compression)
	@remark
		+ File write mode controls the format of a Virtools file when saved. More specifically it
		controls whether compression is enabled and also if the Virtools Dev Interface specific data
		should be stored in the file (if CKFILE_FORVIEWER flag is set , no interface data is saved)

	@see CKContext::SetFileWriteMode, CKContext::GetFileWriteMode, CKContext::SetCompressionLevel,
	CKContext::SetGlobalImagesSaveOptions, CKContext::SetGlobalSoundsSaveOptions
	 */
	enum class CK_FILE_WRITEMODE : CKDWORD {
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
	enum class CK_LOAD_FLAGS : CKDWORD {
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
	enum class CK_FO_OPTIONS : CKDWORD {
		CK_FO_DEFAULT = 0,	/**< Default behavior : a new object will be created with the name stored in CKFileObject  */
		CK_FO_RENAMEOBJECT,	/**< Renaming : a new object will be created with the name stored in CKFileObject + a integer value XXX to ensure its uniqueness  */
		CK_FO_REPLACEOBJECT,	/**< Do not create a new object, instead use an existing one which CK_ID is given by CreatedObject to load the chunk on  */
		CK_FO_DONTLOADOBJECT,	/**< Object chunk will not be read either because it is a reference or because the loaded object already exist in the current level and the user choose to keep the existing one.  */
	};
	/**
	@brief Specify the way an object just loaded should be handled when it already exists in the level.
	 */
	enum class CK_LOADMODE : CKINT {
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
	enum class CK_OBJECTCREATION_OPTIONS : CKDWORD {
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
	enum class CK_PLUGIN_TYPE : CKDWORD {
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
	enum class CK_STATECHUNK_CHUNKOPTIONS : CKDWORD {
		CHNK_OPTION_IDS = 0x01,	/**< IDS are stored inside chunk  */
		CHNK_OPTION_MAN = 0x02,	/**< Managers ints are store inside chunk  */
		CHNK_OPTION_CHN = 0x04,	/**< Sub chunk are stored	inside chunk  */
		CHNK_OPTION_FILE = 0x08,	/**< Chunk was written with indices relative to a file....  */
		CHNK_OPTION_ALLOWDYN = 0x10,	/**< Dynamic object can be written in the chunk  */
		CHNK_OPTION_LISTBIG = 0x20,	/**< List are store in big Endian ?  */
		CHNK_DONTDELETE_PTR = 0x40,	/**< Data buffer stored in m_Buffer is not owned by CKStateChunk , it must not be deleted...  */
		CHNK_DONTDELETE_PARSER = 0x80,	/**< m_Parser Ptr is not owned by CKStateChunk , it must not be deleted...  */
	};
	enum class CK_STATECHUNK_CHUNKVERSION : CKDWORD {
		CHUNK_VERSIONBASE = 0,
		CHUNK_VERSION1 = 4,	/**< equal to file version : WriteObjectID => table  */
		CHUNK_VERSION2 = 5,	/**< add Manager Data  */
		CHUNK_VERSION3 = 6,	/**< New ConvertToBuffer / ReadFromBuffer (file system changed to reflect this )  */
		CHUNK_VERSION4 = 7,	/**< New WriteObjectID when saving to a file  */
	};
	enum class CK_STATECHUNK_DATAVERSION : CKDWORD {
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
	enum class CK_OBJECT_FLAGS : CKDWORD {
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
	enum class CK_3DENTITY_FLAGS : CKDWORD {
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
	{filename:CK_TEXTURE_SAVEOPTIONS}
	Summary: Specify the way textures or sprites will be saved

	Remarks : 
		+ Textures can be stored inside Virtools files or kept as references to external files.
		+ These options can be used for a specific texture (or sprite) or as a global setting.
	See also: CKBitmapData::SetSaveOptions,CKSprite::SetSaveOptions,CKContext::SetGlobalImagesSaveOptions 
	 */
	enum class CK_TEXTURE_SAVEOPTIONS : CKDWORD {
		CKTEXTURE_RAWDATA = 0,	/**< Save raw data inside file. The bitmap is saved in a raw 32 bit per pixel format.  */
		CKTEXTURE_EXTERNAL = 1,	/**< Store only the file name for the texture. The bitmap file must be present in the bitmap paths when loading the composition.  */
		CKTEXTURE_IMAGEFORMAT = 2,	/**< Save using format specified. The bitmap data will be converted to the specified format by the correspondant bitmap plugin and saved inside file.  */
		CKTEXTURE_USEGLOBAL = 3,	/**< Use Global settings, that is the settings given with CKContext::SetGlobalImagesSaveOptions. (Not valid when using CKContext::SetImagesSaveOptions).  */
		CKTEXTURE_INCLUDEORIGINALFILE = 4,	/**< Insert original image file inside CMO file. The bitmap file that was used originally for the texture or sprite will be append to the composition file and extracted when the file is loaded.  */
	};
	/**
	{filename:CK_SOUND_SAVEOPTIONS}
	Summary: Specify the way sounds will be saved

	Remarks : 
		+ Sounds can kept as references to external files or the original sound file can
		be appended to the composition file.
		+ These options can be used for a specific sound or as a global setting.
	See also: CKSound::SetSaveOptions,CKContext::SetGlobalSoundSaveOptions 
	 */
	enum class CK_SOUND_SAVEOPTIONS : CKDWORD {
		CKSOUND_EXTERNAL = 0,	/**< Store only the file name for the sound. The sound file must be present in one of the sound paths when the composition is loaded.  */
		CKSOUND_INCLUDEORIGINALFILE = 1,	/**< Insert original sound file inside the CMO file. The sound file that was used originally will be append to  the composition file and extracted when the file is loaded.  */
		CKSOUND_USEGLOBAL = 2,	/**< Use Global settings. This flag is only valid for the CKSound::SetSaveOptions method.  */
	};

	enum class CK_BITMAPDATA_FLAGS : CKDWORD {
		CKBITMAPDATA_INVALID = 1,
		CKBITMAPDATA_TRANSPARENT = 2,
		CKBITMAPDATA_FORCERESTORE = 4,
		CKBITMAPDATA_CLAMPUPTODATE = 8,
		CKBITMAPDATA_CUBEMAP = 16,
		CKBITMAPDATA_FREEVIDEOMEMORY = 32,
		CKBITMAPDATA_DYNAMIC = 64,
	};

}
