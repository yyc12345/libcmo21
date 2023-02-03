import VTStruct, VTUtils, VTConstants
import PyCmoMisc
import struct, io, datetime, zlib
import typing

g_Header1Packer = struct.Struct('<iiIiiiii')
g_Header2Packer = struct.Struct('<iiiiiiii')
g_FileObjectPacker = struct.Struct('<iiii')

class CKFileReader():

    @staticmethod
    def ReadFileHeaders(self: VTStruct.CKFile) -> VTConstants.CKERROR:
        if self.m_Parser is None:
            return VTConstants.CKERROR(VTConstants.CKERROR.CKERR_INVALIDPARAMETER)
        parser = self.m_Parser

        # ========== magic words ==========
        magic_words = parser.GetReader()[0:4]
        if magic_words != b'Nemo':
            return VTConstants.CKERROR(VTConstants.CKERROR.CKERR_INVALIDFILE)
        
        # ========== read header1 ==========
        if parser.GetSize() < 0x20:
            return VTConstants.CKERROR(VTConstants.CKERROR.CKERR_INVALIDFILE)
        header1 = list(g_Header1Packer.unpack(parser.GetReader().read(8 * 4)))

        # check header1
        if header1[5]:  # i don't know what is this fields stands for
            header1 = list(0 for _ in range(8))

        # virtools is too old to open this file. file is too new.
        if header1[4] > 9:  # file version
            return VTConstants.CKERROR(VTConstants.CKERROR.CKERR_OBSOLETEVIRTOOLS)

        # ========== read header2 ==========
        # file ver < 5 do not have second header
        if header1[4] < 5:
            header2 = list(0 for _ in range(8))
        else:
            if parser.GetSize() < 0x40:
                return VTConstants.CKERROR(VTConstants.CKERROR.CKERR_INVALIDFILE)

            header2 = g_Header2Packer.unpack(parser.GetReader().read(8 * 4))

        # forcely reset too big product ver
        if header2[5] >= 12:    # product version
            header2[5] = 0
            header2[6] = 0x1010000   # product build

        # ========== assign value ==========
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

        # ========== crc and body unpacker ==========
        # file version greater than 8 have crc feature and compression feature
        if self.m_FileInfo.FileVersion >= 8:
            # reset crc field of header
            header1[2] = 0

            # compute crc
            gotten_crc = zlib.adler32(g_Header1Packer.pack(*header1), 0)

            reader = parser.GetReader()
            pos_cache = reader.tell()
            reader.seek(8 * 4, io.SEEK_SET)
            gotten_crc = zlib.adler32(reader.read(
                8 * 4 + self.m_FileInfo.Hdr1PackSize + self.m_FileInfo.DataPackSize
            ), gotten_crc)
            reader.seek(pos_cache, io.SEEK_SET)

            # compare crc
            if gotten_crc != self.m_FileInfo.Crc:
                return VTConstants.CKERROR(VTConstants.CKERROR.CKERR_FILECRCERROR)


            # compare size to decide wheher use compress feature
            if self.m_FileInfo.Hdr1UnPackSize != self.m_FileInfo.Hdr1PackSize:
                # create a new parser for following analyze
                parser = VTUtils.SmallZlibFileReader(parser, self.m_FileInfo.Hdr1PackSize, self.m_FileInfo.Hdr1UnPackSize)

        # ========== object list read ==========
        # file ver >= 7 have this features
        if self.m_FileInfo.FileVersion >= 7:
            self.m_SaveIDMax = self.m_FileInfo.MaxIDSaved
            # resize
            self.m_FileObjects = list(VTStruct.CKFileObject() for _ in range(self.m_FileInfo.ObjectCount))

            # read data
            reader = parser.GetReader()
            for fileobj in self.m_FileObjects:
                # set useless fields
                fileobj.ObjPtr = None
                fileobj.Name = None
                fileobj.Data = None

                # read basic fields
                (fileobj.Object, cid, fileobj.FileIndex, namelen) = g_FileObjectPacker.unpack(
                    reader.read(g_FileObjectPacker.size)
                )
                fileobj.ObjectCid.Set(cid)

                # read name
                if namelen > 0:
                    fileobj.Name = reader.read(namelen).decode('utf-8', errors = 'ignore')
                    
        # ========== dep list read ==========
        # only file ver >= 8, this sector is existed
        if self.m_FileInfo.FileVersion >= 8:
            pass


    @staticmethod
    def Load(self: VTStruct.CKFile, filename: str, load_flags: VTConstants.CK_LOAD_FLAGS) -> VTConstants.CKERROR:
        # todo: clear first

        # setup basic fields
        self.m_FileName = filename
        self.m_Flags = load_flags
        
        # setup parser
        self.m_Parser = VTUtils.RawFileReader(filename)

        # read header first
        err = CKFileReader.ReadFileHeaders(self)

        # todo: finish other read

        return err
