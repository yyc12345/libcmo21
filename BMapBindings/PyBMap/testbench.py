import os
import PyBMap.bmap_wrapper as bmap

def main() -> None:
    input(f'Python PID is {os.getpid()}. Waiting for debugger, press any key to continue...')

    file_name: str = 'LightCameraTest.nmo'
    temp_folder: str = 'Temp'
    texture_folder: str = 'F:\\Ballance\\Ballance\\Textures'
    encodings: tuple[str, ...] = ('cp1252', )
    with bmap.BMFileReader(file_name, temp_folder, texture_folder, encodings) as reader:
        test_common(reader)
        test_equatable(reader)

def test_common(reader: bmap.BMFileReader):
    # print('===== Groups =====')
    # for gp in reader.get_groups():
    #     print(gp.get_name())
    #     for gp_item in gp.get_objects():
    #         print(f'\t{gp_item.get_name()}')

    # print('===== 3dObjects =====')
    # for obj in reader.get_3dobjects():
    #     print(obj.get_name())

    #     current_mesh = obj.get_current_mesh()
    #     mesh_name = '<null>' if current_mesh is None else current_mesh.get_name()
    #     print(f'\tMesh: {mesh_name}')
    #     print(f'\tVisibility: {obj.get_visibility()}')
    #     print(f'\tMatrix: {obj.get_world_matrix().to_const()}')
    
    # print('===== Meshes =====')
    # for mesh in reader.get_meshs():
    #     print(mesh.get_name())

    #     print(f'\tLit Mode: {mesh.get_lit_mode()}')
    #     print(f'\tVertex Count: {mesh.get_vertex_count()}')
    #     print(f'\tFace Count: {mesh.get_face_count()}')
    #     print(f'\tMaterial Slot Count: {mesh.get_material_slot_count()}')

    # print('===== Materials =====')
    # for mtl in reader.get_materials():
    #     print(mtl.get_name())
        
    #     print(f'\tDiffuse: {mtl.get_diffuse().to_const_rgba()}')
    #     print(f'\tAmbient: {mtl.get_ambient().to_const_rgba()}')
    #     print(f'\tSpecular: {mtl.get_specular().to_const_rgba()}')
    #     print(f'\tEmissive: {mtl.get_emissive().to_const_rgba()}')

    #     print(f'\tSpecular Power: {mtl.get_specular_power()}')

    #     print(f'\tTexture Border Color: {mtl.get_texture_border_color().to_const_rgba()}')

    #     print(f'\tTexture Blend Mode: {mtl.get_texture_blend_mode()}')
    #     print(f'\tTexture Min Mode: {mtl.get_texture_min_mode()}')
    #     print(f'\tTexture Mag Mode: {mtl.get_texture_mag_mode()}')
    #     print(f'\tSource Blend: {mtl.get_source_blend()}')
    #     print(f'\tDest Blend: {mtl.get_dest_blend()}')
    #     print(f'\tFill Mode: {mtl.get_fill_mode()}')
    #     print(f'\tShade Mode: {mtl.get_shade_mode()}')

    #     print(f'\tAlpha Test Enabled: {mtl.get_alpha_test_enabled()}')
    #     print(f'\tAlpha Blend Enabled: {mtl.get_alpha_blend_enabled()}')
    #     print(f'\tPerspective Correction Enabled: {mtl.get_perspective_correction_enabled()}')
    #     print(f'\tZ Write Enabled: {mtl.get_z_write_enabled()}')
    #     print(f'\tTwo Sided Enabled: {mtl.get_two_sided_enabled()}')

    #     print(f'\tAlpha Ref: {mtl.get_alpha_ref()}')

    #     print(f'\tAlpha Func: {mtl.get_alpha_func()}')
    #     print(f'\tZ Func: {mtl.get_z_func()}')

    # print('===== Textures =====')
    # for tex in reader.get_textures():
    #     print(tex.get_name())

    #     print(f'\tFile Name: {tex.get_file_name()}')
    #     print(f'\tSave Options: {tex.get_save_options()}')
    #     print(f'\tVideo Format: {tex.get_video_format()}')

    print('===== Target Lights =====')
    for lit in reader.get_target_lights():
        print(lit.get_name())

        print(f'\tVisibility: {lit.get_visibility()}')
        print(f'\tMatrix: {lit.get_world_matrix().to_const()}')
    
        print(f'\tType: {lit.get_type()}')
        print(f'\tColor: {lit.get_color().to_const_rgba()}')
        print(f'\tConstant Attenuation: {lit.get_constant_attenuation()}')
        print(f'\tLinear Attenuation: {lit.get_linear_attenuation()}')
        print(f'\tQuadratic Attenuation: {lit.get_quadratic_attenuation()}')
        print(f'\tRange: {lit.get_range()}')
        print(f'\tHot Spot: {lit.get_hot_spot()}')
        print(f'\tFalloff: {lit.get_falloff()}')
        print(f'\tFalloff Shape: {lit.get_falloff_shape()}')

    print('===== END =====')

def test_equatable(reader: bmap.BMFileReader):
    # Check requirements
    assert (reader.get_3dobject_count() >= 2), '''
    Invalid file for test IEquatable.
    We can not perform IEquatable test because the length of 3dObject is too short (must greater than 2). Please choose another file to perform.
    '''

    # Prepare variables
    all_3dobjects: tuple[bmap.BM3dObject, ...] = tuple(reader.get_3dobjects())
    first_3dobj: bmap.BM3dObject = all_3dobjects[0]
    second_3dobj: bmap.BM3dObject = all_3dobjects[1]
    all_3dobjects = tuple(reader.get_3dobjects())
    first_3dobj_again: bmap.BM3dObject = all_3dobjects[0]

    # Test set
    test_set: set[bmap.BM3dObject] = set()

    test_set.add(first_3dobj)
    assert len(test_set) == 1

    assert first_3dobj in test_set
    assert first_3dobj_again in test_set
    assert second_3dobj not in test_set

    test_set.add(first_3dobj_again)
    assert len(test_set) == 1
    test_set.add(second_3dobj)
    assert len(test_set) == 2

    assert second_3dobj in test_set

    # Test dict
    test_dict: dict[bmap.BM3dObject, str | None] = {}

    test_dict[first_3dobj] = first_3dobj.get_name()
    assert len(test_dict) == 1

    assert first_3dobj in test_dict
    assert first_3dobj_again in test_dict
    assert second_3dobj not in test_dict

    test_dict[first_3dobj_again] = first_3dobj_again.get_name()
    assert len(test_dict) == 1
    test_dict[second_3dobj] = second_3dobj.get_name()
    assert len(test_dict) == 2

    assert second_3dobj in test_dict

if __name__ == '__main__':
    main()
