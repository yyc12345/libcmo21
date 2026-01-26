import java.io.OutputStreamWriter;
import java.util.Locale;

/**
 * Write enum declarations and accessible value into Python format.
 */
public class PythonWriter {

	// =========== Python Enum Declaration Writer ===========

	/**
	 * Internal real enum declaration writer.
	 * 
	 * @param writer {@linkplain java.io.OutputStreamWriter} instance for writing.
	 * @param prog   {@linkplain EnumsHelper.BEnumCollection} instance for writing.
	 * @throws Exception
	 */
	private static void internalWriteEnums(OutputStreamWriter writer, EnumsHelper.BEnumCollection prog)
			throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Python);
		for (EnumsHelper.BEnum benum : prog.mEnums) {
			// write enum start
			indent.printf("class %s(enum.IntEnum):", benum.mEnumName);
			indent.inc();

			// write enum comment
			indent.briefComment(benum.mEnumComment);

			// write enum entries
			for (EnumsHelper.BEnumEntry enumEntry : benum.mEntries) {
				// write entry self
				if (enumEntry.mEntryValue == null) {
					indent.printf("%s = auto()", enumEntry.mEntryName);
				} else {
					indent.printf("%s = %s", enumEntry.mEntryName,
							CommonHelper.convertToPythonNumber(enumEntry.mEntryValue));
				}

				// write entry comment after member
				indent.afterMemberComment(enumEntry.mEntryComment);
			}

			// enum tail
			indent.dec();
		}
	}

	/**
	 * Write an enum declaration collection into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum declaration collection writer.
	 * 
	 * @param filename The name of written file.
	 * @param prog     {@linkplain EnumsHelper.BEnumCollection} instance for
	 *                 writing.
	 * @throws Exception
	 */
	public static void writeEnums(String filename, EnumsHelper.BEnumCollection prog) throws Exception {
		// open file and write
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		internalWriteEnums(fs, prog);
		fs.close();
	}

	/**
	 * Write a single enum declaration into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum declaration collection writer.
	 * 
	 * @param filename The name of written file.
	 * @param _enum    {@linkplain EnumsHelper.BEnum} instance for writing.
	 * @throws Exception
	 */
	public static void writeEnum(String filename, EnumsHelper.BEnum _enum) throws Exception {
		// create collection from single enum
		EnumsHelper.BEnumCollection col = new EnumsHelper.BEnumCollection();
		col.mEnums.add(_enum);
		// open file and write
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		internalWriteEnums(fs, col);
		fs.close();
	}

	// =========== Python Enum Accessible Value Writer ===========

	/**
	 * Try generate human readable name from enum entry name.
	 * <p>
	 * This function is only served for Python code generation.
	 * <p>
	 * As you noticed, almost entries of CK enums are fully capital and splitted by
	 * underline. This is really not good for human reading, especially those who
	 * are not programmer. So this function will try give these programmer-oriented
	 * entry name a human readable name as its display name. However, this extract
	 * method is not perfect. It simply do some split and replacement so the
	 * generated content may still not good for reader.
	 * 
	 * @param entry_name The name of enum entry
	 * @return A human readable entry name. No guaranteen that return value is must
	 *         human readable.
	 */
	private static String extractHumanReadableEntryName(String entry_name) {
		// remove first part (any content before underline '_')
		entry_name = entry_name.replaceFirst("^[a-zA-Z0-9]+_", "");

		// lower all chars except first char
		if (entry_name.length() < 1)
			return entry_name;
		else
			return entry_name.substring(0, 1) + entry_name.substring(1).toLowerCase(Locale.ROOT);
	}

	/**
	 * Internal real enum accessible value writer.
	 * 
	 * @param writer {@linkplain java.io.OutputStreamWriter} instance for writing.
	 * @param prog   {@linkplain EnumsHelper.BEnumCollection} instance for writing.
	 * @throws Exception
	 */
	private static void internalWriteAccVals(OutputStreamWriter writer, EnumsHelper.BEnumCollection prog)
			throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Python);

		// write implements
		for (EnumsHelper.BEnum benum : prog.mEnums) {
			// write enum desc header
			indent.printf("g_Annotation_%s: dict[int, EnumAnnotation] = {", benum.mEnumName);
			indent.inc();

			// write enum desc entries
			for (EnumsHelper.BEnumEntry enumEntry : benum.mEntries) {
				String comment = "";
				if (enumEntry.mEntryComment != null) {
					comment = CommonHelper.escapeString(enumEntry.mEntryComment);
				}

				indent.printf("%s.%s.value: EnumAnnotation(\"%s\", \"%s\"),", benum.mEnumName, enumEntry.mEntryName,
						extractHumanReadableEntryName(enumEntry.mEntryName), comment);
			}

			// write enum tail
			indent.dec();
			indent.puts("}");
		}
	}

	/**
	 * Write an enum accessible value collection into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum accessible value collection
	 * writer.
	 * 
	 * @param filename The name of written file.
	 * @param prog     {@linkplain EnumsHelper.BEnumCollection} instance for
	 *                 writing.
	 * @throws Exception
	 */
	public static void writeAccVals(String filename, EnumsHelper.BEnumCollection prog) throws Exception {
		// open file and write
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		internalWriteAccVals(fs, prog);
		fs.close();
	}

	/**
	 * Write a single enum accessible value into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum accessible value collection
	 * writer.
	 * 
	 * @param filename The name of written file.
	 * @param _enum    {@linkplain EnumsHelper.BEnum} instance for writing.
	 * @throws Exception
	 */
	public static void writeAccVal(String filename, EnumsHelper.BEnum _enum) throws Exception {
		// create a collection with single enum.
		EnumsHelper.BEnumCollection col = new EnumsHelper.BEnumCollection();
		col.mEnums.add(_enum);
		// open file and write
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		internalWriteAccVals(fs, col);
		fs.close();
	}

}
