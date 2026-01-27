import java.io.OutputStreamWriter;
import com.google.gson.JsonArray;
import com.google.gson.JsonObject;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public class JsonWriter {

	private static JsonObject writeBEnumEntry(EnumsHelper.BEnumEntry enumEntry) {
		JsonObject data = new JsonObject();
		data.addProperty("name", enumEntry.mEntryName);
		data.addProperty("value", enumEntry.mEntryValue);
		data.addProperty("comment", enumEntry.mEntryComment);

		// Export hierarchy if possible
		if (enumEntry instanceof EnumsHelper.BHierarchyEnumEntry hierarchyEnumEntry) {
			// We only export name in hierarchy.
			// Otherwise, we may cause recursive calling.
			JsonArray entries = new JsonArray();
			for (EnumsHelper.BHierarchyEnumEntry subEntry : hierarchyEnumEntry.mHierarchy) {
				entries.add(subEntry.mEntryName);
			}
			data.add("hierarchy", entries);
		}

		return data;
	}

	private static JsonObject writeBEnum(EnumsHelper.BEnum benum) {
		JsonObject data = new JsonObject();
		data.addProperty("name", benum.mEnumName);
		data.addProperty("comment", benum.mEnumComment);
		data.addProperty("can_unsigned", benum.mCanUnsigned);
		data.addProperty("use_flags", benum.mUseFlags);
		data.addProperty("use_flags", benum.mUseFlags);

		JsonArray entries = new JsonArray();
		for (EnumsHelper.BEnumEntry enumEntry : benum.mEntries) {
			entries.add(writeBEnumEntry(enumEntry));
		}
		data.add("entries", entries);

		return data;
	}

	private static JsonArray writeBEnumCollection(EnumsHelper.BEnumCollection enumCollection) {
		JsonArray data = new JsonArray();
		for (EnumsHelper.BEnum benum : enumCollection.mEnums) {
			data.add(writeBEnum(benum));
		}
		return data;
	}

	private static void writeJson(String filename, EnumsHelper.BEnumCollection enumCollection) throws Exception {
		OutputStreamWriter writer = CommonHelper.openOutputFile(filename);
		//Gson gsonInstance = new GsonBuilder().setPrettyPrinting().disableHtmlEscaping().create();
		Gson gsonInstance = new GsonBuilder().serializeNulls().disableHtmlEscaping().create();
		writer.write(gsonInstance.toJson(writeBEnumCollection(enumCollection)));
		writer.close();
	}

	public static void writeEnums(String filename, EnumsHelper.BEnumCollection enumCollection) throws Exception {
		writeJson(filename, enumCollection);
	}

	public static void writeEnum(String filename, EnumsHelper.BEnum benum) throws Exception {
		// Build collection manually.
		EnumsHelper.BEnumCollection collection = new EnumsHelper.BEnumCollection();
		collection.mEnums.add(benum);
		// Call underlying writer
		writeEnums(filename, collection);
	}
}
