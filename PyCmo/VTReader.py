import VTStruct, VTUtils, VTConstants
import PyCmoMisc
import struct, io, datetime, zlib
import typing

g_HeaderPacker = struct.Struct('<' + 'i' * 8)

class CKFileReader():

    @staticmethod
    def ReadFileHeaders(self: VTStruct.CKFile) -> VTConstants.CKERROR:
        if self.m_Parser is None:
            return VTConstants.CKERR.CKERR_INVALIDPARAMETER
        header = VTStruct.CKFileHeader()

        # check magic words
        magic_words = self.m_Parser.GetReader()[0:4]
        if magic_words != b'Nemo':
            return VTConstants.CKERR.CKERR_INVALIDFILE
        
        # read header1
        if self.m_Parser.GetSize() < 0x20:
            return VTConstants.CKERR.CKERR_INVALIDFILE
        header1 = g_HeaderPacker.unpack(self.m_Parser.GetReader().read(8 * 4))

        # check header1
        if header1[5]:  # i don't know what is this fields stands for
            header1 = tuple(0 for _ in range(8))

        # virtools is too old to open this file. file is too new.
        if header1[4] > 9:  # file version
            return VTConstants.CKERR.CKERR_OBSOLETEVIRTOOLS

        # read header2
        # file ver < 5 do not have second header
        if header1[4] < 5:
            header2 = tuple(0 for _ in range(8))
        else:
            if self.m_Parser.GetSize() < 0x40:
                return VTConstants.CKERR.CKERR_INVALIDFILE

            header2 = g_HeaderPacker.unpack(self.m_Parser.GetReader().read(8 * 4))

        # forcely reset too big product ver
        if header2[5] >= 12:    # product version
            header2[5] = 0
            header2[6] = 0x1010000   # product build

        # assign value
        self.m_FileInfo.ProductVersion = header2[5]
        self.m_FileInfo.ProductBuild = header2[6]
        self.m_FileInfo.FileWriteMode.m_Value = header1[6]
        self.m_FileInfo.CKVersion = header1[3]
        self.m_FileInfo.FileVersion = header1[4]
        self.m_FileInfo.FileSize = self.m_Parser.GetSize()
        self.m_FileInfo.ManagerCount = header2[2]
        self.m_FileInfo.ObjectCount = header2[3]
        self.m_FileInfo.MaxIDSaved = header2[4]
        self.m_FileInfo.Hdr1PackSize = header1[7]
        self.m_FileInfo.Hdr1UnPackSize = header2[7]
        self.m_FileInfo.DataPackSize = header2[0]
        self.m_FileInfo.DataUnPackSize = header2[1]
        self.m_FileInfo.Crc = header1[2]


        # process date independently
        # date is in BCD code
        day = PyCmoMisc.BcdCodeToDecCode((raw_date >> 24) & 0xff)
        month = PyCmoMisc.BcdCodeToDecCode((raw_date >> 16) & 0xff - 1)
        month = (month % 12) + 1
        year = PyCmoMisc.BcdCodeToDecCode(raw_date & 0xffff)
        header.Timestamp = datetime.date(year, month, day)

        if header.FileVersion >= 8:
            # check crc
            gotten_crc = zlib.adler32(b'Nemo Fi\0', 0)
            gotten_crc = zlib.adler32(struct.pack("<6I", 0, raw_date, header.FileVersion, header.FileVersion2, header.SaveFlags, header.PrewHdrPackSize), gotten_crc)   # reset crc as zero
            gotten_crc = zlib.adler32(struct.pack("<8I", header.DataPackSize, header.DataUnpackSize, header.ManagerCount, header.ObjectCount, header.MaxIDSaved, header.ProductVersion, header.ProductBuild, header.PrewHdrUnpackSize), gotten_crc)
            gotten_crc = zlib.adler32(fs.read(header.PrewHdrPackSize), gotten_crc)
            gotten_crc = zlib.adler32(fs.read(header.DataPackSize), gotten_crc)
            if gotten_crc != header.Crc:
                raise Exception("Crc Error")

        return header

    
    @staticmethod
    def ReadCKComposition(fs: io.BufferedReader):
        composition = VTStruct.CKComposition()

        composition.Header = ReadCKFileHeader(fs)
        return composition
