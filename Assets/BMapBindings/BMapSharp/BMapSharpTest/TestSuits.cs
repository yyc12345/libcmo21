using System;
using System.Collections.Generic;
using System.Diagnostics;
using BMapSharp.BMapWrapper;

namespace BMapSharpTest.TestSuits {

    public static class TestCommon {

        public static void Test(BMFileReader reader) {
            Console.WriteLine("===== Groups =====");
            TestGroup(reader);
            Console.WriteLine("===== 3dObjects =====");
            Test3dObject(reader);
            Console.WriteLine("===== Meshes =====");
            TestMesh(reader);
            Console.WriteLine("===== Materials =====");
            TestMaterial(reader);
            Console.WriteLine("===== Textures =====");
            TestTexture(reader);
            Console.WriteLine("===== Target Lights =====");
            TestTargetLight(reader);
            Console.WriteLine("===== Target Cameras =====");
            TestTargetCamera(reader);
            Console.WriteLine("===== END =====");
        }

        private static void TestGroup(BMFileReader reader) {
            foreach (var gp in reader.GetGroups()) {
                Console.WriteLine(gp.GetName());
                foreach (var gp_item in gp.GetObjects()) {
                    Console.WriteLine($"\t{gp_item.GetName()}");
                }
            }
        }

        private static void Test3dObject(BMFileReader reader) {
            foreach (var obj in reader.Get3dObjects()) {
                Console.WriteLine(obj.GetName());

                var current_mesh = obj.GetCurrentMesh();
                var mesh_name = current_mesh is null ? "<null>" : current_mesh.GetName();
                Console.WriteLine($"\tMesh: {mesh_name}");
                Console.WriteLine($"\tVisibility: {obj.GetVisibility()}");
                Console.WriteLine($"\tMatrix: {obj.GetWorldMatrix().ToManaged()}");
            }
        }

        private static void TestMesh(BMFileReader reader) {
            foreach (var mesh in reader.GetMeshes()) {
                Console.WriteLine(mesh.GetName());

                Console.WriteLine($"\tLit Mode: {mesh.GetLitMode()}");
                Console.WriteLine($"\tVertex Count: {mesh.GetVertexCount()}");
                Console.WriteLine($"\tFace Count: {mesh.GetFaceCount()}");
                Console.WriteLine($"\tMaterial Slot Count: {mesh.GetMaterialSlotCount()}");
            }
        }

        private static void TestMaterial(BMFileReader reader) {
            foreach (var mtl in reader.GetMaterials()) {
                Console.WriteLine(mtl.GetName());

                Console.WriteLine($"\tDiffuse: {mtl.GetDiffuse().ToManagedRGBA()}");
                Console.WriteLine($"\tAmbient: {mtl.GetAmbient().ToManagedRGBA()}");
                Console.WriteLine($"\tSpecular: {mtl.GetSpecular().ToManagedRGBA()}");
                Console.WriteLine($"\tEmissive: {mtl.GetEmissive().ToManagedRGBA()}");

                Console.WriteLine($"\tSpecular Power: {mtl.GetSpecularPower()}");

                var current_texture = mtl.GetTexture();
                var texture_name = current_texture is null ? "<null>" : current_texture.GetName();
                Console.WriteLine($"\tTexture: {texture_name}");

                Console.WriteLine($"\tTexture Border Color: {mtl.GetTextureBorderColor().ToManagedRGBA()}");

                Console.WriteLine($"\tTexture Blend Mode: {mtl.GetTextureBlendMode()}");
                Console.WriteLine($"\tTexture Min Mode: {mtl.GetTextureMinMode()}");
                Console.WriteLine($"\tTexture Mag Mode: {mtl.GetTextureMagMode()}");
                Console.WriteLine($"\tSource Blend: {mtl.GetSourceBlend()}");
                Console.WriteLine($"\tDest Blend: {mtl.GetDestBlend()}");
                Console.WriteLine($"\tFill Mode: {mtl.GetFillMode()}");
                Console.WriteLine($"\tShade Mode: {mtl.GetShadeMode()}");

                Console.WriteLine($"\tAlpha Test Enabled: {mtl.GetAlphaTestEnabled()}");
                Console.WriteLine($"\tAlpha Blend Enabled: {mtl.GetAlphaBlendEnabled()}");
                Console.WriteLine($"\tPerspective Correction Enabled: {mtl.GetPerspectiveCorrectionEnabled()}");
                Console.WriteLine($"\tZ Write Enabled: {mtl.GetZWriteEnabled()}");
                Console.WriteLine($"\tTwo Sided Enabled: {mtl.GetTwoSidedEnabled()}");

                Console.WriteLine($"\tAlpha Ref: {mtl.GetAlphaRef()}");

                Console.WriteLine($"\tAlpha Func: {mtl.GetAlphaFunc()}");
                Console.WriteLine($"\tZ Func: {mtl.GetZFunc()}");
            }
        }

        private static void TestTexture(BMFileReader reader) {
            foreach (var tex in reader.GetTextures()) {
                Console.WriteLine(tex.GetName());

                Console.WriteLine($"\tFile Name: {tex.GetFileName()}");
                Console.WriteLine($"\tSave Options: {tex.GetSaveOptions()}");
                Console.WriteLine($"\tVideo Format: {tex.GetVideoFormat()}");
            }
        }

        private static void TestTargetLight(BMFileReader reader) {
            foreach (var lit in reader.GetTargetLights()) {
                Console.WriteLine(lit.GetName());

                Console.WriteLine($"\tVisibility: {lit.GetVisibility()}");
                Console.WriteLine($"\tMatrix: {lit.GetWorldMatrix().ToManaged()}");

                Console.WriteLine($"\tType: {lit.GetLightType()}");
                Console.WriteLine($"\tColor: {lit.GetColor().ToManagedRGBA()}");
                Console.WriteLine($"\tConstant Attenuation: {lit.GetConstantAttenuation()}");
                Console.WriteLine($"\tLinear Attenuation: {lit.GetLinearAttenuation()}");
                Console.WriteLine($"\tQuadratic Attenuation: {lit.GetQuadraticAttenuation()}");
                Console.WriteLine($"\tRange: {lit.GetRange()}");
                Console.WriteLine($"\tHot Spot: {lit.GetHotSpot()}");
                Console.WriteLine($"\tFalloff: {lit.GetFalloff()}");
                Console.WriteLine($"\tFalloff Shape: {lit.GetFalloffShape()}");
            }
        }

        private static void TestTargetCamera(BMFileReader reader) {
            foreach (var cam in reader.GetTargetCameras()) {
                Console.WriteLine(cam.GetName());

                Console.WriteLine($"\tVisibility: {cam.GetVisibility()}");
                Console.WriteLine($"\tMatrix: {cam.GetWorldMatrix().ToManaged()}");

                Console.WriteLine($"\tType: {cam.GetProjectionType()}");
                Console.WriteLine($"\tOrthographic Zoom: {cam.GetOrthographicZoom()}");
                Console.WriteLine($"\tFront Plane: {cam.GetFrontPlane()}");
                Console.WriteLine($"\tBack Plane: {cam.GetBackPlane()}");
                Console.WriteLine($"\tFov: {cam.GetFov()}");

                cam.GetAspectRatio(out var width, out var height);
                Console.WriteLine($"\tAspect Ratio: {width}:{height}");
            }
        }
    }

    public static class TestIEquatable {

        public static void Test(BMFileReader reader) {
            if (reader.Get3dObjectCount() < 2u) {
                Debug.Fail(
                    "Invalid file for test IEquatable.",
                    "We can not perform IEquatable test because the length of 3dObject is too short (must greater than 2). Please choose another file to perform."
                );
                return;
            }

            // Prepare test variables
            var all_3dobjects = new List<BM3dObject>(reader.Get3dObjects());
            var first_3dobj = all_3dobjects[0];
            var second_3dobj = all_3dobjects[1];
            all_3dobjects = new List<BM3dObject>(reader.Get3dObjects());
            var first_3dobj_again = all_3dobjects[0];
            Debug.Assert(!Object.ReferenceEquals(first_3dobj, first_3dobj_again));

            // Hashtable test
            var test_hashset = new HashSet<BM3dObject>();
            Debug.Assert(test_hashset.Add(first_3dobj));
            Debug.Assert(!test_hashset.Add(first_3dobj_again));
            Debug.Assert(test_hashset.Add(second_3dobj));

            // Dictionary test
            var test_dictionary = new Dictionary<BM3dObject, string>();
            Debug.Assert(test_dictionary.TryAdd(first_3dobj, first_3dobj.GetName()));
            Debug.Assert(!test_dictionary.TryAdd(first_3dobj_again, first_3dobj_again.GetName()));
            Debug.Assert(test_dictionary.TryAdd(second_3dobj, second_3dobj.GetName()));

        }

    }

}
