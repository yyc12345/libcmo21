import os, re

self_path: str = os.path.dirname(__file__)
src_file: str = os.path.join(self_path, '../../BMap/BMExports.hpp')
dst_file: str = os.path.join(self_path, 'dest/BMExports.hpp')

with open(src_file, 'r', encoding='utf-8') as fsrc:
    # read full text
    fulltext: str = fsrc.read()
    # do findall and write into file
    with open(dst_file, 'w', encoding='utf-8') as fdst:
        # We should not only match BMAP_EXPORT,
        # because it may match the defination of BMAP_EXPORT.
        # So we add a bool at head because all BMap functions return bool.
        for item in re.findall('^BMAP_EXPORT[ \\t]+bool[ \\t]+[^;]+;', fulltext, re.MULTILINE):
            fdst.write(item)
            fdst.write('\n')

print('DONE')
