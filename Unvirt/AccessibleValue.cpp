#include "AccessibleValue.hpp"
#include "StringHelper.hpp"
#include <vector>
#include <array>

namespace Unvirt::AccessibleValue {

#pragma region File Formatter

	std::string GetReadableFileSize(uint64_t size) {
		std::string strl;
		static double denominator = (double)0b1111111111;
		uint64_t probe = size;

		// check bytes
		if ((probe >> 10) == UINT64_C(0)) {
			StringHelper::StdstringPrintf(strl, "%" PRIu64 "Bytes", probe);
			return strl;
		}
		probe >>= 10;

		// check kb
		if ((probe >> 10) == UINT64_C(0)) {
			StringHelper::StdstringPrintf(strl, "%.2lfKiB", size / static_cast<double>(UINT64_C(1) << 10));
			return strl;
		}
		probe >>= 10;

		// check mb
		if ((probe >> 10) == UINT64_C(0)) {
			StringHelper::StdstringPrintf(strl, "%.2lfMiB", size / static_cast<double>(UINT64_C(1) << 20));
			return strl;
		}
		probe >>= 10;

		// otherwise gb
		StringHelper::StdstringPrintf(strl, "%.2lfGiB", size / static_cast<double>(UINT64_C(1) << 30));
		return strl;

	}

#pragma endregion

#pragma region CKERROR CK_CLASSID Data

	struct CkClassidReflection { std::vector<const char*> mHierarchy; };
	using CkClassidReflectionArray = std::vector<std::pair<LibCmo::CK2::CK_CLASSID, CkClassidReflection>>;

	struct CkErrorReflection { const char* mName; const char* mDescription; };
	using CkErrorReflectionArray = std::vector<std::pair<LibCmo::CK2::CKERROR, CkErrorReflection>>;

	namespace EnumDesc {

		const GeneralReflectionArray<LibCmo::CK2::CK_FILE_WRITEMODE> CK_FILE_WRITEMODE {
			{ LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_UNCOMPRESSED, {"CKFILE_UNCOMPRESSED"} },
			{ LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_CHUNKCOMPRESSED_OLD, {"CKFILE_CHUNKCOMPRESSED_OLD"} },
			{ LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_EXTERNALTEXTURES_OLD, {"CKFILE_EXTERNALTEXTURES_OLD"} },
			{ LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_FORVIEWER, {"CKFILE_FORVIEWER"} },
			{ LibCmo::CK2::CK_FILE_WRITEMODE::CKFILE_WHOLECOMPRESSED, {"CKFILE_WHOLECOMPRESSED"} },
		};
		const CkClassidReflectionArray CK_CLASSID {
			{ LibCmo::CK2::CK_CLASSID::CKCID_OBJECT, { { "CKCID_OBJECT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PARAMETERIN, { { "CKCID_OBJECT", "CKCID_PARAMETERIN" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PARAMETEROPERATION, { { "CKCID_OBJECT", "CKCID_PARAMETEROPERATION" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_STATE, { { "CKCID_OBJECT", "CKCID_STATE" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_BEHAVIORLINK, { { "CKCID_OBJECT", "CKCID_BEHAVIORLINK" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_BEHAVIOR, { { "CKCID_OBJECT", "CKCID_BEHAVIOR" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_BEHAVIORIO, { { "CKCID_OBJECT", "CKCID_BEHAVIORIO" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_RENDERCONTEXT, { { "CKCID_OBJECT", "CKCID_RENDERCONTEXT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_KINEMATICCHAIN, { { "CKCID_OBJECT", "CKCID_KINEMATICCHAIN" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SCENEOBJECT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_OBJECTANIMATION, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_OBJECTANIMATION" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_ANIMATION, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_ANIMATION" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_KEYEDANIMATION, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_ANIMATION", "CKCID_KEYEDANIMATION" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_BEOBJECT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_DATAARRAY, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_DATAARRAY" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SCENE, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SCENE" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_LEVEL, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_LEVEL" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PLACE, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_PLACE" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_GROUP, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_GROUP" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SOUND, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_WAVESOUND, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND", "CKCID_WAVESOUND" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_MIDISOUND, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND", "CKCID_MIDISOUND" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_MATERIAL, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MATERIAL" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_TEXTURE, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_TEXTURE" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_MESH, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MESH" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PATCHMESH, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MESH", "CKCID_PATCHMESH" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_RENDEROBJECT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_2DENTITY, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SPRITE, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY", "CKCID_SPRITE" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SPRITETEXT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY", "CKCID_SPRITETEXT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_3DENTITY, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_GRID, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_GRID" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_CURVEPOINT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CURVEPOINT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SPRITE3D, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_SPRITE3D" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_CURVE, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CURVE" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_CAMERA, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CAMERA" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_TARGETCAMERA, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CAMERA", "CKCID_TARGETCAMERA" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_LIGHT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_LIGHT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_TARGETLIGHT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_LIGHT", "CKCID_TARGETLIGHT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_CHARACTER, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CHARACTER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_3DOBJECT, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_3DOBJECT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_BODYPART, { { "CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_3DOBJECT", "CKCID_BODYPART" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PARAMETER, { { "CKCID_OBJECT", "CKCID_PARAMETER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PARAMETERLOCAL, { { "CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETERLOCAL" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PARAMETEROUT, { { "CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETEROUT" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_INTERFACEOBJECTMANAGER, { { "CKCID_OBJECT", "CKCID_INTERFACEOBJECTMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_CRITICALSECTION, { { "CKCID_OBJECT", "CKCID_CRITICALSECTION" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_LAYER, { { "CKCID_OBJECT", "CKCID_LAYER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PROGRESSIVEMESH, { { "CKCID_OBJECT", "CKCID_PROGRESSIVEMESH" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SYNCHRO, { { "CKCID_OBJECT", "CKCID_SYNCHRO" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_OBJECTARRAY, { { "CKCID_OBJECTARRAY" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SCENEOBJECTDESC, { { "CKCID_SCENEOBJECTDESC" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_ATTRIBUTEMANAGER, { { "CKCID_ATTRIBUTEMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_MESSAGEMANAGER, { { "CKCID_MESSAGEMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_COLLISIONMANAGER, { { "CKCID_COLLISIONMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_OBJECTMANAGER, { { "CKCID_OBJECTMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_FLOORMANAGER, { { "CKCID_FLOORMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_RENDERMANAGER, { { "CKCID_RENDERMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_BEHAVIORMANAGER, { { "CKCID_BEHAVIORMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_INPUTMANAGER, { { "CKCID_INPUTMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_PARAMETERMANAGER, { { "CKCID_PARAMETERMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_GRIDMANAGER, { { "CKCID_GRIDMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_SOUNDMANAGER, { { "CKCID_SOUNDMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_TIMEMANAGER, { { "CKCID_TIMEMANAGER" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_CUIKBEHDATA, { { "CKCID_CUIKBEHDATA" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_MAXCLASSID, { { "CKCID_MAXCLASSID" } } },
			{ LibCmo::CK2::CK_CLASSID::CKCID_MAXMAXCLASSID, { { "CKCID_MAXMAXCLASSID" } } },
		};

		const CkErrorReflectionArray CKERROR {
			{ LibCmo::CK2::CKERROR::CKERR_OK, { "CKERR_OK", "Operation successful " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDPARAMETER, { "CKERR_INVALIDPARAMETER", "One of the parameter passed to the function was invalid " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDPARAMETERTYPE, { "CKERR_INVALIDPARAMETERTYPE", "One of the parameter passed to the function was invalid " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDSIZE, { "CKERR_INVALIDSIZE", "The parameter size was invalid " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDOPERATION, { "CKERR_INVALIDOPERATION", "The operation type didn't exist " } },
			{ LibCmo::CK2::CKERROR::CKERR_OPERATIONNOTIMPLEMENTED, { "CKERR_OPERATIONNOTIMPLEMENTED", "The function used to execute the operation is not yet implemented " } },
			{ LibCmo::CK2::CKERROR::CKERR_OUTOFMEMORY, { "CKERR_OUTOFMEMORY", "There was not enough memory to perform the action " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOTIMPLEMENTED, { "CKERR_NOTIMPLEMENTED", "The function  is not yet implemented " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOTFOUND, { "CKERR_NOTFOUND", "There was an attempt to remove something not present " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOLEVEL, { "CKERR_NOLEVEL", "There is no level currently created " } },
			{ LibCmo::CK2::CKERROR::CKERR_CANCREATERENDERCONTEXT, { "CKERR_CANCREATERENDERCONTEXT", " " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOTIFICATIONNOTHANDLED, { "CKERR_NOTIFICATIONNOTHANDLED", "The notification message was not used " } },
			{ LibCmo::CK2::CKERROR::CKERR_ALREADYPRESENT, { "CKERR_ALREADYPRESENT", "Attempt to add an item that was already present " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDRENDERCONTEXT, { "CKERR_INVALIDRENDERCONTEXT", "the render context is not valid " } },
			{ LibCmo::CK2::CKERROR::CKERR_RENDERCONTEXTINACTIVE, { "CKERR_RENDERCONTEXTINACTIVE", "the render context is not activated for rendering " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOLOADPLUGINS, { "CKERR_NOLOADPLUGINS", "there was no plugins to load this kind of file " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOSAVEPLUGINS, { "CKERR_NOSAVEPLUGINS", "there was no plugins to save this kind of file " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDFILE, { "CKERR_INVALIDFILE", "attempt to load an invalid file " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDPLUGIN, { "CKERR_INVALIDPLUGIN", "attempt to load with an invalid plugin " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOTINITIALIZED, { "CKERR_NOTINITIALIZED", "attempt use an object that wasnt initialized " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDMESSAGE, { "CKERR_INVALIDMESSAGE", "attempt use a message type that wasn't registred " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDPROTOTYPE, { "CKERR_INVALIDPROTOTYPE", "attempt use an invalid prototype " } },
			{ LibCmo::CK2::CKERROR::CKERR_NODLLFOUND, { "CKERR_NODLLFOUND", "No dll file found in the parse directory " } },
			{ LibCmo::CK2::CKERROR::CKERR_ALREADYREGISTREDDLL, { "CKERR_ALREADYREGISTREDDLL", "this dll has already been registred " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDDLL, { "CKERR_INVALIDDLL", "this dll does not contain information to create the prototype " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDOBJECT, { "CKERR_INVALIDOBJECT", "Invalid Object (attempt to Get an object from an invalid ID) " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDCONDSOLEWINDOW, { "CKERR_INVALIDCONDSOLEWINDOW", "Invalid window was provided as console window " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDKINEMATICCHAIN, { "CKERR_INVALIDKINEMATICCHAIN", "Invalid kinematic chain ( end and start effector may not be part of the same hierarchy ) " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOKEYBOARD, { "CKERR_NOKEYBOARD", "Keyboard not attached or not working properly " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOMOUSE, { "CKERR_NOMOUSE", "Mouse not attached or not working properly " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOJOYSTICK, { "CKERR_NOJOYSTICK", "Joystick not attached or not working properly " } },
			{ LibCmo::CK2::CKERROR::CKERR_INCOMPATIBLEPARAMETERS, { "CKERR_INCOMPATIBLEPARAMETERS", "Try to link imcompatible Parameters " } },
			{ LibCmo::CK2::CKERROR::CKERR_NORENDERENGINE, { "CKERR_NORENDERENGINE", "There is no render engine dll " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOCURRENTLEVEL, { "CKERR_NOCURRENTLEVEL", "There is no current level (use CKSetCurrentLevel ) " } },
			{ LibCmo::CK2::CKERROR::CKERR_SOUNDDISABLED, { "CKERR_SOUNDDISABLED", "Sound Management has been disabled " } },
			{ LibCmo::CK2::CKERROR::CKERR_DINPUTDISABLED, { "CKERR_DINPUTDISABLED", "DirectInput Management has been disabled " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDGUID, { "CKERR_INVALIDGUID", "Guid is already in use or invalid " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOTENOUGHDISKPLACE, { "CKERR_NOTENOUGHDISKPLACE", "There was no more free space on disk when trying to save a file " } },
			{ LibCmo::CK2::CKERROR::CKERR_CANTWRITETOFILE, { "CKERR_CANTWRITETOFILE", "Impossible to write to file (write-protection ?) " } },
			{ LibCmo::CK2::CKERROR::CKERR_BEHAVIORADDDENIEDBYCB, { "CKERR_BEHAVIORADDDENIEDBYCB", "The behavior cannnot be added to this entity " } },
			{ LibCmo::CK2::CKERROR::CKERR_INCOMPATIBLECLASSID, { "CKERR_INCOMPATIBLECLASSID", "The behavior cannnot be added to this entity " } },
			{ LibCmo::CK2::CKERROR::CKERR_MANAGERALREADYEXISTS, { "CKERR_MANAGERALREADYEXISTS", "A manager was registered more than once " } },
			{ LibCmo::CK2::CKERROR::CKERR_PAUSED, { "CKERR_PAUSED", "CKprocess or TimeManager process while CK is paused will fail " } },
			{ LibCmo::CK2::CKERROR::CKERR_PLUGINSMISSING, { "CKERR_PLUGINSMISSING", "Some plugins were missing whileloading a file " } },
			{ LibCmo::CK2::CKERROR::CKERR_OBSOLETEVIRTOOLS, { "CKERR_OBSOLETEVIRTOOLS", "Virtools version too old to load this file " } },
			{ LibCmo::CK2::CKERROR::CKERR_FILECRCERROR, { "CKERR_FILECRCERROR", "CRC Error while loading file " } },
			{ LibCmo::CK2::CKERROR::CKERR_ALREADYFULLSCREEN, { "CKERR_ALREADYFULLSCREEN", "A Render context is already in Fullscreen Mode " } },
			{ LibCmo::CK2::CKERROR::CKERR_CANCELLED, { "CKERR_CANCELLED", "Operation was cancelled by user " } },
			{ LibCmo::CK2::CKERROR::CKERR_NOANIMATIONKEY, { "CKERR_NOANIMATIONKEY", "there were no animation key at the given index " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDINDEX, { "CKERR_INVALIDINDEX", "attemp to acces an animation key with an invalid index " } },
			{ LibCmo::CK2::CKERROR::CKERR_INVALIDANIMATION, { "CKERR_INVALIDANIMATION", "the animation is invalid (no entity associated or zero length) " } },
		};

	}

#pragma endregion

#pragma region CKERROR CK_CLASSID Process

	static const CkErrorReflection* GetCkError(LibCmo::CK2::CKERROR err) {
		for (auto& item : EnumDesc::CKERROR) {
			if (item.first == err) {
				return &item.second;
			}
		}
		return nullptr;
	}

	std::string GetCkErrorName(LibCmo::CK2::CKERROR err) {
		const CkErrorReflection* node = GetCkError(err);
		if (node != nullptr) return node->mName;
		else return c_InvalidEnumName;
	}

	std::string GetCkErrorDescription(LibCmo::CK2::CKERROR err) {
		const CkErrorReflection* node = GetCkError(err);
		if (node != nullptr) return node->mDescription;
		else return c_InvalidEnumName;
	}

	static const CkClassidReflection* GetCkClassid(LibCmo::CK2::CK_CLASSID cls) {
		for (auto& item : EnumDesc::CK_CLASSID) {
			if (item.first == cls) {
				return &item.second;
			}
		}
		return nullptr;
	}

	std::string GetClassIdName(LibCmo::CK2::CK_CLASSID cls) {
		const CkClassidReflection* node = GetCkClassid(cls);
		if (node == nullptr) return c_InvalidEnumName;
		else return node->mHierarchy.back();
	}

	std::string GetClassIdHierarchy(LibCmo::CK2::CK_CLASSID cls) {
		const CkClassidReflection* node = GetCkClassid(cls);
		if (node == nullptr) return c_InvalidEnumName;

		std::string strl;
		for (auto& hierarchy_node : node->mHierarchy) {
			if (!strl.empty()) strl += " -> ";
			strl += hierarchy_node;
		}
		return strl;
	}

#pragma endregion

#pragma region Other Enums

	namespace EnumDesc {

		const GeneralReflectionArray<LibCmo::CK2::CK_FO_OPTIONS> CK_FO_OPTIONS {
			{ LibCmo::CK2::CK_FO_OPTIONS::CK_FO_DEFAULT, {"CK_FO_DEFAULT"} },
			{ LibCmo::CK2::CK_FO_OPTIONS::CK_FO_RENAMEOBJECT, {"CK_FO_RENAMEOBJECT"} },
			{ LibCmo::CK2::CK_FO_OPTIONS::CK_FO_REPLACEOBJECT, {"CK_FO_REPLACEOBJECT"} },
			{ LibCmo::CK2::CK_FO_OPTIONS::CK_FO_DONTLOADOBJECT, {"CK_FO_DONTLOADOBJECT"} },
		};
		const GeneralReflectionArray<LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS> CK_STATECHUNK_CHUNKOPTIONS {
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_IDS, {"CHNK_OPTION_IDS"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_MAN, {"CHNK_OPTION_MAN"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_CHN, {"CHNK_OPTION_CHN"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_FILE, {"CHNK_OPTION_FILE"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_ALLOWDYN, {"CHNK_OPTION_ALLOWDYN"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_OPTION_LISTBIG, {"CHNK_OPTION_LISTBIG"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_DONTDELETE_PTR, {"CHNK_DONTDELETE_PTR"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKOPTIONS::CHNK_DONTDELETE_PARSER, {"CHNK_DONTDELETE_PARSER"} },
		};
		const GeneralReflectionArray<LibCmo::CK2::CK_STATECHUNK_DATAVERSION> CK_STATECHUNK_DATAVERSION {
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNKDATA_OLDVERSION, {"CHUNKDATA_OLDVERSION"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNKDATA_BASEVERSION, {"CHUNKDATA_BASEVERSION"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_WAVESOUND_VERSION2, {"CHUNK_WAVESOUND_VERSION2"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_WAVESOUND_VERSION3, {"CHUNK_WAVESOUND_VERSION3"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_MATERIAL_VERSION_ZTEST, {"CHUNK_MATERIAL_VERSION_ZTEST"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_MAJORCHANGE_VERSION, {"CHUNK_MAJORCHANGE_VERSION"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_MACCHANGE_VERSION, {"CHUNK_MACCHANGE_VERSION"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_WAVESOUND_VERSION4, {"CHUNK_WAVESOUND_VERSION4"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_SCENECHANGE_VERSION, {"CHUNK_SCENECHANGE_VERSION"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_MESHCHANGE_VERSION, {"CHUNK_MESHCHANGE_VERSION"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNK_DEV_2_1, {"CHUNK_DEV_2_1"} },
			{ LibCmo::CK2::CK_STATECHUNK_DATAVERSION::CHUNKDATA_CURRENTVERSION, {"CHUNKDATA_CURRENTVERSION"} },
		};
		const GeneralReflectionArray<LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION> CK_STATECHUNK_CHUNKVERSION {
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSIONBASE, {"CHUNK_VERSIONBASE"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION1, {"CHUNK_VERSION1"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION2, {"CHUNK_VERSION2"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION3, {"CHUNK_VERSION3"} },
			{ LibCmo::CK2::CK_STATECHUNK_CHUNKVERSION::CHUNK_VERSION4, {"CHUNK_VERSION4"} },
		};

	}

#pragma endregion

}
