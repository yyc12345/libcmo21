using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BMapSharpTest {

    public class CliException : Exception {
        public CliException() { }
        public CliException(string message)
            : base(message) { }
        public CliException(string message, Exception inner)
            : base(message, inner) { }
    }

    public class Cli {
        public Cli() {
            var filename = System.Environment.GetEnvironmentVariable("BMAP_FILE_NAME");
            if (filename is null) {
                throw new CliException("You must specify BMAP_FILE_NAME environment variable before running this test.");
            }
            this.FileName = filename;

            var ballance_dir = System.Environment.GetEnvironmentVariable("BMAP_BALLANCE_DIR");
            if (ballance_dir is null) {
                throw new CliException("You must specify BMAP_BALLANCE_DIR environment variable before running this test.");
            }
            this.BallanceDirectory = ballance_dir;

            var encodings = System.Environment.GetEnvironmentVariable("BMAP_ENCODINGS");
            if (encodings is null) {
                throw new CliException("You must specify BMAP_ENCODINGS environment variable before running this test.");
            }
            this.Encodings = encodings.Split(",");
        }

        /// <summary>
        /// The path to the map for loading.
        /// </summary>
        public string FileName { get; private set; }
        /// <summary>
        /// The path to the Ballance directory for finding textures
        /// </summary>
        public string BallanceDirectory { get; private set; }
        /// <summary>
        /// The name of encodings used by BMap for loading map.
        /// </summary>
        public string[] Encodings { get; private set; }
    }
    

}
