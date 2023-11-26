import java.io.OutputStreamWriter;
import java.util.Locale;

/**
 * Generic Enum Writer. Including Data Type Defination and Nameof Values.
 */
public class GeneralWriter {

	public static void writeEnums(OutputStreamWriter writer, EnumsHelper.EnumCollection_t prog) throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.CPP);
		for (EnumsHelper.Enum_t enum_t : prog.mEnums) {
			// write enum comment
			indent.briefComment(enum_t.mEnumComment);

			// write enum start
			indent.printf("enum class %s : %s {", enum_t.mEnumName,
					CommonHelper.getEnumUnderlyingType(enum_t.mCanUnsigned));
			indent.inc();

			// write enum entries
			for (EnumsHelper.EnumEntry_t enumEntry_t : enum_t.mEntries) {
				// write entry self
				if (enumEntry_t.mEntryValue == null) {
					indent.printf("%s,", enumEntry_t.mEntryName);
				} else {
					indent.printf("%s = %s,", enumEntry_t.mEntryName, enumEntry_t.mEntryValue);
				}

				// write entry comment after member
				indent.afterMemberComment(enumEntry_t.mEntryComment);
			}

			// write enum tail
			indent.dec();
			indent.puts("};");
		}
	}

	public static void writeEnums(String filename, EnumsHelper.EnumCollection_t prog) throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writeEnums(fs, prog);
		fs.close();
	}

	public static void writeEnum(OutputStreamWriter writer, EnumsHelper.Enum_t _enum) throws Exception {
		EnumsHelper.EnumCollection_t col = new EnumsHelper.EnumCollection_t();
		col.mEnums.add(_enum);
		writeEnums(writer, col);
	}

	public static void writeEnum(String filename, EnumsHelper.Enum_t _enum) throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writeEnum(fs, _enum);
		fs.close();
	}

	public static void writeAccVals(OutputStreamWriter writer, EnumsHelper.EnumCollection_t prog,
			CommonHelper.CKParts parts) throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.CPP);
		// write decls
		for (EnumsHelper.Enum_t enum_t : prog.mEnums) {
			indent.printf("extern const GeneralReflectionArray<LibCmo::%s::%s> %s;",
					CommonHelper.getCKPartsNamespace(parts), enum_t.mEnumName, enum_t.mEnumName);
		}

		indent.puts("");
		indent.puts("");
		indent.puts("");

		// write implements
		for (EnumsHelper.Enum_t enum_t : prog.mEnums) {
			// write enum desc header
			indent.printf("const GeneralReflectionArray<LibCmo::%s::%s> %s {", CommonHelper.getCKPartsNamespace(parts),
					enum_t.mEnumName, enum_t.mEnumName);
			indent.inc();

			// write enum desc entries
			for (EnumsHelper.EnumEntry_t enumEntry_t : enum_t.mEntries) {
				indent.printf("{ LibCmo::%s::%s::%s, {\"%s\"} },", CommonHelper.getCKPartsNamespace(parts),
						enum_t.mEnumName, enumEntry_t.mEntryName, enumEntry_t.mEntryName);
			}

			// write enum tail
			indent.dec();
			indent.puts("};");
		}
	}

	public static void writeAccVals(String filename, EnumsHelper.EnumCollection_t prog, CommonHelper.CKParts parts)
			throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writeAccVals(fs, prog, parts);
		fs.close();
	}

	public static void writeAccVal(OutputStreamWriter writer, EnumsHelper.Enum_t _enum, CommonHelper.CKParts parts)
			throws Exception {
		EnumsHelper.EnumCollection_t col = new EnumsHelper.EnumCollection_t();
		col.mEnums.add(_enum);
		writeAccVals(writer, col, parts);
	}

	public static void writeAccVal(String filename, EnumsHelper.Enum_t _enum, CommonHelper.CKParts parts)
			throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writeAccVal(fs, _enum, parts);
		fs.close();
	}

	public static void writePyEnums(OutputStreamWriter writer, EnumsHelper.EnumCollection_t prog) throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Python);
		for (EnumsHelper.Enum_t enum_t : prog.mEnums) {
			// write enum start
			indent.printf("class %s(enum.IntEnum):", enum_t.mEnumName);
			indent.inc();

			// write enum comment
			indent.briefComment(enum_t.mEnumComment);

			// write enum entries
			for (EnumsHelper.EnumEntry_t enumEntry_t : enum_t.mEntries) {
				// write entry self
				if (enumEntry_t.mEntryValue == null) {
					indent.printf("%s = auto()", enumEntry_t.mEntryName);
				} else {
					indent.printf("%s = %s", enumEntry_t.mEntryName,
							CommonHelper.convertToPythonNumber(enumEntry_t.mEntryValue));
				}

				// write entry comment after member
				indent.afterMemberComment(enumEntry_t.mEntryComment);
			}

			// enum tail
			indent.dec();
		}
	}

	public static void writePyEnums(String filename, EnumsHelper.EnumCollection_t prog) throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writePyEnums(fs, prog);
		fs.close();
	}

	public static void writePyEnum(OutputStreamWriter writer, EnumsHelper.Enum_t _enum) throws Exception {
		EnumsHelper.EnumCollection_t col = new EnumsHelper.EnumCollection_t();
		col.mEnums.add(_enum);
		writePyEnums(writer, col);
	}

	public static void writePyEnum(String filename, EnumsHelper.Enum_t _enum) throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writePyEnum(fs, _enum);
		fs.close();
	}

	private static String extractHumanReadableEntryName(String entry_name) {
		// remove first part (any content before underline '_')
		entry_name = entry_name.replaceFirst("^[a-zA-Z0-9]+_", "");

		// lower all chars except first char
		if (entry_name.length() < 1)
			return entry_name;
		else
			return entry_name.substring(0, 1) + entry_name.substring(1).toLowerCase(Locale.ROOT);
	}

	public static void writePyAccVals(OutputStreamWriter writer, EnumsHelper.EnumCollection_t prog) throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Python);

		// write implements
		for (EnumsHelper.Enum_t enum_t : prog.mEnums) {
			// write enum desc header
			indent.printf("g_Annotation_%s: dict[int, EnumAnnotation] = {", enum_t.mEnumName);
			indent.inc();

			// write enum desc entries
			for (EnumsHelper.EnumEntry_t enumEntry_t : enum_t.mEntries) {
				String comment = "";
				if (enumEntry_t.mEntryComment != null) {
					comment = CommonHelper.escapeString(enumEntry_t.mEntryComment);
				}

				indent.printf("%s.%s.value: EnumAnnotation(\"%s\", \"%s\"),", enum_t.mEnumName, enumEntry_t.mEntryName,
						extractHumanReadableEntryName(enumEntry_t.mEntryName), comment);
			}

			// write enum tail
			indent.dec();
			indent.puts("}");
		}
	}

	public static void writePyAccVals(String filename, EnumsHelper.EnumCollection_t prog) throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writePyAccVals(fs, prog);
		fs.close();
	}

	public static void writePyAccVal(OutputStreamWriter writer, EnumsHelper.Enum_t _enum) throws Exception {
		EnumsHelper.EnumCollection_t col = new EnumsHelper.EnumCollection_t();
		col.mEnums.add(_enum);
		writePyAccVals(writer, col);
	}

	public static void writePyAccVal(String filename, EnumsHelper.Enum_t _enum) throws Exception {
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		writePyAccVal(fs, _enum);
		fs.close();
	}

}
