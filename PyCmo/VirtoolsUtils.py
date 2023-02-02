import zlib
import io

class ZlibDecompressBuffer(object):
    def __init__(self, _fs: io.BufferedReader, _len: int, _is_compressed: bool):
        self.__fs: io.BufferedReader = _fs
        self.__len: int = _len
        self.__compressed: bool = _is_compressed

        self.__pos: int = 0
        self.__parser: zlib._Decompress = zlib.decompressobj()
        self.__cache: bytes = b''
        self.__cachelen: int = 0

    def __ParseOnce(self) -> bytes:
        # check remain
        remain: int = self.__len - self.__pos
        if remain <= 0:
            return None

        # read it and increase pos
        read_count: int = min(remain, 1024)
        gotten_uncompressed: bytes = self.__parser.decompress(self.__fs.read(read_count))
        self.__pos += read_count

        # everything has done, no more data, flush it and get it remained data
        if self.__pos >= self.__len:
            gotten_uncompressed += self.__parser.flush()

        return gotten_uncompressed

    def Read(self, expected: int):
        # try enrich cache
        while self.__cachelen < expected:
            new_data = self.__ParseOnce()
            if new_data is None:
                # no more data
                raise Exception("No more data.")
            else:
                self.__cache += new_data
                self.__cachelen += len(new_data)

        # change data
        returned_data = self.__cache[:expected]
        self.__cache = self.__cache[expected:]
        self.__cachelen -= expected
        
        return returned_data