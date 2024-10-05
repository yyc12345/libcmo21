import os
import PyBMap.bmap_wrapper as bmap

def main() -> None:
    input(f'Python PID is {os.getpid()}. Waiting for debugger, press any key to continue...')

    file_name: str = 'Level_02.NMO'
    temp_folder: str = 'Temp'
    texture_folder: str = 'F:\\Ballance\\Ballance\\Textures'
    encodings: tuple[str, ...] = ('cp1252', )
    with bmap.BMFileReader(file_name, temp_folder, texture_folder, encodings) as reader:
        print('===== Groups =====')
        for gp in reader.get_groups():
            print(gp.get_name())

        print('===== 3dObjects =====')
        for obj in reader.get_3dobjects():
            print(obj.get_name())
        
        print('===== Meshes =====')
        for mesh in reader.get_meshs():
            print(mesh.get_name())

        print('===== Materials =====')
        for mtl in reader.get_materials():
            print(mtl.get_name())

        print('===== Textures =====')
        for tex in reader.get_textures():
            print(tex.get_name())

    print('===== END =====')

if __name__ == '__main__':
    main()
