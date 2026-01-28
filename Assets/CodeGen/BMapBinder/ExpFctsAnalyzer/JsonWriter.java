import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public class JsonWriter {
	
	private static JsonObject writeVariableType(ExpFctsHelper.VariableType vt) {
		JsonObject data = new JsonObject();
		
		JsonArray hierarchy = new JsonArray();
		for (String item : vt.getBaseTypeHierarchy()) {
			hierarchy.add(item);
		}
		data.add("hierarchy", hierarchy);
		data.addProperty("pointer_level", vt.getPointerLevel());
		
		return data;
	}
	
	private static JsonObject writeExpFctParam(ExpFctsHelper.ExpFctParam param) {
		JsonObject data = new JsonObject();
		data.addProperty("name", param.mVarName);
		data.addProperty("is_input", param.mIsInput);
		data.addProperty("desc", param.mVarDesc);
		data.add("type", writeVariableType(param.mVarType));
		
		return data;
	}
	
	private static JsonObject writeExpFct(ExpFctsHelper.ExpFct fct) {
		JsonObject data = new JsonObject();
		data.addProperty("name", fct.mFctName);
		data.add("return", writeVariableType(fct.mFctRetType));
		
		JsonArray paramList = new JsonArray();
		for (ExpFctsHelper.ExpFctParam param : fct.mFctParams) {
			paramList.add(writeExpFctParam(param));
		}
		data.add("params", paramList);
		
		return data;
	}

	private static JsonArray writeExpFctCollection(ExpFctsHelper.ExpFctCollection fctCollection) {
		JsonArray data = new JsonArray();
		for (ExpFctsHelper.ExpFct fct : fctCollection.mFcts) {
			data.add(writeExpFct(fct));
		}
		return data;
	}

	public static void writeJson(ExpFctsHelper.ExpFctCollection data) throws Exception {
		FileOutputStream fs = new FileOutputStream(CommonHelper.getOutputFilePath("BMExports.json"));
		OutputStreamWriter writer = new OutputStreamWriter(fs, StandardCharsets.UTF_8);
		//Gson gsonInstance = new GsonBuilder().serializeNulls().setPrettyPrinting().disableHtmlEscaping().create();
		Gson gsonInstance = new GsonBuilder().serializeNulls().disableHtmlEscaping().create();
		writer.write(gsonInstance.toJson(writeExpFctCollection(data)));
		writer.close();
	}
}
