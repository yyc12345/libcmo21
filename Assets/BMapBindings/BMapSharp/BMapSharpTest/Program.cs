using System;
using System.IO;
using System.Text;
using BMapSharp;

namespace BMapSharpTest {
    internal class Program {

        static int Main(string[] args) {
            // Parse arguments
            Cli cli;
            try {
                cli = new Cli();
            } catch (CliException e) {
                Console.WriteLine($"Can not launch test. Reason: {e.Message}");
                return 2;
            }

            // Check console encoding.
            Console.OutputEncoding = Encoding.UTF8;
            // Check BMap status.
            if (!BMapSharp.BMapWrapper.Utils.IsBMapAvailable()) {
                Console.WriteLine("Fail to initialize native BMap.");
                return 1;
            }

            // Waiting debugger
            int pid = System.Diagnostics.Process.GetCurrentProcess().Id;
            Console.WriteLine($"C# PID is {pid}. Waiting debugger, press any key to continue...");
            Console.ReadKey(true);

            // Start testbench
            string file_name = cli.FileName;
            var temp_dir_info = Directory.CreateTempSubdirectory();
            string temp_dir = temp_dir_info.FullName;
            string texture_dir = Path.Combine(cli.BallanceDir, "Textures");
            string[] encodings = cli.Encodings;

            using (var reader = new BMapSharp.BMapWrapper.BMFileReader(file_name, temp_dir, texture_dir, encodings)) {
                TestSuits.TestCommon.Test(reader);
                TestSuits.TestIEquatable.Test(reader);
            }

            Console.WriteLine("Press any key to quit...");
            Console.ReadKey(true);
            return 0;
        }

    }
}