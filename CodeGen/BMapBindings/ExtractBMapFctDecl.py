import os, re

self_path: str = os.path.dirname(__file__)
src_file: str = os.path.join(self_path, '../../BMap/BMExports.hpp')
dst_file: str = os.path.join(self_path, 'dest/BMExports.hpp')

with open(src_file, 'r', encoding='utf-8') as fsrc:
    # read full text
    fulltext: str = fsrc.read()
    # do findall and write into file
    with open(dst_file, 'w', encoding='utf-8') as fdst:
        for item in re.findall('^LIBCMO_EXPORT[^;]+;', fulltext, re.MULTILINE):
            fdst.write(item)
            fdst.write('\n')

print('DONE')
