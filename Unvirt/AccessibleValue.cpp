#include "AccessibleValue.hpp"
#include "StringHelper.hpp"
#include <vector>
#include <array>

namespace Unvirt {
	namespace AccessibleValue {

		template<typename TKey, typename TValue>
		static inline const TValue* GetEnumData(const std::vector<std::pair<TKey, TValue>>& vec, const TKey key) {
			size_t len = vec.size();
			for (size_t i = 0; i < len; ++i) {
				if (key == vec[i].first) return &vec[i].second;
			}
			return nullptr;
		}

		static const std::vector<std::pair<LibCmo::CKERROR, std::array<const char*, 2>>> _CkErrorData{
			{ LibCmo::CKERROR::CKERR_OK, { "CKERR_OK", "Operation successful" } },
			{ LibCmo::CKERROR::CKERR_INVALIDPARAMETER, {"CKERR_INVALIDPARAMETER", "One of the parameter passed to the function was invalid"} },
			{ LibCmo::CKERROR::CKERR_INVALIDPARAMETERTYPE, {"CKERR_INVALIDPARAMETERTYPE", "One of the parameter passed to the function was invalid"} },
			{ LibCmo::CKERROR::CKERR_INVALIDSIZE, {"CKERR_INVALIDSIZE", "The parameter size was invalid"} },
			{ LibCmo::CKERROR::CKERR_INVALIDOPERATION, {"CKERR_INVALIDOPERATION", "The operation type didn't exist"} },
			{ LibCmo::CKERROR::CKERR_OPERATIONNOTIMPLEMENTED, {"CKERR_OPERATIONNOTIMPLEMENTED", "The function used to execute the operation is not yet implemented"} },
			{ LibCmo::CKERROR::CKERR_OUTOFMEMORY, {"CKERR_OUTOFMEMORY", "There was not enough memory to perform the action"} },
			{ LibCmo::CKERROR::CKERR_NOTIMPLEMENTED, {"CKERR_NOTIMPLEMENTED", "The function  is not yet implemented"} },
			{ LibCmo::CKERROR::CKERR_NOTFOUND, {"CKERR_NOTFOUND", "There was an attempt to remove something not present"} },
			{ LibCmo::CKERROR::CKERR_NOLEVEL, {"CKERR_NOLEVEL", "There is no level currently created"} },
			{ LibCmo::CKERROR::CKERR_CANCREATERENDERCONTEXT, {"CKERR_CANCREATERENDERCONTEXT", ""} },
			{ LibCmo::CKERROR::CKERR_NOTIFICATIONNOTHANDLED, {"CKERR_NOTIFICATIONNOTHANDLED", "The notification message was not used"} },
			{ LibCmo::CKERROR::CKERR_ALREADYPRESENT, {"CKERR_ALREADYPRESENT", "Attempt to add an item that was already present"} },
			{ LibCmo::CKERROR::CKERR_INVALIDRENDERCONTEXT, {"CKERR_INVALIDRENDERCONTEXT", "the render context is not valid"} },
			{ LibCmo::CKERROR::CKERR_RENDERCONTEXTINACTIVE, {"CKERR_RENDERCONTEXTINACTIVE", "the render context is not activated for rendering"} },
			{ LibCmo::CKERROR::CKERR_NOLOADPLUGINS, {"CKERR_NOLOADPLUGINS", "there was no plugins to load this kind of file"} },
			{ LibCmo::CKERROR::CKERR_NOSAVEPLUGINS, {"CKERR_NOSAVEPLUGINS", "there was no plugins to save this kind of file"} },
			{ LibCmo::CKERROR::CKERR_INVALIDFILE, {"CKERR_INVALIDFILE", "attempt to load an invalid file"} },
			{ LibCmo::CKERROR::CKERR_INVALIDPLUGIN, {"CKERR_INVALIDPLUGIN", "attempt to load with an invalid plugin"} },
			{ LibCmo::CKERROR::CKERR_NOTINITIALIZED, {"CKERR_NOTINITIALIZED", "attempt use an object that wasnt initialized"} },
			{ LibCmo::CKERROR::CKERR_INVALIDMESSAGE, {"CKERR_INVALIDMESSAGE", "attempt use a message type that wasn't registred"} },
			{ LibCmo::CKERROR::CKERR_INVALIDPROTOTYPE, {"CKERR_INVALIDPROTOTYPE", "attempt use an invalid prototype"} },
			{ LibCmo::CKERROR::CKERR_NODLLFOUND, {"CKERR_NODLLFOUND", "No dll file found in the parse directory"} },
			{ LibCmo::CKERROR::CKERR_ALREADYREGISTREDDLL, {"CKERR_ALREADYREGISTREDDLL", "this dll has already been registred"} },
			{ LibCmo::CKERROR::CKERR_INVALIDDLL, {"CKERR_INVALIDDLL", "this dll does not contain information to create the prototype"} },
			{ LibCmo::CKERROR::CKERR_INVALIDOBJECT, {"CKERR_INVALIDOBJECT", "Invalid Object (attempt to Get an object from an invalid ID)"} },
			{ LibCmo::CKERROR::CKERR_INVALIDCONDSOLEWINDOW, {"CKERR_INVALIDCONDSOLEWINDOW", "Invalid window was provided as console window"} },
			{ LibCmo::CKERROR::CKERR_INVALIDKINEMATICCHAIN, {"CKERR_INVALIDKINEMATICCHAIN", "Invalid kinematic chain ( end and start effector may not be part of the same hierarchy )"} },
			{ LibCmo::CKERROR::CKERR_NOKEYBOARD, {"CKERR_NOKEYBOARD", "Keyboard not attached or not working properly"} },
			{ LibCmo::CKERROR::CKERR_NOMOUSE, {"CKERR_NOMOUSE", "Mouse not attached or not working properly"} },
			{ LibCmo::CKERROR::CKERR_NOJOYSTICK, {"CKERR_NOJOYSTICK", "Joystick not attached or not working properly"} },
			{ LibCmo::CKERROR::CKERR_INCOMPATIBLEPARAMETERS, {"CKERR_INCOMPATIBLEPARAMETERS", "Try to link imcompatible Parameters"} },
			{ LibCmo::CKERROR::CKERR_NORENDERENGINE, {"CKERR_NORENDERENGINE", "There is no render engine dll"} },
			{ LibCmo::CKERROR::CKERR_NOCURRENTLEVEL, {"CKERR_NOCURRENTLEVEL", "There is no current level (use CKSetCurrentLevel )"} },
			{ LibCmo::CKERROR::CKERR_SOUNDDISABLED, {"CKERR_SOUNDDISABLED", "Sound Management has been disabled"} },
			{ LibCmo::CKERROR::CKERR_DINPUTDISABLED, {"CKERR_DINPUTDISABLED", "DirectInput Management has been disabled"} },
			{ LibCmo::CKERROR::CKERR_INVALIDGUID, {"CKERR_INVALIDGUID", "Guid is already in use or invalid"} },
			{ LibCmo::CKERROR::CKERR_NOTENOUGHDISKPLACE, {"CKERR_NOTENOUGHDISKPLACE", "There was no more free space on disk when trying to save a file"} },
			{ LibCmo::CKERROR::CKERR_CANTWRITETOFILE, {"CKERR_CANTWRITETOFILE", "Impossible to write to file (write-protection ?)"} },
			{ LibCmo::CKERROR::CKERR_BEHAVIORADDDENIEDBYCB, {"CKERR_BEHAVIORADDDENIEDBYCB", "The behavior cannnot be added to this entity"} },
			{ LibCmo::CKERROR::CKERR_INCOMPATIBLECLASSID, {"CKERR_INCOMPATIBLECLASSID", "The behavior cannnot be added to this entity"} },
			{ LibCmo::CKERROR::CKERR_MANAGERALREADYEXISTS, {"CKERR_MANAGERALREADYEXISTS", "A manager was registered more than once"} },
			{ LibCmo::CKERROR::CKERR_PAUSED, {"CKERR_PAUSED", "CKprocess or TimeManager process while CK is paused will fail"} },
			{ LibCmo::CKERROR::CKERR_PLUGINSMISSING, {"CKERR_PLUGINSMISSING", "Some plugins were missing whileloading a file"} },
			{ LibCmo::CKERROR::CKERR_OBSOLETEVIRTOOLS, {"CKERR_OBSOLETEVIRTOOLS", "Virtools version too old to load this file"} },
			{ LibCmo::CKERROR::CKERR_FILECRCERROR, {"CKERR_FILECRCERROR", "CRC Error while loading file"} },
			{ LibCmo::CKERROR::CKERR_ALREADYFULLSCREEN, {"CKERR_ALREADYFULLSCREEN", "A Render context is already in Fullscreen Mode"} },
			{ LibCmo::CKERROR::CKERR_CANCELLED, {"CKERR_CANCELLED", "Operation was cancelled by user"} },
			{ LibCmo::CKERROR::CKERR_NOANIMATIONKEY, {"CKERR_NOANIMATIONKEY", "there were no animation key at the given index"} },
			{ LibCmo::CKERROR::CKERR_INVALIDINDEX, {"CKERR_INVALIDINDEX", "attemp to acces an animation key with an invalid index"} },
			{ LibCmo::CKERROR::CKERR_INVALIDANIMATION, {"CKERR_INVALIDANIMATION", "the animation is invalid (no entity associated or zero length)"} }
		};

		void GetCkErrorName(std::string& strl, LibCmo::CKERROR err) {
			strl.clear();
			const std::array<const char*, 2>* pErrDesc = GetEnumData<LibCmo::CKERROR, std::array<const char*, 2>>(
				_CkErrorData, err
				);

			if (pErrDesc != nullptr) {
				strl = pErrDesc->front();
			}
		}

		void GetCkErrorDescription(std::string& strl, LibCmo::CKERROR err) {
			strl.clear();
			const std::array<const char*, 2>* pErrDesc = GetEnumData<LibCmo::CKERROR, std::array<const char*, 2>>(
				_CkErrorData, err
				);

			if (pErrDesc != nullptr) {
				strl = pErrDesc->back();
			}
		}

		static const std::vector<std::pair<LibCmo::CK_CLASSID, std::vector<const char*>>> _CkClassHierarchy{
			{ LibCmo::CK_CLASSID::CKCID_OBJECT, {"CKCID_OBJECT"} },
			{ LibCmo::CK_CLASSID::CKCID_PARAMETERIN, {"CKCID_OBJECT", "CKCID_PARAMETERIN"} },
			{ LibCmo::CK_CLASSID::CKCID_PARAMETEROPERATION, {"CKCID_OBJECT", "CKCID_PARAMETEROPERATION"} },
			{ LibCmo::CK_CLASSID::CKCID_STATE, {"CKCID_OBJECT", "CKCID_STATE"} },
			{ LibCmo::CK_CLASSID::CKCID_BEHAVIORLINK, {"CKCID_OBJECT", "CKCID_BEHAVIORLINK"} },
			{ LibCmo::CK_CLASSID::CKCID_BEHAVIOR, {"CKCID_OBJECT", "CKCID_BEHAVIOR"} },
			{ LibCmo::CK_CLASSID::CKCID_BEHAVIORIO, {"CKCID_OBJECT", "CKCID_BEHAVIORIO"} },
			{ LibCmo::CK_CLASSID::CKCID_RENDERCONTEXT, {"CKCID_OBJECT", "CKCID_RENDERCONTEXT"} },
			{ LibCmo::CK_CLASSID::CKCID_KINEMATICCHAIN, {"CKCID_OBJECT", "CKCID_KINEMATICCHAIN"} },
			{ LibCmo::CK_CLASSID::CKCID_SCENEOBJECT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT"} },
			{ LibCmo::CK_CLASSID::CKCID_OBJECTANIMATION, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_OBJECTANIMATION"} },
			{ LibCmo::CK_CLASSID::CKCID_ANIMATION, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_ANIMATION"} },
			{ LibCmo::CK_CLASSID::CKCID_KEYEDANIMATION, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_ANIMATION", "CKCID_KEYEDANIMATION"} },
			{ LibCmo::CK_CLASSID::CKCID_BEOBJECT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT"} },
			{ LibCmo::CK_CLASSID::CKCID_DATAARRAY, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_DATAARRAY"} },
			{ LibCmo::CK_CLASSID::CKCID_SCENE, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SCENE"} },
			{ LibCmo::CK_CLASSID::CKCID_LEVEL, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_LEVEL"} },
			{ LibCmo::CK_CLASSID::CKCID_PLACE, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_PLACE"} },
			{ LibCmo::CK_CLASSID::CKCID_GROUP, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_GROUP"} },
			{ LibCmo::CK_CLASSID::CKCID_SOUND, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND"} },
			{ LibCmo::CK_CLASSID::CKCID_WAVESOUND, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND", "CKCID_WAVESOUND"} },
			{ LibCmo::CK_CLASSID::CKCID_MIDISOUND, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND", "CKCID_MIDISOUND"} },
			{ LibCmo::CK_CLASSID::CKCID_MATERIAL, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MATERIAL"} },
			{ LibCmo::CK_CLASSID::CKCID_TEXTURE, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_TEXTURE"} },
			{ LibCmo::CK_CLASSID::CKCID_MESH, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MESH"} },
			{ LibCmo::CK_CLASSID::CKCID_PATCHMESH, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MESH", "CKCID_PATCHMESH"} },
			{ LibCmo::CK_CLASSID::CKCID_RENDEROBJECT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT"} },
			{ LibCmo::CK_CLASSID::CKCID_2DENTITY, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY"} },
			{ LibCmo::CK_CLASSID::CKCID_SPRITE, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY", "CKCID_SPRITE"} },
			{ LibCmo::CK_CLASSID::CKCID_SPRITETEXT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY", "CKCID_SPRITETEXT"} },
			{ LibCmo::CK_CLASSID::CKCID_3DENTITY, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY"} },
			{ LibCmo::CK_CLASSID::CKCID_GRID, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_GRID"} },
			{ LibCmo::CK_CLASSID::CKCID_CURVEPOINT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CURVEPOINT"} },
			{ LibCmo::CK_CLASSID::CKCID_SPRITE3D, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_SPRITE3D"} },
			{ LibCmo::CK_CLASSID::CKCID_CURVE, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CURVE"} },
			{ LibCmo::CK_CLASSID::CKCID_CAMERA, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CAMERA"} },
			{ LibCmo::CK_CLASSID::CKCID_TARGETCAMERA, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CAMERA", "CKCID_TARGETCAMERA"} },
			{ LibCmo::CK_CLASSID::CKCID_LIGHT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_LIGHT"} },
			{ LibCmo::CK_CLASSID::CKCID_TARGETLIGHT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_LIGHT", "CKCID_TARGETLIGHT"} },
			{ LibCmo::CK_CLASSID::CKCID_CHARACTER, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CHARACTER"} },
			{ LibCmo::CK_CLASSID::CKCID_3DOBJECT, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_3DOBJECT"} },
			{ LibCmo::CK_CLASSID::CKCID_BODYPART, {"CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_3DOBJECT", "CKCID_BODYPART"} },
			{ LibCmo::CK_CLASSID::CKCID_PARAMETER, {"CKCID_OBJECT", "CKCID_PARAMETER"} },
			{ LibCmo::CK_CLASSID::CKCID_PARAMETERLOCAL, {"CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETERLOCAL"} },
			{ LibCmo::CK_CLASSID::CKCID_PARAMETERVARIABLE, {"CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETERLOCAL", "CKCID_PARAMETERVARIABLE"} },
			{ LibCmo::CK_CLASSID::CKCID_PARAMETEROUT, {"CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETEROUT"} },
			{ LibCmo::CK_CLASSID::CKCID_INTERFACEOBJECTMANAGER, {"CKCID_OBJECT", "CKCID_INTERFACEOBJECTMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_CRITICALSECTION, {"CKCID_OBJECT", "CKCID_CRITICALSECTION"} },
			{ LibCmo::CK_CLASSID::CKCID_LAYER, {"CKCID_OBJECT", "CKCID_LAYER"} },
			{ LibCmo::CK_CLASSID::CKCID_PROGRESSIVEMESH, {"CKCID_OBJECT", "CKCID_PROGRESSIVEMESH"} },
			{ LibCmo::CK_CLASSID::CKCID_SYNCHRO, {"CKCID_OBJECT", "CKCID_SYNCHRO"} },
			{ LibCmo::CK_CLASSID::CKCID_OBJECTARRAY, {"CKCID_OBJECTARRAY"} },
			{ LibCmo::CK_CLASSID::CKCID_SCENEOBJECTDESC, {"CKCID_SCENEOBJECTDESC"} },
			{ LibCmo::CK_CLASSID::CKCID_ATTRIBUTEMANAGER, {"CKCID_ATTRIBUTEMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_MESSAGEMANAGER, {"CKCID_MESSAGEMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_COLLISIONMANAGER, {"CKCID_COLLISIONMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_OBJECTMANAGER, {"CKCID_OBJECTMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_FLOORMANAGER, {"CKCID_FLOORMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_RENDERMANAGER, {"CKCID_RENDERMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_BEHAVIORMANAGER, {"CKCID_BEHAVIORMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_INPUTMANAGER, {"CKCID_INPUTMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_PARAMETERMANAGER, {"CKCID_PARAMETERMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_GRIDMANAGER, {"CKCID_GRIDMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_SOUNDMANAGER, {"CKCID_SOUNDMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_TIMEMANAGER, {"CKCID_TIMEMANAGER"} },
			{ LibCmo::CK_CLASSID::CKCID_CUIKBEHDATA, {"CKCID_CUIKBEHDATA"} },
			{ LibCmo::CK_CLASSID::CKCID_MAXCLASSID, {"CKCID_MAXCLASSID"} },
			{ LibCmo::CK_CLASSID::CKCID_MAXMAXCLASSID, {"CKCID_MAXMAXCLASSID"} }
		};

		void GetClassIdName(std::string& strl, LibCmo::CK_CLASSID cls) {
			strl.clear();
			const std::vector<const char*>* pHierarchy = GetEnumData<LibCmo::CK_CLASSID, std::vector<const char*>>(
				_CkClassHierarchy, cls
				);

			if (pHierarchy != nullptr) {
				strl = pHierarchy->back();
			}
		}

		void GetClassIdHierarchy(std::string& strl, LibCmo::CK_CLASSID cls) {
			strl.clear();
			const std::vector<const char*>* pHierarchy = GetEnumData<LibCmo::CK_CLASSID, std::vector<const char*>>(
				_CkClassHierarchy, cls
				);

			if (pHierarchy != nullptr) {
				for (auto it = pHierarchy->begin(); it != pHierarchy->end(); ++it) {
					if (it != pHierarchy->begin()) strl += " -> ";
					strl += (*it);
				}
			}
		}

		void GetAccessibleFileSize(std::string& strl, uint64_t size) {
			static double denominator = (double)0b1111111111;

			// check bytes
			if ((size >> 10) == UINT64_C(0)) {
				StringHelper::StdstringPrintf(strl, "%" PRIu64 "Bytes", size);
				return;
			}
			size >>= 10;

			// check kb
			if ((size >> 10) == UINT64_C(0)) {
				StringHelper::StdstringPrintf(strl, "%.2lfKiB", size / denominator);
				return;
			}
			size >>= 10;

			// check mb
			if ((size >> 10) == UINT64_C(0)) {
				StringHelper::StdstringPrintf(strl, "%.2lfMiB", size / denominator);
				return;
			}
			size >>= 10;

			// otherwise gb
			StringHelper::StdstringPrintf(strl, "%.2lfGiB", size / denominator);
			return;

		}

	}
}
