import argparse
import io
import struct
import typing
import itertools
from pathlib import Path
from dataclasses import dataclass

# region: Kernel

T = typing.TypeVar('T')
Vector = tuple[float, ...]
Indices = tuple[int, ...]

def get_file_length(fs: typing.BinaryIO) -> int:
    """
    Get the full length of given file in bytes.

    :param fs: File stream for measuring.
    :return: File length in bytes.
    """
    pos = fs.tell()
    fs.seek(0, io.SEEK_END)
    fsize = fs.tell()
    fs.seek(pos, io.SEEK_SET)
    return fsize

def evaluate_count(filename: Path, unit_size: int) -> int:
    """
    Evaluate the count of items in given file.

    :param filename: File name to evaluate.
    :param unit_size: Size of each item in bytes.
    :return: Count of items in given file.
    """
    with open(filename, 'rb') as fs:
        file_size = get_file_length(fs)
        count, modrem = divmod(file_size, unit_size)
        if modrem != 0:
            raise Exception("invalid file length")
        return count

def assert_file_size(fs: typing.BinaryIO, expected_size: int):
    """
    Check whether given file has expected size.

    :param fs: File stream to check.
    :param expected_size: Expected file size.
    """
    if expected_size != get_file_length(fs):
            raise Exception("invalid file length")

def read_f32s(filename: Path, count: int) -> tuple[float, ...]:
    with open(filename, 'rb') as fs:
        # construct class
        cstruct = struct.Struct(f'<{count}f')
        # assert file size
        assert_file_size(fs, cstruct.size)
        # read
        return cstruct.unpack(fs.read(cstruct.size))

def read_u16s(filename: Path, count: int) -> tuple[int, ...]:
    with open(filename, 'rb') as fs:
        # construct class
        cstruct = struct.Struct(f'<{count}H')
        # assert file size
        assert_file_size(fs, cstruct.size)
        # read
        return cstruct.unpack(fs.read(cstruct.size))
    
def batched_tuple(full_list: tuple[T, ...], couple_count: int) -> tuple[tuple[T, ...], ...]:
    """
    Batch a tuple into a tuple of tuples.

    This function will check whether given tuple can be batched without any remnants.
    If it is, throw exception, otherwise return the batched tuple.

    For example, given `('roses', 'red', 'violets', 'blue', 'sugar', 'sweet')`,
    it will produce `(('roses', 'red'), ('violets', 'blue'), ('sugar', 'sweet'))`.

    :param full_list: The tuple to batch.
    :param couple_count: The count of items in each batch.
    :return: The batched tuple.
    """
    # TODO: Replace the whole body with itertools.batched once we upgrade into Python 3.12
    # return itertools.batched(full_list, couple_count, strict=True)
    count, modrem = divmod(len(full_list), couple_count)
    if modrem != 0:
        raise Exception("invalid tuple length")
    
    return tuple(map(lambda x: tuple(full_list[x * couple_count:x * couple_count + couple_count]), range(count)))
    
def build_obj_file(filename: Path, vpos: tuple[Vector, ...], vnml: tuple[Vector, ...], vuv: tuple[Vector, ...], findices: tuple[Indices, ...]):
    with open(filename, 'w', encoding='utf-8') as fs:
        for v in vpos:
            fs.write(f'v {v[0]} {v[1]} {v[2]}\n')
        for v in vnml:
            fs.write(f'vn {v[0]} {v[1]} {v[2]}\n')
        for v in vuv:
            fs.write(f'vt {v[0]} {v[1]}\n')
        for f in findices:
            fs.write(f'f {f[0] + 1}/{f[0] + 1}/{f[0] + 1} {f[1] + 1}/{f[1] + 1}/{f[1] + 1} {f[2] + 1}/{f[2] + 1}/{f[2] + 1}\n')
        fs.write('g obj\n')

# endregion

# region Command Line Processor

@dataclass
class Cli:
    """Command Line Arguments"""

    in_vpos: Path
    """The path to file storing vertex positions"""
    in_vnml: Path
    """The path to file storing vertex normals"""
    in_vuv: Path
    """The path to file storing vertex UVs"""
    in_findices: Path
    """The path to file storing face indices"""
    out_obj: Path
    """The path to output OBJ file"""
    out_mtl: Path
    """The path to output MTL file"""

def parse() -> Cli:
    # construct parser
    parser = argparse.ArgumentParser(description='The mesh data combinator for libcmo21 debugging.')
    parser.add_argument('-p', '--in-vpos', required=True, type=str, action='store', dest='in_vpos', metavar='vpos.bin',
                        help='''The path to file storing vertex positions''')
    parser.add_argument('-n', '--in-vnml', required=True, type=str, action='store', dest='in_vnml', metavar='vnml.bin',
                        help='''The path to file storing vertex normals''')
    parser.add_argument('-u', '--in-vuv', required=True, type=str, action='store', dest='in_vuv', metavar='vuv.bin',
                        help='''The path to file storing vertex UVs''')
    parser.add_argument('-i', '--in-findices', required=True, type=str, action='store', dest='in_findices', metavar='findices.bin',
                        help='''The path to file storing face indices''')
    parser.add_argument('-o', '--out-obj', required=True, type=str, action='store', dest='out_obj', metavar='mesh.obj',
                        help='''The path to output OBJ file''')
    parser.add_argument('-m', '--out-mtl', required=True, type=str, action='store', dest='out_mtl', metavar='mesh.mtl',
                        help='''The path to output MTL file''')

    # parse arg
    args = parser.parse_args()
    # return value
    return Cli(
        Path(args.in_vpos),
        Path(args.in_vnml),
        Path(args.in_vuv),
        Path(args.in_findices),
        Path(args.out_obj),
        Path(args.out_mtl)
    )


# endregion

def main():
    # parse arguments
    opts = parse()
    
    vertex_count = evaluate_count(opts.in_vpos, 3 * 4) # 3 float(4 bytes)
    print(f'Vertex Count Evaluated: {vertex_count}')
    vpos = batched_tuple(read_f32s(opts.in_vpos, 3 * vertex_count), 3)

    vnml = batched_tuple(read_f32s(opts.in_vnml, 3 * vertex_count), 3)

    vuv = batched_tuple(read_f32s(opts.in_vuv, 2 * vertex_count), 2)

    face_count = evaluate_count(opts.in_findices, 3 * 2) # 3 WORD(2 bytes)
    print(f'Face Count Evaluated: {face_count}')
    findices = batched_tuple(read_u16s(opts.in_findices, 3 * face_count), 3)

    build_obj_file(opts.out_obj, vpos, vnml, vuv, findices)
    print('Done')
    

if __name__ == '__main__':
    main()
