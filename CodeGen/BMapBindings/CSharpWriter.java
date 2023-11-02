import java.io.OutputStreamWriter;
import java.util.Vector;

public class CSharpWriter {
	
	public static void writeCSharpCode(Vector<ExpFctDecl> data) throws Exception {
		OutputStreamWriter writer = CommonHelper.openWriter("dest/BMExports.cs");
		writer.write("// WIP");
		writer.close();
	}
	
}
