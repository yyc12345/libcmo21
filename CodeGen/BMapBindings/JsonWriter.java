import java.io.OutputStreamWriter;
import java.util.Vector;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public class JsonWriter {
	
	private static JsonObject writeVariableType(VariableType vt) {
		JsonObject data = new JsonObject();
		
		JsonArray hierarchy = new JsonArray();
		for (String item : vt.getBaseTypeHierarchy()) {
			hierarchy.add(item);
		}
		data.add("hierarchy", hierarchy);
		data.addProperty("pointer_level", vt.getPointerLevel());
		
		return data;
	}
	
	private static JsonObject writeExpFctParamDecl(ExpFctParamDecl paramdecl) {
		JsonObject data = new JsonObject();
		data.addProperty("name", paramdecl.mVarName);
		data.addProperty("is_input", paramdecl.mIsInput);
		data.addProperty("desc", paramdecl.mVarDesc);
		data.add("type", writeVariableType(paramdecl.mVarType));
		
		return data;
	}
	
	private static JsonObject writeExpFctDecl(ExpFctDecl fctdecl) {
		JsonObject data = new JsonObject();
		data.addProperty("name", fctdecl.mFctName);
		data.add("return", writeVariableType(fctdecl.mFctRetType));
		
		JsonArray paramlist = new JsonArray();
		for (ExpFctParamDecl paramdecl : fctdecl.mFctParams) {
			paramlist.add(writeExpFctParamDecl(paramdecl));
		}
		data.add("params", paramlist);
		
		return data;
	}
	
	public static void writeJson(Vector<ExpFctDecl> data) throws Exception {
		OutputStreamWriter writer = CommonHelper.openWriter("dest/BMExports.json");
		//Gson gson_instance = new GsonBuilder().setPrettyPrinting().disableHtmlEscaping().create();
		Gson gson_instance = new GsonBuilder().disableHtmlEscaping().create();
		
		JsonArray fcts = new JsonArray();
		for (ExpFctDecl fctdecl : data) {
			fcts.add(writeExpFctDecl(fctdecl));
		}
		
		writer.write(gson_instance.toJson(fcts));
		writer.close();
	}
}
