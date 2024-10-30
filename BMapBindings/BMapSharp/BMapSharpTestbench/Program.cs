using System;
using System.Text;

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
                // }

                Console.WriteLine("===== Materials =====");
                foreach (var mtl in reader.GetMaterials()) {
                    Console.WriteLine(mtl.GetName());

                    Console.WriteLine($"\tDiffuse: {mtl.GetDiffuse().ToManagedRGBA()}");
                    Console.WriteLine($"\tAmbient: {mtl.GetAmbient().ToManagedRGBA()}");
                    Console.WriteLine($"\tSpecular: {mtl.GetSpecular().ToManagedRGBA()}");
                    Console.WriteLine($"\tEmissive: {mtl.GetEmissive().ToManagedRGBA()}");
                    Console.WriteLine($"\tSpecular Power: {mtl.GetSpecularPower()}");
                }

                // Console.WriteLine("===== Textures =====");
                // foreach (var tex in reader.GetTextures()) {
                //     Console.WriteLine(tex.GetName());

                //     Console.WriteLine($"\tFile Name: {tex.GetFileName()}");
                //     Console.WriteLine($"\tSave Options: {tex.GetSaveOptions()}");
                //     Console.WriteLine($"\tVideo Format: {tex.GetVideoFormat()}");
                // }

            }

            Console.WriteLine("===== Done =====");
            Console.ReadKey(true);

        }
    }
}