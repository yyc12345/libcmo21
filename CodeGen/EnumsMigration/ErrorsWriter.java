import java.io.OutputStreamWriter;

/**
 * The nameof values writer for CKERROR
 */
public class ErrorsWriter {
	public static void writeAccVal(String filename, EnumsHelper.Enum_t errors) throws Exception {
		OutputStreamWriter writer = CommonHelper.openOutputFile(filename);
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.CPP);

		indent.puts("const CkErrorReflectionArray CKERROR {");
		indent.inc();
		for (EnumsHelper.EnumEntry_t entry : errors.mEntries) {
			String comment = CommonHelper.escapeString(entry.mEntryComment);
			if (comment == null)
				comment = "";

			indent.printf("{ LibCmo::CK2::CKERROR::%s, { \"%s\", \"%s\" } },", entry.mEntryName, entry.mEntryName,
					comment);
		}
		indent.dec();
		indent.puts("};");

		writer.close();
	}
}
