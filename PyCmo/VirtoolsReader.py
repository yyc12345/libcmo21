import VirtoolsStruct
import VirtoolsUtils
import PyCmoMisc
import struct, io, datetime, zlib

g_dword = struct.Struct("<I")

def ReadCKFileHeader(fs: io.BufferedReader):
    header = VirtoolsStruct.CKFileHeader()

    # check magic words
    magic_words = fs.read(8)
    if magic_words != b'Nemo Fi\0':
        raise Exception("Fail to read file header magic words")
    header.Signature = magic_words

    # assign data
    (header.Crc, raw_date, header.FileVersion, header.FileVersion2, header.SaveFlags, header.PrewHdrPackSize,
    header.DataPackSize, header.DataUnpackSize, header.ManagerCount, header.ObjectCount, header.MaxIDSaved, 
    header.ProductVersion, header.ProductBuild, header.PrewHdrUnpackSize) = struct.unpack("<14I", fs.read(14 * 4))

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


def ReadCKComposition(fs: io.BufferedReader):
    composition = VirtoolsStruct.CKComposition()

    composition.Header = ReadCKFileHeader(fs)
    return composition
