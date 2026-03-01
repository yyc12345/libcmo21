use bmap_rs::bmap_wrapper as bmap;
use std::io::Read;
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
    let mut texture_folder = cliopts.ballance_dir.clone();
    texture_folder.push("Textures");
    let encodings = cliopts.encodings.clone();
    bmap::BMap::with_bmap(|b| {
        let r = b
            .create_file_reader(
                file_name.to_str().unwrap(),
                temp_folder.path().to_str().unwrap(),
                texture_folder.to_str().unwrap(),
                encodings.iter().map(|e| e.as_str()),
            )
            .unwrap();

        testsuits::common::test(&r);
        testsuits::eq::test(&r);
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
                encodings: encodings
                    .split(",")
                    .map(|e| e.to_string())
                    .collect::<Vec<_>>(),
            }
        }
    }
}

mod testsuits {
    use super::bmap;

    pub mod common {
        use super::bmap;
        use bmap::{
            BM3dEntityDecl, BMCameraDecl, BMLightDecl, BMMaterialDecl, BMMeshDecl, BMObjectDecl,
            BMTextureDecl, BMGroupDecl
        };

        pub fn test(reader: &bmap::BMFileReader) {
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

        #[rustfmt::skip]
        fn test_group(reader: &bmap::BMFileReader) {
            for gp in reader.get_groups().unwrap() {
                let gp = gp.unwrap();

                println!("{:?}", gp.get_name().unwrap());
                for gp_item in gp.get_objects().unwrap() {
                    let gp_item = gp_item.unwrap();
                    println!("\t{:?}", gp_item.get_name().unwrap());
                }
            }
        }

        #[rustfmt::skip]
        fn test_3dobject(reader: &bmap::BMFileReader) {
            for obj in reader.get_3dobjects().unwrap() {
                let obj = obj.unwrap();

                println!("{:?}", obj.get_name().unwrap());

                let current_mesh = obj.get_current_mesh().unwrap();
                let mesh_name = match current_mesh {
                    Some(mesh) => format!("{:?}", mesh.get_name()),
                    None => "<null>".to_string(),
                };
                println!("\tMesh: {}", mesh_name);
                println!("\tVisibility: {}", obj.get_visibility().unwrap());
                println!("\tMatrix: {:?}", obj.get_world_matrix().unwrap());
            }
        }

        #[rustfmt::skip]
        fn test_mesh(reader: &bmap::BMFileReader) {
            for mesh in reader.get_meshes().unwrap() {
                let mesh = mesh.unwrap();

                println!("{:?}", mesh.get_name().unwrap());

                println!("\tLit Mode: {:?}", mesh.get_lit_mode().unwrap());
                println!("\tVertex Count: {}", mesh.get_vertex_count().unwrap());
                println!("\tFace Count: {}", mesh.get_face_count().unwrap());
                println!("\tMaterial Slot Count: {}", mesh.get_material_slot_count().unwrap());
            }
        }

        #[rustfmt::skip]
        fn test_material(reader: &bmap::BMFileReader) {
            for mtl in reader.get_materials().unwrap() {
                let mtl = mtl.unwrap();

                println!("{:?}", mtl.get_name().unwrap());

                println!("\tDiffuse: {:?}", mtl.get_diffuse().unwrap());
                println!("\tAmbient: {:?}", mtl.get_ambient().unwrap());
                println!("\tSpecular: {:?}", mtl.get_specular().unwrap());
                println!("\tEmissive: {:?}", mtl.get_emissive().unwrap());

                println!("\tSpecular Power: {}", mtl.get_specular_power().unwrap());

                println!("\tTexture Border Color: {:?}", mtl.get_texture_border_color().unwrap());

                println!("\tTexture Blend Mode: {:?}", mtl.get_texture_blend_mode().unwrap());
                println!("\tTexture Min Mode: {:?}", mtl.get_texture_min_mode().unwrap());
                println!("\tTexture Mag Mode: {:?}", mtl.get_texture_mag_mode().unwrap());
                println!("\tSource Blend: {:?}", mtl.get_source_blend().unwrap());
                println!("\tDest Blend: {:?}", mtl.get_dest_blend().unwrap());
                println!("\tFill Mode: {:?}", mtl.get_fill_mode().unwrap());
                println!("\tShade Mode: {:?}", mtl.get_shade_mode().unwrap());

                println!("\tAlpha Test Enabled: {}", mtl.get_alpha_test_enabled().unwrap());
                println!("\tAlpha Blend Enabled: {}", mtl.get_alpha_blend_enabled().unwrap());
                println!("\tPerspective Correction Enabled: {}", mtl.get_perspective_correction_enabled().unwrap());
                println!("\tZ Write Enabled: {}", mtl.get_z_write_enabled().unwrap());
                println!("\tTwo Sided Enabled: {}", mtl.get_two_sided_enabled().unwrap());

                println!("\tAlpha Ref: {}", mtl.get_alpha_ref().unwrap());

                println!("\tAlpha Func: {:?}", mtl.get_alpha_func().unwrap());
                println!("\tZ Func: {:?}", mtl.get_z_func().unwrap());
            }
        }

        #[rustfmt::skip]
        fn test_texture(reader: &bmap::BMFileReader) {
            for tex in reader.get_textures().unwrap() {
                let tex = tex.unwrap();

                println!("{:?}", tex.get_name().unwrap());

                println!("\tFile Name: {:?}", tex.get_file_name().unwrap());
                println!("\tSave Options: {:?}", tex.get_save_options().unwrap());
                println!("\tVideo Format: {:?}", tex.get_video_format().unwrap());
            }
        }

        #[rustfmt::skip]
        fn test_target_light(reader: &bmap::BMFileReader) {
            for lit in reader.get_target_lights().unwrap() {
                let lit = lit.unwrap();

                println!("{:?}", lit.get_name().unwrap());

                println!("\tVisibility: {:?}", lit.get_visibility().unwrap());
                println!("\tMatrix: {:?}", lit.get_world_matrix().unwrap());

                println!("\tType: {:?}", lit.get_type().unwrap());
                println!("\tColor: {:?}", lit.get_color().unwrap());
                println!("\tConstant Attenuation: {}", lit.get_constant_attenuation().unwrap());
                println!("\tLinear Attenuation: {}", lit.get_linear_attenuation().unwrap());
                println!("\tQuadratic Attenuation: {}", lit.get_quadratic_attenuation().unwrap());
                println!("\tRange: {}", lit.get_range().unwrap());
                println!("\tHot Spot: {}", lit.get_hot_spot().unwrap());
                println!("\tFalloff: {}", lit.get_falloff().unwrap());
                println!("\tFalloff Shape: {}", lit.get_falloff_shape().unwrap());
            }
        }

        #[rustfmt::skip]
        fn test_target_camera(reader: &bmap::BMFileReader) {
            for cam in reader.get_target_cameras().unwrap() {
                let cam = cam.unwrap();

                println!("{:?}", cam.get_name().unwrap());

                println!("\tVisibility: {:?}", cam.get_visibility().unwrap());
                println!("\tMatrix: {:?}", cam.get_world_matrix().unwrap());

                println!("\tType: {:?}", cam.get_projection_type().unwrap());
                println!("\tOrthographic Zoom: {}", cam.get_orthographic_zoom().unwrap());
                println!("\tFront Plane: {}", cam.get_front_plane().unwrap());
                println!("\tBack Plane: {}", cam.get_back_plane().unwrap());
                println!("\tFov: {}", cam.get_fov().unwrap());

                let (width, height) = cam.get_aspect_ratio().unwrap();
                println!("\tAspect Ratio: {}:{}", width, height);
            }
        }
    }

    pub mod eq {
        use super::bmap;
        use std::collections::{BTreeSet, HashSet};

        pub fn test(reader: &bmap::BMFileReader) {
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
            assert!(test_hashset.insert(first_3dobj));
            assert!(!test_hashset.insert(first_3dobj_again));
            assert!(test_hashset.insert(second_3dobj));

            // Test BTreeSet
            let mut test_btreeset = BTreeSet::new();
            assert!(test_btreeset.insert(first_3dobj));
            assert!(!test_btreeset.insert(first_3dobj_again));
            assert!(test_btreeset.insert(second_3dobj));
        }
    }
}
