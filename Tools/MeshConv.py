import argparse, io, math, struct

# setup parser
parser = argparse.ArgumentParser(description='The Mesh Converter.')
parser.add_argument('-p', '--in-vpos', required=True, type=str, action='store', dest='in_vpos', metavar='vpos.bin')
parser.add_argument('-n', '--in-vbml', required=True, type=str, action='store', dest='in_vnml', metavar='vnml.bin')
parser.add_argument('-u', '--in-vuv', required=True, type=str, action='store', dest='in_vuv', metavar='vuv.bin')
parser.add_argument('-i', '--in-findices', required=True, type=str, action='store', dest='in_findices', metavar='findices.bin')
parser.add_argument('-o', '--out-obj', required=True, type=str, action='store', dest='out_obj', metavar='mesh.obj')
parser.add_argument('-m', '--out-mtl', required=True, type=str, action='store', dest='out_mtl', metavar='mesh.mtl')

Vector = tuple[float]
Indices = tuple[int]

def GetFileLength(fs: io.BufferedReader) -> int:
    pos = fs.tell()
    fs.seek(0, io.SEEK_END)
    fsize = fs.tell()
    fs.seek(pos, io.SEEK_SET)
    return fsize

def EvaluateCount(filename: str, unit_size: int) -> int:
    with open(filename, 'rb') as fs:
        filesize = GetFileLength(fs)
        count, modrem = divmod(filesize, unit_size)
        if modrem != 0:
            raise Exception("invalid file length")
        return count

def AssertFileSize(fs: io.BufferedReader, expected_size: int):
    if expected_size != GetFileLength(fs):
            raise Exception("invalid file length")

def ReadFloats(filename: str, count: int) -> tuple[float]:
    with open(filename, 'rb') as fs:
        # construct class
        cstruct = struct.Struct(f'<{count}f')
        # assert file size
        AssertFileSize(fs, cstruct.size)
        # read
        return cstruct.unpack(fs.read(cstruct.size))

def ReadShorts(filename: str, count: int) -> tuple[float]:
    with open(filename, 'rb') as fs:
        # construct class
        cstruct = struct.Struct(f'<{count}H')
        # assert file size
        AssertFileSize(fs, cstruct.size)
        # read
        return cstruct.unpack(fs.read(cstruct.size))
    
def RecoupleTuple(fulllist: tuple, couple_count: int) -> tuple[tuple]:
    count, modrem = divmod(len(fulllist), couple_count)
    if modrem != 0:
        raise Exception("invalid tuple length")
    
    return tuple(map(lambda x: tuple(fulllist[x * couple_count:x * couple_count + couple_count]), range(count)))
    
def GenerateObj(filename: str, vpos: tuple[Vector], vnml: tuple[Vector], vuv: tuple[Vector], findices: tuple[Indices]):
    with open(filename, 'w', encoding='utf-8') as fs:
        for v in vpos:
            fs.write('v {0} {1} {2}\n'.format(v[0], v[1], v[2]))
        for v in vnml:
            fs.write('vn {0} {1} {2}\n'.format(v[0], v[1], v[2]))
        for v in vuv:
            fs.write('vt {0} {1}\n'.format(v[0], v[1]))
        for f in findices:
            fs.write('f {0}/{0}/{0} {1}/{1}/{1} {2}/{2}/{2}\n'.format(f[0] + 1, f[1] + 1, f[2] + 1))
        fs.write('g obj\n')

if __name__ == '__main__':
    # parse arg
    args = parser.parse_args()

    #input("Prepare VertexPositions please.")
    vertexcount = EvaluateCount(args.in_vpos, 3 * 4) # 3 float(4 bytes)
    print(f'Vertex Count Evaluated: {vertexcount}')
    vpos = RecoupleTuple(ReadFloats(args.in_vpos, 3 * vertexcount), 3)

    #input("Prepare VertexNormals please.")
    vnml = RecoupleTuple(ReadFloats(args.in_vnml, 3 * vertexcount), 3)

    #input("Prepare VertexUVs please.")
    vuv = RecoupleTuple(ReadFloats(args.in_vuv, 2 * vertexcount), 2)

    #input("Prepare FaceIndices please.")
    facecount = EvaluateCount(args.in_findices, 3 * 2) # 3 WORD(2 bytes)
    print(f'Face Count Evaluated: {facecount}')
    findices = RecoupleTuple(ReadShorts(args.in_findices, 3 * facecount), 3)

    GenerateObj(args.out_obj, vpos, vnml, vuv, findices)
    print('Done')
