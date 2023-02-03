import VTConstants, VTUtils
import PyCmoMisc
import datetime

class CKGUID():
    def __init__(self):
        self.D1: int = 0
        self.D2: int = 0
    def __init__(self, d1: int, d2: int):
        self.D1: int = d1
        self.D2: int = d2
    def __repr__(self):
        return f"<CKGUID: 0x{self.D1:08X}, 0x{self.D2:08X}"
    def __str__(self):
        return self.__repr__()

class CKFileInfo:
    def __init__(self):
        self.ProductVersion: int = 0
        self.ProductBuild: int = 0

        self.FileWriteMode: VTConstants.CK_FILE_WRITEMODE = VTConstants.CK_FILE_WRITEMODE(0)
        
        self.FileVersion: int = 0
        self.CKVersion: int = 0

        self.FileSize: int = 0
        self.ObjectCount: int = 0
        self.ManagerCount: int = 0
        self.MaxIDSaved: int = 0

        self.Crc: int = 0
        
        self.Hdr1PackSize: int = 0
        self.Hdr1UnPackSize: int = 0
        self.DataPackSize: int = 0
        self.DataUnPackSize: int = 0

    def GetProductBuildTuple(self) -> tuple[int]:
        return (
            (self.ProductBuild >> 24) & 0xff, 
            (self.ProductBuild >> 16) & 0xff, 
            (self.ProductBuild >> 8) & 0xff, 
            self.ProductBuild & 0xff
        )

    def __repr__(self) -> str:
        return f"""<CKFile: 
    Version (File / CK): {self.FileVersion:d} / 0x{self.CKVersion:08X}
    Product (Version / Build): {self.ProductVersion:d} / {'.0'.join(map(lambda x: str(x), self.GetProductBuildTuple()))}
    Save Flags: {self.FileWriteMode}
    File Size: {PyCmoMisc.OutputSizeHumanReadable(self.FileSize)}
    Crc: 0x{self.Crc:08X}

    Hdr1 (Pack / Unpack): {PyCmoMisc.OutputSizeHumanReadable(self.Hdr1PackSize)} / {PyCmoMisc.OutputSizeHumanReadable(self.Hdr1UnPackSize)}
    Data (Pack / Unpack): {PyCmoMisc.OutputSizeHumanReadable(self.DataPackSize)} / {PyCmoMisc.OutputSizeHumanReadable(self.DataUnPackSize)}

    Manager Count: {self.ManagerCount:d}
    Object Count: {self.ObjectCount:d}
    Max ID Saved: {self.MaxIDSaved:d}
>"""
    def __str__(self) -> str:
        return self.__repr__()

class CKFileObject():
    def __init__(self):
        self.Object: VTConstants.CK_ID = 0
        self.CreatedObject: VTConstants.CK_ID = 0
        self.ObjectCid: VTConstants.CK_CLASSID = VTConstants.CK_CLASSID(VTConstants.CK_CLASSID.CKCID_OBJECT)
        self.ObjPtr = None
        self.Name: str = None
        self.Data = None
        self.PostPackSize: int = 0
        self.PrePackSize: int = 0
        self.Options: VTConstants.CK_FO_OPTIONS = VTConstants.CK_FO_OPTIONS(VTConstants.CK_FO_OPTIONS.CK_FO_DEFAULT)
        self.FileIndex: int = 0
        self.SaveFlags: VTConstants.CK_FILE_WRITEMODE = VTConstants.CK_FILE_WRITEMODE(0)

    def __repr__(self) -> str:
        return f'<{self.Name if self.Name else "[Anonymous]"}: {self.ObjectCid}>'
    def __str__(self) -> str:
        return self.__repr__()

class CKFilePluginDependencies():
    def __init__(self):
        self.m_PluginCategory: int = 0
        self.m_Guids: list[CKGUID] = []
        self.ValidGuids: list[bool] = []
        
    def __repr__(self) -> str:
        return repr(self.m_Guids)
    def __str__(self) -> str:
        return self.__repr__()

class CKFile(object):
    def __init__(self):
        self.m_SaveIDMax: int = 0
        self.m_FileObjects: list[CKFileObject] = []
        self.m_ManagersData = []
        self.m_PluginDep: list[CKFilePluginDependencies] = []
        self.m_IndexByClassId: list[list[int]] = []
        self.m_IncludedFiles: list[str] = []

        self.m_FileInfo: CKFileInfo = CKFileInfo()

        self.m_Flags: VTConstants.CK_LOAD_FLAGS = VTConstants.CK_LOAD_FLAGS(VTConstants.CK_LOAD_FLAGS.CK_LOAD_DEFAULT)
        self.m_FileName: str = ''
        self.m_Parser: VTUtils.UniversalFileReader = None

        self.m_ReadFileDataDone: bool = False

    def __repr__(self) -> str:
        return repr(self.m_FileInfo)
    def __str__(self) -> str:
        return self.__repr__()

