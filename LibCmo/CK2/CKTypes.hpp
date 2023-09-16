#pragma once

#include "../VTUtils.hpp"
#include <string>
#include <vector>
#include <cstring>
#include <cinttypes>
#include <compare>

// ========== Basic Types Section ==========

namespace LibCmo {
	
	// Types.
	// These types is general types used in every module.
	// So we declare them in LibCmo, not LibCmo::CK2 to make sure every module can use it.

	/**
	 * @brief General Const String Type. Encoding Unrelated.
	*/
	using CKSTRING = const char*;
	/**
	 * @brief Changeble CKSTRING.
	 * @see CKSTRING
	*/
	using CKMUTSTRING = char*;
	/**
	 * @brief The Representation of Single Character (1 byte). Encoding Unrelated.
	 * @remark
	 * + Only used with string process.
	 * + For memory representation and moving, use CKBYTE instead.
	 * @see CKBYTE
	*/
	using CKCHAR = char;

	/**
	 * @brief Always Represent a Byte (1 byte, unsigned). Platform Independent.
	 * @remark
	 * + This type should only be used when representing memory data or position.
	 * + If you want to represent a char, or a sequence of chars, use CKCHAR instead.
	 * @see CKCHAR
	*/
	using CKBYTE = uint8_t;
	/**
	 * @brief Always Represent a WORD (2 byte, unsigned). Platform Independent.
	*/
	using CKWORD = uint16_t;
	/**
	 * @brief Always Represent a DWORD (4 byte, unsigned). Platform Independent.
	 * @see CKINT
	*/
	using CKDWORD = uint32_t;
	/**
	 * @brief Always Represent a QWORD (8 byte, unsigned). Platform Independent.
	*/
	using CKQWORD = uint64_t;

	/**
	 * @brief The Int type used in LibCmo.
	 * @remark
	 * + All 'int' type in original Virtools SDK should be replaced with CKINT in this project if needed.
	 * + This type also can be seen as the equvalent of signed CKDWORD.
	 * @see CKDWORD
	*/
	using CKINT = int32_t;

	/**
	 * @brief Always Represent a float (32 bit). Platform Independent.
	*/
	using CKFLOAT = float;
	/**
	 * @brief Always Represent a double (64 bit). Platform Independent.
	*/
	using CKDOUBLE = double;

	/**
	 * @brief The bool type used by LibCmo.
	 * Generally it is just C++ bool.
	*/
	using CKBOOL = bool;
	/**
	 * @brief The True value of CKBOOL.
	*/
	constexpr CKBOOL CKTRUE = true;
	/**
	 * @brief The False value of CKBOOL.
	*/
	constexpr CKBOOL CKFALSE = false;

	/**
	 * @brief Represent a x86 Platform Pointer.
	 * @remark
	 * + This type only can be used when replacing pointer in old Virtools struct / class.
	 * + Due to Virtools shitty design, in some cases we need read data with x86 memory layout from file.
	 * So we use this type to replace native pointer in struct existed in Virtools SDK to make sure this 
	 * program can run perfectly on x64 and more architectures.
	 * + A example can be found in CKTexture::Load().
	*/
	using CKPTR = uint32_t;
	
	// Format constants for the std::fprintf family of functions

#define PRI_CKSTRING "s"
#define PRI_CKCHAR "c"

#define PRIuCKBYTE PRIu8
#define PRIuCKWORD PRIu16
#define PRIuCKDWORD PRIu32
#define PRIuCKQWORD PRIu64

#define PRIxCKBYTE PRIx8
#define PRIxCKWORD PRIx16
#define PRIxCKDWORD PRIx32
#define PRIxCKQWORD PRIx64

#define PRIXCKBYTE PRIX8
#define PRIXCKWORD PRIX16
#define PRIXCKDWORD PRIX32
#define PRIXCKQWORD PRIX64

#define PRIiCKINT PRIi32

#define PRIxCKPTR PRIx32
#define PRIXCKPTR PRIX32

	/*
	The convenient sizeof which return CKDWORD, not size_t.
	*/
#define CKSizeof(_Ty) (static_cast<LibCmo::CKDWORD>(sizeof(_Ty)))

}

// ========== CK2 Section ==========

namespace LibCmo::CK2 {

	/**
	@brief Unique Identifier for all Objects instanciated in a given CKContext
	@remarks
	+ Each instance of CKObject and derived classes are automatically given a global unique
	ID at creation time. This ID can be accessed through the CKObject::GetID method.
	It is safer, though a bit slower, to reference object through their global ID than through
	a direct pointer reference. In any case the referenced object may be deleted even though
	the client object has a ID for it. The client object should verify that the referenced object
	still exists when used with the CKGetObject function.
	+ The global ID for an instance remains unique and unchanged through a application session, but there
	is no garanty that this ID will be the same when a level is saved and loaded back again.
	@see CKObject::GetID, CKContext::GetObject
	*/
	using CK_ID = CKDWORD;

	enum class CKERROR : CKINT {
		CKERR_OK = 0,	/**< Operation successful  */
		CKERR_INVALIDPARAMETER = -1,	/**< One of the parameter passed to the function was invalid  */
		CKERR_INVALIDPARAMETERTYPE = -2,	/**< One of the parameter passed to the function was invalid  */
		CKERR_INVALIDSIZE = -3,	/**< The parameter size was invalid  */
		CKERR_INVALIDOPERATION = -4,	/**< The operation type didn't exist  */
		CKERR_OPERATIONNOTIMPLEMENTED = -5,	/**< The function used to execute the operation is not yet implemented  */
		CKERR_OUTOFMEMORY = -6,	/**< There was not enough memory to perform the action  */
		CKERR_NOTIMPLEMENTED = -7,	/**< The function  is not yet implemented  */
		CKERR_NOTFOUND = -11,	/**< There was an attempt to remove something not present  */
		CKERR_NOLEVEL = -13,	/**< There is no level currently created  */
		CKERR_CANCREATERENDERCONTEXT = -14,	/**<   */
		CKERR_NOTIFICATIONNOTHANDLED = -16,	/**< The notification message was not used  */
		CKERR_ALREADYPRESENT = -17,	/**< Attempt to add an item that was already present  */
		CKERR_INVALIDRENDERCONTEXT = -18,	/**< the render context is not valid  */
		CKERR_RENDERCONTEXTINACTIVE = -19,	/**< the render context is not activated for rendering  */
		CKERR_NOLOADPLUGINS = -20,	/**< there was no plugins to load this kind of file  */
		CKERR_NOSAVEPLUGINS = -21,	/**< there was no plugins to save this kind of file  */
		CKERR_INVALIDFILE = -22,	/**< attempt to load an invalid file  */
		CKERR_INVALIDPLUGIN = -23,	/**< attempt to load with an invalid plugin  */
		CKERR_NOTINITIALIZED = -24,	/**< attempt use an object that wasnt initialized  */
		CKERR_INVALIDMESSAGE = -25,	/**< attempt use a message type that wasn't registred  */
		CKERR_INVALIDPROTOTYPE = -28,	/**< attempt use an invalid prototype  */
		CKERR_NODLLFOUND = -29,	/**< No dll file found in the parse directory  */
		CKERR_ALREADYREGISTREDDLL = -30,	/**< this dll has already been registred  */
		CKERR_INVALIDDLL = -31,	/**< this dll does not contain information to create the prototype  */
		CKERR_INVALIDOBJECT = -34,	/**< Invalid Object (attempt to Get an object from an invalid ID)  */
		CKERR_INVALIDCONDSOLEWINDOW = -35,	/**< Invalid window was provided as console window  */
		CKERR_INVALIDKINEMATICCHAIN = -36,	/**< Invalid kinematic chain ( end and start effector may not be part of the same hierarchy )  */
		CKERR_NOKEYBOARD = -37,	/**< Keyboard not attached or not working properly  */
		CKERR_NOMOUSE = -38,	/**< Mouse not attached or not working properly  */
		CKERR_NOJOYSTICK = -39,	/**< Joystick not attached or not working properly  */
		CKERR_INCOMPATIBLEPARAMETERS = -40,	/**< Try to link imcompatible Parameters  */
		CKERR_NORENDERENGINE = -44,	/**< There is no render engine dll  */
		CKERR_NOCURRENTLEVEL = -45,	/**< There is no current level (use CKSetCurrentLevel )  */
		CKERR_SOUNDDISABLED = -46,	/**< Sound Management has been disabled  */
		CKERR_DINPUTDISABLED = -47,	/**< DirectInput Management has been disabled  */
		CKERR_INVALIDGUID = -48,	/**< Guid is already in use or invalid  */
		CKERR_NOTENOUGHDISKPLACE = -49,	/**< There was no more free space on disk when trying to save a file  */
		CKERR_CANTWRITETOFILE = -50,	/**< Impossible to write to file (write-protection ?)  */
		CKERR_BEHAVIORADDDENIEDBYCB = -51,	/**< The behavior cannnot be added to this entity  */
		CKERR_INCOMPATIBLECLASSID = -52,	/**< The behavior cannnot be added to this entity  */
		CKERR_MANAGERALREADYEXISTS = -53,	/**< A manager was registered more than once  */
		CKERR_PAUSED = -54,	/**< CKprocess or TimeManager process while CK is paused will fail  */
		CKERR_PLUGINSMISSING = -55,	/**< Some plugins were missing whileloading a file  */
		CKERR_OBSOLETEVIRTOOLS = -56,	/**< Virtools version too old to load this file  */
		CKERR_FILECRCERROR = -57,	/**< CRC Error while loading file  */
		CKERR_ALREADYFULLSCREEN = -58,	/**< A Render context is already in Fullscreen Mode  */
		CKERR_CANCELLED = -59,	/**< Operation was cancelled by user  */
		CKERR_NOANIMATIONKEY = -121,	/**< there were no animation key at the given index  */
		CKERR_INVALIDINDEX = -122,	/**< attemp to acces an animation key with an invalid index  */
		CKERR_INVALIDANIMATION = -123,	/**< the animation is invalid (no entity associated or zero length)  */
	};

	/**
	@brief Per Class Unique Identifier.
	@remark
	+ Each class derived from the CKObject class has a unique class ID.
	+ This ID can be accessed through each instance of these classes, with the
	CKObject::GetClassID method.
	+ This class ID is used internally for various matching operations, like matching behaviors on
	objects, etc..
	@see CKObject::GetClassID, CKIsChildClassOf, Class Identifiers
	*/
	enum class CK_CLASSID : CKINT {
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
		CKCID_CUIKBEHDATA = -1,
		CKCID_MAXCLASSID = 55,
		CKCID_MAXMAXCLASSID = 128,
	};

	// ========== Type Definition ==========
	// type define

	using CKParameterType = CKINT;
	using CKOperationType = CKINT;
	using CKMessageType = CKINT;
	using CKAttributeType = CKINT;
	using CKAttributeCategory = CKINT;
	
	// format constant
#define PRIuCKID PRIuCKDWORD
#define PRIiCKERROR PRIiCKINT
#define PRIiCLASSID PRIiCKINT

	// ========== Class List ==========
	// Objects and derivated classes

	namespace ObjImpls {
		class CKObject;
		class CKInterfaceObjectManager;
		class CKRenderContext;
		class CKParameterIn;
		class CKParameter;
		class CKParameterOut;
		class CKParameterLocal;
		class CKParameterOperation;
		class CKBehaviorLink;
		class CKBehaviorIO;
		class CKRenderContext;
		class CKSynchroObject;
		class CKStateObject;
		class CKCriticalSectionObject;
		class CKKinematicChain;
		class CKObjectAnimation;
		class CKLayer;
		class CKSceneObject;
		class CKBehavior;
		class CKAnimation;
		class CKKeyedAnimation;
		class CKBeObject;
		class CKScene;
		class CKLevel;
		class CKPlace;
		class CKGroup;
		class CKMaterial;
		class CKTexture;
		class CKMesh;
		class CKPatchMesh;
		class CKProgressiveMesh;
		class CKDataArray;
		class CKSound;
		class CKMidiSound;
		class CKWaveSound;
		class CKRenderObject;
		class CK2dEntity;
		class CKSprite;
		class CKSpriteText;
		class CK3dEntity;
		class CKCamera;
		class CKTargetCamera;
		class CKCurvePoint;
		class CKSprite3D;
		class CKLight;
		class CKTargetLight;
		class CKCharacter;
		class CK3dObject;
		class CKBodyPart;
		class CKCurve;
		class CKGrid;
	}

	//---- Misc
	class CKBehaviorPrototype;
	class CKMessage;
	class CK2dCurvePoint;
	class CK2dCurve;
	//class CKStateChunk;
	//class CKFile;
	class CKDependencies;
	class CKDependenciesContext;
	class CKDebugContext;
	class CKObjectArray;
	class CKObjectDeclaration;
	//class CKContext;
	class CKBitmapProperties;
	class CKFileExtension;
	class CKVertexBuffer;

	//--- Managers
	namespace MgrImpls {
		class CKBaseManager;
		class CKObjectManager;
		class CKSoundManager;
		class CKTimeManager;
		class CKRenderManager;
		class CKBehaviorManager;
		class CKMessageManager;
		class CKParameterManager;
		class CKAttributeManager;
		class CKPathManager;
		class CKVariableManager;
		class CKSceneObjectDesc;
		class CKPluginManager;
	}

	namespace DataHandlers {
		class CKBitmapHandler;
		class CKMovieHandler;
		class CKSoundHandler;
	}

	//--- Important classes
	class CKContext;
	class CKStateChunk;
	class CKFileReader;
	class CKFileWriter;
	class CKFileVisitor;

	/**
	@brief Global Unique Identifier Struture.
	@remark
	+ Guids are used to uniquely identify plugins,operation types, parameter types and behavior prototypes.
	+ Its defined as
	```
	typedef struct CKGUID {
		union {
			struct { CKDWORD d1,d2; };
			CKDWORD d[2];
			};
	};
	```
	+ Comparison operators are defined so CKGUIDS can be compared with
	==, != , <, > operators.

	@see Pre-Registred Parameter Types, ParameterOperation Types
	*/
	struct CKGUID {
		CKDWORD d1, d2;

		constexpr CKGUID(CKDWORD gd1 = 0, CKDWORD gd2 = 0) : d1(gd1), d2(gd2) {}
		CKGUID(const CKGUID& rhs) : d1(rhs.d1), d2(rhs.d2) {}
		CKGUID& operator=(const CKGUID& rhs) {
			this->d1 = rhs.d1;
			this->d2 = rhs.d2;

			return *this;
		}

		auto operator<=>(const CKGUID& rhs) const {
			auto cmp = this->d1 <=> rhs.d1;
			if (cmp != 0)
				return cmp;
			return this->d2 <=> rhs.d2;
		}
		bool operator==(const CKGUID& rhs) const {
			return ((this->d1 == rhs.d1) && (this->d2 == rhs.d2));
		}
	};

}
