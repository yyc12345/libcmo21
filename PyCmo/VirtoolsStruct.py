import VirtoolsConstants
import datetime
import PyCmoMisc

class CKFileHeader:
    def __init__(self):
        self.Signature: bytes = b'Nemo Fi\0'
        self.Crc: int = 0
        self.Timestamp: datetime.date = datetime.date.today()
        self.FileVersion: int = 0
        self.FileVersion2: int = 0
        self.SaveFlags: int = 0
        self.PrewHdrPackSize: int = 0

        self.DataPackSize: int = 0
        self.DataUnpackSize: int = 0
        self.ManagerCount: int = 0
        self.ObjectCount: int = 0
        self.MaxIDSaved: int = 0
        self.ProductVersion: int = 0
        self.ProductBuild: int = 0
        self.PrewHdrUnpackSize: int = 0

    def __str__(self):
        return f"""File Version: {self.FileVersion:d} / {self.FileVersion2:d}
Production (Version / Build): {self.ProductVersion:d} / {(self.ProductBuild >> 24) & 0xff:d}.{(self.ProductBuild >> 16) & 0xff:d}.{(self.ProductBuild >> 8) & 0xff:d}.{self.ProductBuild & 0xff:d}
Crc: 0x{self.Crc:08X}
Timestamp: {str(self.Timestamp)}
Save Flags: {VirtoolsConstants.PyEnum.PrintEnumFlag(self.SaveFlags, VirtoolsConstants.CK_FILE_WRITEMODE)}

Preview Header (Pack / Unpack): {PyCmoMisc.OutputSizeHumanReadable(self.PrewHdrPackSize)} / {PyCmoMisc.OutputSizeHumanReadable(self.PrewHdrUnpackSize)}
Data (Pack / Unpack): {PyCmoMisc.OutputSizeHumanReadable(self.DataPackSize)} / {PyCmoMisc.OutputSizeHumanReadable(self.DataUnpackSize)}

Manager Count: {self.ManagerCount:d}
Object Count: {self.ObjectCount:d}
Max ID Saved: {self.MaxIDSaved:d}
"""


class CKComposition(object):
    def __init__(self):
        self.Header: CKFileHeader = None

