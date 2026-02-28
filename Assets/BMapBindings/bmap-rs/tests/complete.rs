use bmap_rs::bmap_wrapper as bmap;
use std::path::PathBuf;

#[test]
fn test_complete() {
    // Parse arguments
    let cliopts = cli::Cli::new();

    // Check BMap status.
    if !bmap::BMap::is_available() {
        panic!("Fail to initialize native BMap.");
    }

    // Waiting debugger
    println!(
        "Rust PID is {}. Waiting for debugger, press any key to continue...",
        std::process::id()
    );
    let buffer = &mut [0u8];
    std::io::stdin().read_exact(buffer).unwrap();

    // Start testbench
    let file_name = cliopts.file_name.clone();
    let temp_folder = tempfile::tempdir().unwrap();
    let texture_folder = cliopts.ballance_dir.clone();
    texture_folder.push("Textures");
    let encodings = cliopts.encodings.clone();
    bmap::BMap::with_bmap(|b| {
        let r = b.create_file_reader("", "", "", std::iter::once(""));
    });
    drop(temp_folder);
}

mod cli {
    use super::PathBuf;

    pub struct Cli {
        /// The path to the map for loading.
        pub file_name: PathBuf,
        /// The path to the Ballance directory for finding textures
        pub ballance_dir: PathBuf,
        /// The name of encodings used by BMap for loading map.
        pub encodings: Vec<String>,
    }

    impl Cli {
        pub fn new() -> Self {
            let file_name = std::env::var("BMAP_FILE_NAME").expect(
                "You must specify BMAP_FILE_NAME environment variable before running this test.",
            );
            let ballance_dir = std::env::var("BMAP_BALLANCE_DIR").expect(
                "You must specify BMAP_BALLANCE_DIR environment variable before running this test.",
            );
            let encodings = std::env::var("BMAP_ENCODINGS").expect(
                "You must specify BMAP_ENCODINGS environment variable before running this test.",
            );

            Self {
                file_name: PathBuf::from(file_name),
                ballance_dir: PathBuf::from(ballance_dir),
                encodings: encodings.split(",").collect(),
            }
        }
    }
}

mod testsuits {
    use super::bmap;

    pub mod common {
        use super::bmap;

        pub fn test(reader: bmap::BMFileReader) {
            println!("===== Groups =====");
            test_group(reader);
            println!("===== 3dObjects =====");
            test_3dobject(reader);
            println!("===== Meshes =====");
            test_mesh(reader);
            println!("===== Materials =====");
            test_material(reader);
            println!("===== Textures =====");
            test_texture(reader);
            println!("===== Target Lights =====");
            test_target_light(reader);
            println!("===== Target Cameras =====");
            test_target_camera(reader);
            println!("===== END =====");
        }

        fn test_group(reader: bmap::BMFileReader) {
            for gp in reader.get_groups() {
                println!("{}", gp.get_name());
                for gp_item in gp.get_objects() {
                    println!("\t{}", gp_item.get_name());
                }
            }
        }

        fn test_3dobject(reader: bmap::BMFileReader) {
            for obj in reader.get_3dobjects() {
                println!("{}", obj.get_name());

                let current_mesh = obj.get_current_mesh();
                let mesh_name = if let Some(mesh) = current_mesh {
                    mesh.get_name()
                } else {
                    "<null>".to_string()
                };
                println!("\tMesh: {}", mesh_name);
                println!("\tVisibility: {}", obj.get_visibility());
                println!("\tMatrix: {:?}", obj.get_world_matrix().to_const());
            }
        }

        fn test_mesh(reader: bmap::BMFileReader) {
            for mesh in reader.get_meshs() {
                println!("{}", mesh.get_name());

                println!("\tLit Mode: {}", mesh.get_lit_mode());
                println!("\tVertex Count: {}", mesh.get_vertex_count());
                println!("\tFace Count: {}", mesh.get_face_count());
                println!("\tMaterial Slot Count: {}", mesh.get_material_slot_count());
            }
        }

        fn test_material(reader: bmap::BMFileReader) {
            for mtl in reader.get_materials() {
                println!("{}", mtl.get_name());

                println!("\tDiffuse: {:?}", mtl.get_diffuse().to_const_rgba());
                println!("\tAmbient: {:?}", mtl.get_ambient().to_const_rgba());
                println!("\tSpecular: {:?}", mtl.get_specular().to_const_rgba());
                println!("\tEmissive: {:?}", mtl.get_emissive().to_const_rgba());

                println!("\tSpecular Power: {}", mtl.get_specular_power());

                println!(
                    "\tTexture Border Color: {:?}",
                    mtl.get_texture_border_color().to_const_rgba()
                );

                println!("\tTexture Blend Mode: {}", mtl.get_texture_blend_mode());
                println!("\tTexture Min Mode: {}", mtl.get_texture_min_mode());
                println!("\tTexture Mag Mode: {}", mtl.get_texture_mag_mode());
                println!("\tSource Blend: {}", mtl.get_source_blend());
                println!("\tDest Blend: {}", mtl.get_dest_blend());
                println!("\tFill Mode: {}", mtl.get_fill_mode());
                println!("\tShade Mode: {}", mtl.get_shade_mode());

                println!("\tAlpha Test Enabled: {}", mtl.get_alpha_test_enabled());
                println!("\tAlpha Blend Enabled: {}", mtl.get_alpha_blend_enabled());
                println!(
                    "\tPerspective Correction Enabled: {}",
                    mtl.get_perspective_correction_enabled()
                );
                println!("\tZ Write Enabled: {}", mtl.get_z_write_enabled());
                println!("\tTwo Sided Enabled: {}", mtl.get_two_sided_enabled());

                println!("\tAlpha Ref: {}", mtl.get_alpha_ref());

                println!("\tAlpha Func: {}", mtl.get_alpha_func());
                println!("\tZ Func: {}", mtl.get_z_func());
            }
        }

        fn test_texture(reader: bmap::BMFileReader) {
            for tex in reader.get_textures() {
                println!("{}", tex.get_name());

                println!("\tFile Name: {}", tex.get_file_name());
                println!("\tSave Options: {}", tex.get_save_options());
                println!("\tVideo Format: {}", tex.get_video_format());
            }
        }

        fn test_target_light(reader: bmap::BMFileReader) {
            for lit in reader.get_target_lights() {
                println!("{}", lit.get_name());

                println!("\tVisibility: {}", lit.get_visibility());
                println!("\tMatrix: {:?}", lit.get_world_matrix().to_const());

                println!("\tType: {}", lit.get_type());
                println!("\tColor: {:?}", lit.get_color().to_const_rgba());
                println!("\tConstant Attenuation: {}", lit.get_constant_attenuation());
                println!("\tLinear Attenuation: {}", lit.get_linear_attenuation());
                println!(
                    "\tQuadratic Attenuation: {}",
                    lit.get_quadratic_attenuation()
                );
                println!("\tRange: {}", lit.get_range());
                println!("\tHot Spot: {}", lit.get_hot_spot());
                println!("\tFalloff: {}", lit.get_falloff());
                println!("\tFalloff Shape: {}", lit.get_falloff_shape());
            }
        }

        fn test_target_camera(reader: bmap::BMFileReader) {
            for cam in reader.get_target_cameras() {
                println!("{}", cam.get_name());

                println!("\tVisibility: {}", cam.get_visibility());
                println!("\tMatrix: {:?}", cam.get_world_matrix().to_const());

                println!("\tType: {}", cam.get_projection_type());
                println!("\tOrthographic Zoom: {}", cam.get_orthographic_zoom());
                println!("\tFront Plane: {}", cam.get_front_plane());
                println!("\tBack Plane: {}", cam.get_back_plane());
                println!("\tFov: {}", cam.get_fov());

                let (width, height) = cam.get_aspect_ratio();
                println!("\tAspect Ratio: {}:{}", width, height);
            }
        }
    }

    pub mod eq {
        use super::bmap;
        use std::collections::{BTreeSet, HashSet};

        pub fn test(reader: bmap::BMFileReader) {
            // Check requirements
            assert!(
                reader.get_3dobject_count().unwrap() >= 2,
                r#"Invalid file for test Eq.
We can not perform Eq test because the length of 3dObject is too short (must greater than 2). Please choose another file to perform."#
            );

            // Prepare variables
            let all_3dobjects = reader
                .get_3dobjects()
                .unwrap()
                .map(|o| o.unwrap())
                .collect::<Vec<_>>();
            let first_3dobj = &all_3dobjects[0];
            let second_3dobj = &all_3dobjects[1];
            let all_3dobjects_again = reader
                .get_3dobjects()
                .unwrap()
                .map(|o| o.unwrap())
                .collect::<Vec<_>>();
            let first_3dobj_again = &all_3dobjects_again[0];

            // Test HashSet
            let mut test_hashset = HashSet::new();
            assert!(test_hashset.insert(first_3dobj.clone()));
            assert!(!test_hashset.insert(first_3dobj_again.clone()));
            assert!(test_hashset.insert(second_3dobj));

            // Test BTreeSet
            let mut test_btreeset = BTreeSet::new();
            assert!(test_btreeset.insert(first_3dobj.clone()));
            assert!(!test_btreeset.insert(first_3dobj_again.clone()));
            assert!(test_btreeset.insert(second_3dobj));
        }
    }
}
