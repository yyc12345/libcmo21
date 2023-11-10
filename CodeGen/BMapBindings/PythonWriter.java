import java.io.OutputStreamWriter;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.stream.Collectors;

public class PythonWriter {

	private static final Map<String, String> g_CppTypeMap = cppTypeMapCreator();

	private static Map<String, String> cppTypeMapCreator() {
		Map<String, String> cache = new HashMap<String, String>();
		cache.put("CKSTRING", "CKSTRING");
		cache.put("CKDWORD", "CKDWORD");
		cache.put("CKWORD", "CKWORD");
		cache.put("CKINT", "CKINT");
		cache.put("bool", "bool");
		cache.put("CKFLOAT", "CKFLOAT");
		cache.put("CKBYTE", "CKBYTE");
		cache.put("CK_ID", "CKID");
		cache.put("BMFile", "void");
		cache.put("BMMeshTransition", "void");
		cache.put("VxVector3", "VxVector3");
		cache.put("VxVector2", "VxVector2");
		cache.put("VxColor", "VxColor");
		cache.put("VxMatrix", "VxMatrix");
		cache.put("CK_TEXTURE_SAVEOPTIONS", "enum");
		cache.put("VX_PIXELFORMAT", "enum");
		cache.put("VXTEXTURE_BLENDMODE", "enum");
		cache.put("VXTEXTURE_FILTERMODE", "enum");
		cache.put("VXTEXTURE_ADDRESSMODE", "enum");
		cache.put("VXBLEND_MODE", "enum");
		cache.put("VXFILL_MODE", "enum");
		cache.put("VXSHADE_MODE", "enum");
		cache.put("VXCMPFUNC", "enum");
		return Collections.unmodifiableMap(cache);
	}

	public static String pythonTypeGetter(ExpFctParamDecl paramdecl) {
		VariableType vt = paramdecl.mVarType;
		if (!paramdecl.mIsInput) {
			vt = vt.getPointerOfThis();
		}

		StringBuilder sb = new StringBuilder();
		sb.append("bm_");
		sb.append(g_CppTypeMap.get(vt.getBaseType()));
		if (vt.isPointer()) {
			sb.append("_");
			sb.append(String.join("", Collections.nCopies(vt.getPointerLevel(), "p")));
		}
		return sb.toString();
	}

	public static void writePythonCode(Vector<ExpFctDecl> data) throws Exception {
		OutputStreamWriter writer = CommonHelper.openWriter("dest/BMExports.py");
		IndentHelper helper = new IndentHelper(writer);

		// write snippet
		CommonHelper.writeSnippet(writer, "snippets/header.py");

		// write function decls
		
		helper.puts("");
		helper.puts("#region Function Defines");
		helper.puts("");
		
		for (ExpFctDecl fctdecl : data) {
			// write annotation
			// function name
			helper.printf("## %s", fctdecl.mFctName);
			// param
			for (ExpFctParamDecl paramdecl : fctdecl.mFctParams) {
				helper.printf("#  @param %s[%s] Type: %s. %s%s", paramdecl.mVarName,
						CommonHelper.getDoxygenInOutStr(paramdecl.mIsInput), paramdecl.mVarType.toCType(),
						(paramdecl.mIsInput ? "" : "Use ctypes.byref(data) pass it. "), paramdecl.mVarDesc);
			}
			// return val
			helper.puts("#  @return True if no error, otherwise False.");

			// write real declaration
			// first, we need join all param
			helper.printf("%s = _create_bmap_func('%s', [%s])", fctdecl.mFctName, fctdecl.mFctName, fctdecl.mFctParams
					.stream().map(value -> pythonTypeGetter(value)).collect(Collectors.joining(", ")));
		}

		helper.puts("");
		helper.puts("#endregion");
		helper.puts("");
		
		writer.close();
	}

}
