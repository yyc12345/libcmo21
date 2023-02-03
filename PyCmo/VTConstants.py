import enum

class CKEnumItem(object):
    def __init__(self, name: str, value: int):
        self.name: str = name
        self.value: int = value

class CKEnum(object):
    def __init__(self, val: int):
        self.m_Value: int = val

        self.__ItemsList = (
            CKEnumItem(attr, getattr(self, attr)) 
            for attr in dir(self) if not attr.startswith("__") and not callable(getattr(self, attr))
        )

    def Set(self, data: int):
        self.m_Value = data

    def __iter__(self):
        return self.__ItemsList

    def __repr__(self):
        for i in self:
            if i.value == self.m_Value:
                return '<' + i.name + '>'

        return "<[None] {:d}>".format(self.m_Value)

    def __str__(self):
        return self.__repr__()

class CKFlagEnum(CKEnum):
    def Add(self, data: int):
        self.m_Value = self.m_Value | data

    def __contains__(self, probe):
        if isinstance(probe, int):
            return bool(self.m_Value & probe)
        if isinstance(probe, CKFlagEnum):
            return bool(self.m_Value & probe.m_Value)

        return False

    def __repr__(self):
        pending = []

        for i in self:
            # if it have exactly same entry, return directly
            if i.value == self.m_Value:
                return i.name

            # check exist
            if bool(self.m_Value & i.value):
                pending.append(i.name)

        result = ', '.join(pending)
        return ('<' + result + '>') if len(result) != 9 else "<[None] {:d}>".format(self.m_Value)

class CK_FILE_WRITEMODE(CKFlagEnum):
    CKFILE_UNCOMPRESSED	       =0	# Save data uncompressed
    CKFILE_CHUNKCOMPRESSED_OLD =1	# Obsolete
    CKFILE_EXTERNALTEXTURES_OLD=2	# Obsolete : use CKContext::SetGlobalImagesSaveOptions instead.
    CKFILE_FORVIEWER           =4	# Don't save Interface Data within the file, the level won't be editable anymore in the interface
    CKFILE_WHOLECOMPRESSED     =8	# Compress the whole file

class CK_LOAD_FLAGS(CKFlagEnum):
    CK_LOAD_ANIMATION					=1<<0	# Load animations
    CK_LOAD_GEOMETRY					=1<<1	# Load geometry.
    CK_LOAD_DEFAULT						=CK_LOAD_GEOMETRY|CK_LOAD_ANIMATION	# Load animations & geometry
    CK_LOAD_ASCHARACTER					=1<<2 # Load all the objects and create a character that contains them all .
    CK_LOAD_DODIALOG					=1<<3 # Check object name unicity and warns the user with a dialog box when duplicate names are found. 
    CK_LOAD_AS_DYNAMIC_OBJECT			=1<<4 # Objects loaded from this file may be deleted at run-time or are temporary
    CK_LOAD_AUTOMATICMODE				=1<<5 # Check object name unicity and automatically rename or replace according to the options specified in CKContext::SetAutomaticLoadMode
    CK_LOAD_CHECKDUPLICATES				=1<<6 # Check object name unicity (The list of duplicates is stored in the CKFile class after a OpenFile call
    CK_LOAD_CHECKDEPENDENCIES			=1<<7 # Check if every plugins needed are availables
    CK_LOAD_ONLYBEHAVIORS				=1<<8 # 

class CK_FO_OPTIONS(CKEnum):
    CK_FO_DEFAULT = 0
    CK_FO_RENAMEOBJECT = 1
    CK_FO_REPLACEOBJECT = 2
    CK_FO_DONTLOADOBJECT = 3

CK_ID = int

ClassHierarchy = {
    1: ("CKCID_OBJECT", ),
    2: ("CKCID_OBJECT", "CKCID_PARAMETERIN", ),
    4: ("CKCID_OBJECT", "CKCID_PARAMETEROPERATION", ),
    5: ("CKCID_OBJECT", "CKCID_STATE", ),
    6: ("CKCID_OBJECT", "CKCID_BEHAVIORLINK", ),
    8: ("CKCID_OBJECT", "CKCID_BEHAVIOR", ),
    9: ("CKCID_OBJECT", "CKCID_BEHAVIORIO", ),
    12: ("CKCID_OBJECT", "CKCID_RENDERCONTEXT", ),
    13: ("CKCID_OBJECT", "CKCID_KINEMATICCHAIN", ),
    11: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", ),
    15: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_OBJECTANIMATION", ),
    16: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_ANIMATION", ),
    18: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_ANIMATION", "CKCID_KEYEDANIMATION", ),
    19: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", ),
    52: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_DATAARRAY", ),
    10: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SCENE", ),
    21: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_LEVEL", ),
    22: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_PLACE", ),
    23: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_GROUP", ),
    24: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND", ),
    25: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND", "CKCID_WAVESOUND", ),
    26: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_SOUND", "CKCID_MIDISOUND", ),
    30: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MATERIAL", ),
    31: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_TEXTURE", ),
    32: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MESH", ),
    53: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_MESH", "CKCID_PATCHMESH", ),
    47: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", ),
    27: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY", ),
    28: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY", "CKCID_SPRITE", ),
    29: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_RENDEROBJECT", "CKCID_2DENTITY", "CKCID_SPRITETEXT", ),
    33: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", ),
    50: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_GRID", ),
    36: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CURVEPOINT", ),
    37: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_SPRITE3D", ),
    43: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CURVE", ),
    34: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CAMERA", ),
    35: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CAMERA", "CKCID_TARGETCAMERA", ),
    38: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_LIGHT", ),
    39: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_LIGHT", "CKCID_TARGETLIGHT", ),
    40: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_CHARACTER", ),
    41: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_3DOBJECT", ),
    42: ("CKCID_OBJECT", "CKCID_SCENEOBJECT", "CKCID_BEOBJECT", "CKCID_3DENTITY", "CKCID_3DOBJECT", "CKCID_BODYPART", ),
    46: ("CKCID_OBJECT", "CKCID_PARAMETER", ),
    45: ("CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETERLOCAL", ),
    55: ("CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETERLOCAL", "CKCID_PARAMETERVARIABLE", ),
    3: ("CKCID_OBJECT", "CKCID_PARAMETER", "CKCID_PARAMETEROUT", ),
    48: ("CKCID_OBJECT", "CKCID_INTERFACEOBJECTMANAGER", ),
    49: ("CKCID_OBJECT", "CKCID_CRITICALSECTION", ),
    51: ("CKCID_OBJECT", "CKCID_LAYER", ),
    54: ("CKCID_OBJECT", "CKCID_PROGRESSIVEMESH", ),
    20: ("CKCID_OBJECT", "CKCID_SYNCHRO", ),
    80: ("CKCID_OBJECTARRAY", ),
    81: ("CKCID_SCENEOBJECTDESC", ),
    82: ("CKCID_ATTRIBUTEMANAGER", ),
    83: ("CKCID_MESSAGEMANAGER", ),
    84: ("CKCID_COLLISIONMANAGER", ),
    85: ("CKCID_OBJECTMANAGER", ),
    86: ("CKCID_FLOORMANAGER", ),
    87: ("CKCID_RENDERMANAGER", ),
    88: ("CKCID_BEHAVIORMANAGER", ),
    89: ("CKCID_INPUTMANAGER", ),
    90: ("CKCID_PARAMETERMANAGER", ),
    91: ("CKCID_GRIDMANAGER", ),
    92: ("CKCID_SOUNDMANAGER", ),
    93: ("CKCID_TIMEMANAGER", ),
    -1: ("CKCID_CUIKBEHDATA", ),
    56: ("CKCID_MAXCLASSID", ),
    128: ("CKCID_MAXMAXCLASSID", ),
}
class CK_CLASSID(CKEnum):
    CKCID_OBJECT = 1
    CKCID_PARAMETERIN = 2
    CKCID_PARAMETEROPERATION = 4
    CKCID_STATE = 5
    CKCID_BEHAVIORLINK = 6
    CKCID_BEHAVIOR = 8
    CKCID_BEHAVIORIO = 9
    CKCID_RENDERCONTEXT = 12
    CKCID_KINEMATICCHAIN = 13
    CKCID_SCENEOBJECT = 11
    CKCID_OBJECTANIMATION = 15
    CKCID_ANIMATION = 16
    CKCID_KEYEDANIMATION = 18
    CKCID_BEOBJECT = 19
    CKCID_DATAARRAY = 52
    CKCID_SCENE = 10
    CKCID_LEVEL = 21
    CKCID_PLACE = 22
    CKCID_GROUP = 23
    CKCID_SOUND = 24
    CKCID_WAVESOUND = 25
    CKCID_MIDISOUND = 26
    CKCID_MATERIAL = 30
    CKCID_TEXTURE = 31
    CKCID_MESH = 32
    CKCID_PATCHMESH = 53
    CKCID_RENDEROBJECT = 47
    CKCID_2DENTITY = 27
    CKCID_SPRITE = 28
    CKCID_SPRITETEXT = 29
    CKCID_3DENTITY = 33
    CKCID_GRID = 50
    CKCID_CURVEPOINT = 36
    CKCID_SPRITE3D = 37
    CKCID_CURVE = 43
    CKCID_CAMERA = 34
    CKCID_TARGETCAMERA = 35
    CKCID_LIGHT = 38
    CKCID_TARGETLIGHT = 39
    CKCID_CHARACTER = 40
    CKCID_3DOBJECT = 41
    CKCID_BODYPART = 42
    CKCID_PARAMETER = 46
    CKCID_PARAMETERLOCAL = 45
    CKCID_PARAMETERVARIABLE = 55
    CKCID_PARAMETEROUT = 3
    CKCID_INTERFACEOBJECTMANAGER = 48
    CKCID_CRITICALSECTION = 49
    CKCID_LAYER = 51
    CKCID_PROGRESSIVEMESH = 54
    CKCID_SYNCHRO = 20

    CKCID_OBJECTARRAY = 80
    CKCID_SCENEOBJECTDESC = 81
    CKCID_ATTRIBUTEMANAGER = 82
    CKCID_MESSAGEMANAGER = 83
    CKCID_COLLISIONMANAGER = 84
    CKCID_OBJECTMANAGER = 85
    CKCID_FLOORMANAGER = 86
    CKCID_RENDERMANAGER = 87
    CKCID_BEHAVIORMANAGER = 88
    CKCID_INPUTMANAGER = 89
    CKCID_PARAMETERMANAGER = 90
    CKCID_GRIDMANAGER = 91
    CKCID_SOUNDMANAGER = 92
    CKCID_TIMEMANAGER = 93
    CKCID_CUIKBEHDATA = -1

    CKCID_MAXCLASSID = 56
    CKCID_MAXMAXCLASSID = 128

    def __repr__(self):
        hierarchy = ClassHierarchy.get(self.m_Value, None)
        if hierarchy is None:
            return "[Undefined] {:d}".format(self.m_Value)
        return "{} ({})".format(
            hierarchy[-1],
            ' -> '.join(hierarchy)
        )
    def __str__(self):
        hierarchy = ClassHierarchy.get(self.m_Value, None)
        if hierarchy is None:
            return "[Undefined] {:d}".format(self.m_Value)
        return hierarchy[-1]


ErrorDescription = {
    0: "Operation successful",
    -1: "One of the parameter passed to the function was invalid",
    -2: "One of the parameter passed to the function was invalid",
    -3: "The parameter size was invalid",
    -4: "The operation type didn't exist",
    -5: "The function used to execute the operation is not yet implemented",
    -6: "There was not enough memory to perform the action",
    -7: "The function  is not yet implemented",
    -11: "There was an attempt to remove something not present",
    -13: "There is no level currently created",
    -14: "There is no level currently created",
    -16: "The notification message was not used",
    -17: "Attempt to add an item that was already present",
    -18: "the render context is not valid",
    -19: "the render context is not activated for rendering",
    -20: "there was no plugins to load this kind of file",
    -21: "there was no plugins to save this kind of file",
    -22: "attempt to load an invalid file",
    -23: "attempt to load with an invalid plugin",
    -24: "attempt use an object that wasnt initialized",
    -25: "attempt use a message type that wasn't registred",
    -29: "No dll file found in the parse directory",
    -30: "this dll has already been registred",
    -31: "this dll does not contain information to create the prototype",
    -34: "Invalid Object (attempt to Get an object from an invalid ID)",
    -35: "Invalid window was provided as console window",
    -36: "Invalid kinematic chain ( end and start effector may not be part of the same hierarchy )",
    -37: "Keyboard not attached or not working properly",
    -38: "Mouse not attached or not working properly",
    -39: "Joystick not attached or not working properly",
    -40: "Try to link imcompatible Parameters",
    -44: "There is no render engine dll",
    -45: "There is no current level (use CKSetCurrentLevel )",
    -46: "Sound Management has been disabled",
    -47: "DirectInput Management has been disabled",
    -48: "Guid is already in use or invalid",
    -49: "There was no more free space on disk when trying to save a file",
    -50: "Impossible to write to file (write-protection ?)",
    -51: "The behavior cannnot be added to this entity",
    -52: "The behavior cannnot be added to this entity",
    -53: "A manager was registered more than once",
    -54: "CKprocess or TimeManager process while CK is paused will fail",
    -55: "Some plugins were missing whileloading a file",
    -56: "Virtools version too old to load this file",
    -57: "CRC Error while loading file",
    -58: "A Render context is already in Fullscreen Mode",
    -59: "Operation was cancelled by user",
    -121: "there were no animation key at the given index",
    -122: "attemp to acces an animation key with an invalid index",
    -123: "the animation is invalid (no entity associated or zero length)",
}
class CKERROR(CKEnum):
    CKERR_OK =  0
    CKERR_INVALIDPARAMETER = -1
    CKERR_INVALIDPARAMETERTYPE = -2
    CKERR_INVALIDSIZE = -3
    CKERR_INVALIDOPERATION = -4
    CKERR_OPERATIONNOTIMPLEMENTED = -5
    CKERR_OUTOFMEMORY = -6
    CKERR_NOTIMPLEMENTED = -7
    CKERR_NOTFOUND = -11
    CKERR_NOLEVEL = -13
    CKERR_CANCREATERENDERCONTEXT = -14
    CKERR_NOTIFICATIONNOTHANDLED = -16
    CKERR_ALREADYPRESENT = -17
    CKERR_INVALIDRENDERCONTEXT = -18
    CKERR_RENDERCONTEXTINACTIVE = -19
    CKERR_NOLOADPLUGINS = -20
    CKERR_NOSAVEPLUGINS = -21
    CKERR_INVALIDFILE = -22
    CKERR_INVALIDPLUGIN = -23
    CKERR_NOTINITIALIZED = -24
    CKERR_INVALIDMESSAGE = -25
    CKERR_NODLLFOUND = -29
    CKERR_ALREADYREGISTREDDLL = -30
    CKERR_INVALIDDLL = -31
    CKERR_INVALIDOBJECT = -34
    CKERR_INVALIDCONDSOLEWINDOW = -35
    CKERR_INVALIDKINEMATICCHAIN = -36
    CKERR_NOKEYBOARD = -37
    CKERR_NOMOUSE = -38
    CKERR_NOJOYSTICK = -39
    CKERR_INCOMPATIBLEPARAMETERS = -40
    CKERR_NORENDERENGINE = -44
    CKERR_NOCURRENTLEVEL = -45
    CKERR_SOUNDDISABLED = -46
    CKERR_DINPUTDISABLED = -47
    CKERR_INVALIDGUID = -48
    CKERR_NOTENOUGHDISKPLACE = -49
    CKERR_CANTWRITETOFILE = -50
    CKERR_BEHAVIORADDDENIEDBYCB = -51
    CKERR_INCOMPATIBLECLASSID = -52
    CKERR_MANAGERALREADYEXISTS = -53
    CKERR_PAUSED = -54
    CKERR_PLUGINSMISSING = -55
    CKERR_OBSOLETEVIRTOOLS = -56
    CKERR_FILECRCERROR = -57
    CKERR_ALREADYFULLSCREEN = -58
    CKERR_CANCELLED = -59
    CKERR_NOANIMATIONKEY = -121
    CKERR_INVALIDINDEX = -122
    CKERR_INVALIDANIMATION = -123

    def __repr__(self):
        return ErrorDescription.get(self.m_Value, "[Undefined] {:d}".format(self.m_Value))

