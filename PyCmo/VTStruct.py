import VTConstants, VTUtils
import PyCmoMisc
import datetime

class CKFileInfo:
    def __init__(self: CKFileInfo):
        self.ProductVersion: int = 0
        self.ProductBuild: int = 0

        self.FileWriteMode: VTConstants.CK_FILE_WRITEMODE = 0
        
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
        self.DataUnpackSize: int = 0

    def GetProductBuildTuple(self: CKFileInfo) -> tuple[int]:
        return (
            (self.ProductBuild >> 24) & 0xff, 
            (self.ProductBuild >> 16) & 0xff, 
            (self.ProductBuild >> 8) & 0xff, 
            self.ProductBuild & 0xff
        )

    def __repr__(self: CKFileInfo) -> str:
        return f"""Version (File / CK): {self.FileVersion:08X} / {self.CKVersion:08X}
Product (Version / Build): {self.ProductVersion:d} / {'.0'.join(self.GetProductBuildTuple())}
Save Flags: {self.SaveFlags}
File Size: {PyCmoMisc.OutputSizeHumanReadable(self.FileSize)}
Crc: 0x{self.Crc:08X}

Preview Header (Pack / Unpack): {PyCmoMisc.OutputSizeHumanReadable(self.PrewHdrPackSize)} / {PyCmoMisc.OutputSizeHumanReadable(self.PrewHdrUnpackSize)}
Data (Pack / Unpack): {PyCmoMisc.OutputSizeHumanReadable(self.DataPackSize)} / {PyCmoMisc.OutputSizeHumanReadable(self.DataUnpackSize)}

Manager Count: {self.ManagerCount:d}
Object Count: {self.ObjectCount:d}
Max ID Saved: {self.MaxIDSaved:d}
"""



class CKFile(object):
    def __init__(self):
        self.m_FileName: str = ''
        self.m_FileInfo: CKFileInfo = CKFileInfo()
        self.m_Parser: VTUtils.UniversalFileReader = None

    def __repr__(self: CKFile) -> str:
        return self.m_FileInfo

