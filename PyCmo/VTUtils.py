import PyCmoMisc
import zlib, io, mmap, os
import typing

class RawFileReader():
    def __init__(self, filename: str):
        self.__size: int = os.path.getsize(filename)
        self.__fs = open(filename, 'rb')
        self.__mm: mmap.mmap = mmap.mmap(self.__fs.fileno, 0, access = mmap.ACCESS_READ)
    
    def __del__(self):
        self.__mm.close()
        del self.__mm
        self.__fs.close()
        del self.__fs

    def GetSize(self) -> int:
        return self.__size

    def GetReader(self) -> mmap.mmap:
        return self.__mm

class LargeZlibFileReader():
    def __init__(self, raw_reader: RawFileReader, comp_size: int, uncomp_size: int):
        # set size
        self.__size: int = uncomp_size

        # create mmap
        self.__mm: mmap.mmap = mmap.mmap(-1, -1, access = mmap.ACCESS_WRITE)
        
        # decompress data
        reader = raw_reader.GetReader()
        parser: zlib._Decompress = zlib.decompressobj()

        buf = reader.read(io.DEFAULT_BUFFER_SIZE)
        while buf:
            self.__mm.write(parser.decompress(buf))
            buf = reader.read(io.DEFAULT_BUFFER_SIZE)
        self._mm.write(parser.flush())

    def __del__(self):
        self.__mm.close()
        del self.__mm

    def GetSize(self) -> int:
        return self.__size

    def GetReader(self) -> mmap.mmap:
        return self.__mm

class SmallZlibFileReader():
    def __init__(self):
        # create io
        self.__ss: io.BytesIO = io.BytesIO()

        # decompress data
        reader = raw_reader.GetReader()
        parser: zlib._Decompress = zlib.decompressobj()

        buf = reader.read(io.DEFAULT_BUFFER_SIZE)
        while buf:
            self.__ss.write(parser.decompress(buf))
            buf = reader.read(io.DEFAULT_BUFFER_SIZE)
        self._ss.write(parser.flush())

    def __del__(self):
        del self.__ss

    def GetSize(self) -> int:
        return len(self.__ss.getvalue())

    def GetReader(self) -> io.BytesIO:
        return self.__ss

UniversalFileReader = typing.Union[RawFileReader, LargeZlibFileReader, SmallZlibFileReader]
