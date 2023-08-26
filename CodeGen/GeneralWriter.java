import java.io.OutputStreamWriter;

/**
 * Generic Enum Writer. Including Data Type Defination and Nameof Values.
 */
public class GeneralWriter {

	public static void writeEnums(OutputStreamWriter writer, EnumsHelper.EnumCollection_t prog) throws Exception {
		IndentHelper indent = new IndentHelper(writer);
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

	public static void writeEnum(OutputStreamWriter writer, EnumsHelper.Enum_t _enum) throws Exception {
		EnumsHelper.EnumCollection_t col = new EnumsHelper.EnumCollection_t();
		col.mEnums.add(_enum);
		writeEnums(writer, col);
	}

	public static void writeAccVals(OutputStreamWriter writer, EnumsHelper.EnumCollection_t prog,
			CommonHelper.CKParts parts) throws Exception {
		IndentHelper indent = new IndentHelper(writer);
		// write decls
		for (EnumsHelper.Enum_t enum_t : prog.mEnums) {
			indent.printf("extern const GeneralReflectionArray<LibCmo::%s::%s> %s;", CommonHelper.getCKPartsNamespace(parts),
					enum_t.mEnumName, enum_t.mEnumName);
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

	public static void writeAccVal(OutputStreamWriter writer, EnumsHelper.Enum_t _enum, CommonHelper.CKParts parts)
			throws Exception {
		EnumsHelper.EnumCollection_t col = new EnumsHelper.EnumCollection_t();
		col.mEnums.add(_enum);
		writeAccVals(writer, col, parts);
	}

}
