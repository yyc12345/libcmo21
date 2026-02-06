using System;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;
using BMapSharp;

namespace BMapSharpTest {
    internal class Program {

        static void Main(string[] args) {
            // Parse arguments
            try {

            } catch (CliException e) {
                Console.WriteLine($"Can not launch test. Reason: {e.Message}");
                Environment.Exit(0);
            }

            // Check console encoding.
            Console.OutputEncoding = Encoding.UTF8;
            // Check BMap status.
            if (!BMapSharp.BMapWrapper.Utils.IsBMapAvailable()) {
                Console.WriteLine("Fail to initialize native BMap.");
                Environment.Exit(0);
            }

            // Waiting debugger
            int pid = System.Diagnostics.Process.GetCurrentProcess().Id;
            Console.WriteLine($"C# PID is {pid}. Waiting debugger, press any key to continue...");
            Console.ReadKey(true);

            // Start testbench
            string file_name = resolved_args.mFileName; // "LightCameraTest.nmo";
            string temp_folder = resolved_args.mTempFolder; // "Temp";
            string texture_folder = resolved_args.mTextureFolder; // "F:\\Ballance\\Ballance\\Textures";
            string[] encodings = resolved_args.mEncodings; // ["cp1252", "gb2312"];

            using (var reader = new BMapSharp.BMapWrapper.BMFileReader(file_name, temp_folder, texture_folder, encodings)) {
                TestSuits.TestCommon.Test(reader);
                TestSuits.TestIEquatable.Test(reader);
            }

            Console.WriteLine("Press any key to quit...");
            Console.ReadKey(true);

        }

    }
}