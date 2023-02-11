#pragma once

#include <cinttypes>
#include <cstdint>
#include <cstdarg>
#include <type_traits>
#include <string>
#include <vector>

namespace LibCmo {

    namespace EnumHelper {
        template<typename TEnum>
        inline TEnum FlagEnumAdd(TEnum e, ...) {
            TEnum result = e;
            va_list argptr;
            va_start(argptr, e);
            result = static_cast<TEnum>(static_cast<std::underlying_type_t<TEnum>>(result) | static_cast<std::underlying_type_t<TEnum>>(va_arg(argptr, TEnum)));
            va_end(argptr);
            return result;
        }

        template<typename TEnum>
        inline bool FlagEnumHas(TEnum e, TEnum probe) {
            return static_cast<bool>(static_cast<std::underlying_type_t<TEnum>>(e) & static_cast<std::underlying_type_t<TEnum>>(probe));
        }
    }

    using CKINT = int32_t;
    using CK_ID = uint32_t;
    using CKDWORD = uint32_t;
    using CKBOOL = int32_t;
    using CKMUTSTRING = char*;
    using CKSTRING = const char*;

    using XString = std::string;
    using XBitArray = std::vector<bool>;
    template<typename T>
    using XArray = std::vector<T>;
    using XIntArray = std::vector<int32_t>;
    template<typename T>
    using XClassArray = std::vector<T>;
    //using CKObjectArray = std::vector<CKObject*>;

    enum class CK_CLASSID : uint32_t {
        CKCID_OBJECT = 1,
        CKCID_PARAMETERIN = 2,
        CKCID_PARAMETEROPERATION = 4,
        CKCID_STATE = 5,
        CKCID_BEHAVIORLINK = 6,
        CKCID_BEHAVIOR = 8,
        CKCID_BEHAVIORIO = 9,
        CKCID_RENDERCONTEXT = 12,
        CKCID_KINEMATICCHAIN = 13,
        CKCID_SCENEOBJECT = 11,
        CKCID_OBJECTANIMATION = 15,
        CKCID_ANIMATION = 16,
        CKCID_KEYEDANIMATION = 18,
        CKCID_BEOBJECT = 19,
        CKCID_DATAARRAY = 52,
        CKCID_SCENE = 10,
        CKCID_LEVEL = 21,
        CKCID_PLACE = 22,
        CKCID_GROUP = 23,
        CKCID_SOUND = 24,
        CKCID_WAVESOUND = 25,
        CKCID_MIDISOUND = 26,
        CKCID_MATERIAL = 30,
        CKCID_TEXTURE = 31,
        CKCID_MESH = 32,
        CKCID_PATCHMESH = 53,
        CKCID_RENDEROBJECT = 47,
        CKCID_2DENTITY = 27,
        CKCID_SPRITE = 28,
        CKCID_SPRITETEXT = 29,
        CKCID_3DENTITY = 33,
        CKCID_GRID = 50,
        CKCID_CURVEPOINT = 36,
        CKCID_SPRITE3D = 37,
        CKCID_CURVE = 43,
        CKCID_CAMERA = 34,
        CKCID_TARGETCAMERA = 35,
        CKCID_LIGHT = 38,
        CKCID_TARGETLIGHT = 39,
        CKCID_CHARACTER = 40,
        CKCID_3DOBJECT = 41,
        CKCID_BODYPART = 42,
        CKCID_PARAMETER = 46,
        CKCID_PARAMETERLOCAL = 45,
        CKCID_PARAMETEROUT = 3,
        CKCID_INTERFACEOBJECTMANAGER = 48,
        CKCID_CRITICALSECTION = 49,
        CKCID_LAYER = 51,
        CKCID_PROGRESSIVEMESH = 54,
        CKCID_SYNCHRO = 20,

        CKCID_OBJECTARRAY = 80,
        CKCID_SCENEOBJECTDESC = 81,
        CKCID_ATTRIBUTEMANAGER = 82,
        CKCID_MESSAGEMANAGER = 83,
        CKCID_COLLISIONMANAGER = 84,
        CKCID_OBJECTMANAGER = 85,
        CKCID_FLOORMANAGER = 86,
        CKCID_RENDERMANAGER = 87,
        CKCID_BEHAVIORMANAGER = 88,
        CKCID_INPUTMANAGER = 89,
        CKCID_PARAMETERMANAGER = 90,
        CKCID_GRIDMANAGER = 91,
        CKCID_SOUNDMANAGER = 92,
        CKCID_TIMEMANAGER = 93,
        CKCID_CUIKBEHDATA = static_cast<uint32_t>(-1),

        CKCID_MAXCLASSID = 55,
        CKCID_MAXMAXCLASSID = 128
	};

    enum class CKERROR : int32_t {
        CKERR_OK = 0,
        CKERR_INVALIDPARAMETER = -1,
        CKERR_INVALIDPARAMETERTYPE = -2,
        CKERR_INVALIDSIZE = -3,
        CKERR_INVALIDOPERATION = -4,
        CKERR_OPERATIONNOTIMPLEMENTED = -5,
        CKERR_OUTOFMEMORY = -6,
        CKERR_NOTIMPLEMENTED = -7,
        CKERR_NOTFOUND = -11,
        CKERR_NOLEVEL = -13,
        CKERR_CANCREATERENDERCONTEXT = -14,
        CKERR_NOTIFICATIONNOTHANDLED = -16,
        CKERR_ALREADYPRESENT = -17,
        CKERR_INVALIDRENDERCONTEXT = -18,
        CKERR_RENDERCONTEXTINACTIVE = -19,
        CKERR_NOLOADPLUGINS = -20,
        CKERR_NOSAVEPLUGINS = -21,
        CKERR_INVALIDFILE = -22,
        CKERR_INVALIDPLUGIN = -23,
        CKERR_NOTINITIALIZED = -24,
        CKERR_INVALIDMESSAGE = -25,
        CKERR_INVALIDPROTOTYPE = -28,
        CKERR_NODLLFOUND = -29,
        CKERR_ALREADYREGISTREDDLL = -30,
        CKERR_INVALIDDLL = -31,
        CKERR_INVALIDOBJECT = -34,
        CKERR_INVALIDCONDSOLEWINDOW = -35,
        CKERR_INVALIDKINEMATICCHAIN = -36,
        CKERR_NOKEYBOARD = -37,
        CKERR_NOMOUSE = -38,
        CKERR_NOJOYSTICK = -39,
        CKERR_INCOMPATIBLEPARAMETERS = -40,
        CKERR_NORENDERENGINE = -44,
        CKERR_NOCURRENTLEVEL = -45,
        CKERR_SOUNDDISABLED = -46,
        CKERR_DINPUTDISABLED = -47,
        CKERR_INVALIDGUID = -48,
        CKERR_NOTENOUGHDISKPLACE = -49,
        CKERR_CANTWRITETOFILE = -50,
        CKERR_BEHAVIORADDDENIEDBYCB = -51,
        CKERR_INCOMPATIBLECLASSID = -52,
        CKERR_MANAGERALREADYEXISTS = -53,
        CKERR_PAUSED = -54,
        CKERR_PLUGINSMISSING = -55,
        CKERR_OBSOLETEVIRTOOLS = -56,
        CKERR_FILECRCERROR = -57,
        CKERR_ALREADYFULLSCREEN = -58,
        CKERR_CANCELLED = -59,
        CKERR_NOANIMATIONKEY = -121,
        CKERR_INVALIDINDEX = -122,
        CKERR_INVALIDANIMATION = -123
    };

    enum class CK_FILE_WRITEMODE : int32_t {
        CKFILE_UNCOMPRESSED = 0,
        CKFILE_CHUNKCOMPRESSED_OLD = 1,
        CKFILE_EXTERNALTEXTURES_OLD = 2,
        CKFILE_FORVIEWER = 4,
        CKFILE_WHOLECOMPRESSED = 8
    };
    enum class CK_LOAD_FLAGS : int32_t {
        CK_LOAD_ANIMATION = 1 << 0,
        CK_LOAD_GEOMETRY = 1 << 1,
        CK_LOAD_DEFAULT = CK_LOAD_GEOMETRY | CK_LOAD_ANIMATION,
        CK_LOAD_ASCHARACTER = 1 << 2,
        CK_LOAD_DODIALOG = 1 << 3,
        CK_LOAD_AS_DYNAMIC_OBJECT = 1 << 4,
        CK_LOAD_AUTOMATICMODE = 1 << 5,
        CK_LOAD_CHECKDUPLICATES = 1 << 6,
        CK_LOAD_CHECKDEPENDENCIES = 1 << 7,
        CK_LOAD_ONLYBEHAVIORS = 1 << 8
    };
    enum class CK_FO_OPTIONS : int32_t {
        CK_FO_DEFAULT = 0,
        CK_FO_RENAMEOBJECT,
        CK_FO_REPLACEOBJECT,
        CK_FO_DONTLOADOBJECT
    };

}

