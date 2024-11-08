using BMapSharp.BMapWrapper;
using System;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;

namespace BMapSharpTestbench {
    internal class Program {

        static void Main(string[] args) {
            // Check environment
            Console.OutputEncoding = Encoding.UTF8;
            if (!BMapSharp.BMapWrapper.Utils.IsBMapAvailable()) {
                Console.WriteLine("Fail to initialize native BMap.");
                Environment.Exit(0);
            }

            // Waiting debugger
            int pid = System.Diagnostics.Process.GetCurrentProcess().Id;
            Console.WriteLine($"C# PID is {pid}. Waiting debugger, press any key to continue...");
            Console.ReadKey(true);

            // Start testbench
            string file_name = "Level_02.NMO";
            string temp_folder = "Temp";
            string texture_folder = "F:\\Ballance\\Ballance\\Textures";
            string[] encodings = ["cp1252", "gb2312"];

            using (var reader = new BMapSharp.BMapWrapper.BMFileReader(file_name, temp_folder, texture_folder, encodings)) {
                TestCommon(reader);
                TestIEquatable(reader);
            }

            Console.WriteLine("Press any key to quit...");
            Console.ReadKey(true);

        }

        static void TestCommon(BMapSharp.BMapWrapper.BMFileReader reader) {
            // Console.WriteLine("===== Groups =====");
            // foreach (var gp in reader.GetGroups()) {
            //     Console.WriteLine(gp.GetName());
            //     foreach (var gp_item in gp.GetObjects()) {
            //         Console.WriteLine($"\t{gp_item.GetName()}");
            //     }
            // }

            // Console.WriteLine("===== 3dObjects =====");
            // foreach (var obj in reader.Get3dObjects()) {
            //     Console.WriteLine(obj.GetName());

            //     var current_mesh = obj.GetCurrentMesh();
            //     var mesh_name = current_mesh is null ? "<null>" : current_mesh.GetName();
            //     Console.WriteLine($"\tMesh: {mesh_name}");
            //     Console.WriteLine($"\tVisibility: {obj.GetVisibility()}");
            //     Console.WriteLine($"\tMatrix: {obj.GetWorldMatrix().ToManaged()}");
            // }

            // Console.WriteLine("===== Meshes =====");
            // foreach (var mesh in reader.GetMeshes()) {
            //     Console.WriteLine(mesh.GetName());

            //     Console.WriteLine($"\tLit Mode: {mesh.GetLitMode()}");
            //     Console.WriteLine($"\tVertex Count: {mesh.GetVertexCount()}");
            //     Console.WriteLine($"\tFace Count: {mesh.GetFaceCount()}");
            //     Console.WriteLine($"\tMaterial Slot Count: {mesh.GetMaterialSlotCount()}");
            // }

            Console.WriteLine("===== Materials =====");
            foreach (var mtl in reader.GetMaterials()) {
                Console.WriteLine(mtl.GetName());

                Console.WriteLine($"\tDiffuse: {mtl.GetDiffuse().ToManagedRGBA()}");
                Console.WriteLine($"\tAmbient: {mtl.GetAmbient().ToManagedRGBA()}");
                Console.WriteLine($"\tSpecular: {mtl.GetSpecular().ToManagedRGBA()}");
                Console.WriteLine($"\tEmissive: {mtl.GetEmissive().ToManagedRGBA()}");

                Console.WriteLine($"\tSpecular Power: {mtl.GetSpecularPower()}");

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

            // Console.WriteLine("===== Textures =====");
            // foreach (var tex in reader.GetTextures()) {
            //     Console.WriteLine(tex.GetName());

            //     Console.WriteLine($"\tFile Name: {tex.GetFileName()}");
            //     Console.WriteLine($"\tSave Options: {tex.GetSaveOptions()}");
            //     Console.WriteLine($"\tVideo Format: {tex.GetVideoFormat()}");
            // }

            Console.WriteLine("===== END =====");
        }

        static void TestIEquatable(BMapSharp.BMapWrapper.BMFileReader reader) {
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