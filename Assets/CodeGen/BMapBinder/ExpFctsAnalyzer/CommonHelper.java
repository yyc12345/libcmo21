//import java.io.FileInputStream;
import java.io.FileOutputStream;
//import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.nio.file.Paths;

public class CommonHelper {

	// =========== File Operations ===========

	private static Path getRootDirectoryPath() throws Exception {
		String rootDir = System.getenv("BMAP_BINDER_ROOT");
		if (rootDir == null) {
			throw new RuntimeException("Can not find essential environment variable BMAP_BINDER_ROOT");
		} else {
			return Paths.get(rootDir);
		}
	}

	public static String getInputFilePath(String filename) throws Exception {
		Path rootDir = getRootDirectoryPath();
		Path filePath = rootDir.resolve("Extracted").resolve(filename);
		return filePath.toString();
	}

	public static String getOutputFilePath(String filename) throws Exception {
		Path rootDir = getRootDirectoryPath();
		Path filePath = rootDir.resolve("Analyzed").resolve(filename);
		return filePath.toString();
	}

}
