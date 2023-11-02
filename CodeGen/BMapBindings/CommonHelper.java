import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;

public class CommonHelper {

	public static InputStreamReader openReader(String filename) throws Exception {
		FileInputStream fs = new FileInputStream(filename);
		return new InputStreamReader(fs, StandardCharsets.UTF_8);
	}

	public static OutputStreamWriter openWriter(String filename) throws Exception {
		FileOutputStream fs = new FileOutputStream(filename);
		return new OutputStreamWriter(fs, StandardCharsets.UTF_8);
	}

	public static void writeSnippet(OutputStreamWriter writer, String snippet_path) throws Exception {
		// open snippet
		InputStreamReader reader = openReader(snippet_path);
		// write into writer
		reader.transferTo(writer);
		reader.close();
	}
	
	public static String getDoxygenInOutStr(boolean isInput) {
		return isInput ? "in" : "out";
	}

}
