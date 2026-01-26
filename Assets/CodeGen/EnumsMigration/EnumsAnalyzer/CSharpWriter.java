import java.io.OutputStreamWriter;

/**
 * Write enum declarations and accessible value into CSharp format.
 */
public class CSharpWriter {

	// =========== C# Enum Declaration Writer ===========

	/**
	 * Get corredponding C# underlying type of given enum.
	 * <p>
	 * This is C# specific function.
	 * 
	 * @param canUnsigned The parameter stored in Enum_t that indicate whether this
	 *                    enum can use unsigned int as its underlying type.
	 * @return The string form of its underlying type.
	 */
	private static String getEnumUnderlyingType(boolean canUnsigned) {
		return canUnsigned ? "uint" : "int";
	}

	/**
	 * Internal real enum declaration writer.
	 * 
	 * @param writer {@linkplain java.io.OutputStreamWriter} instance for writing.
	 * @param prog   {@linkplain EnumsHelper.BEnumCollection} instance for writing.
	 * @throws Exception
	 */
	private static void internalWriteEnums(OutputStreamWriter writer, EnumsHelper.BEnumCollection prog)
			throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.CSharp);
		for (EnumsHelper.BEnum benum : prog.mEnums) {
			// write enum comment
			indent.briefComment(benum.mEnumComment);

			// write enum start
			// write flasg attribute if it is
			if (benum.mUseFlags) {
				indent.puts("[Flags]");
			}
			indent.printf("public enum %s : %s {", benum.mEnumName, getEnumUnderlyingType(benum.mCanUnsigned));
			indent.inc();

			// write enum entries
			for (EnumsHelper.BEnumEntry enumEntry : benum.mEntries) {
				// write entry self
				if (enumEntry.mEntryValue == null) {
					indent.printf("%s,", enumEntry.mEntryName);
				} else {
					indent.printf("%s = %s,", enumEntry.mEntryName, enumEntry.mEntryValue);
				}

				// write entry comment after member
				indent.afterMemberComment(enumEntry.mEntryComment);
			}

			// write enum tail
			indent.dec();
			indent.puts("}");
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

	// =========== C# Enum Accessible Value Writer ===========

	/**
	 * Internal real enum accessible value writer.
	 * 
	 * @param writer {@linkplain java.io.OutputStreamWriter} instance for writing.
	 * @param prog   {@linkplain EnumsHelper.BEnumCollection} instance for writing.
	 * @throws Exception
	 */
	private static void internalWriteAccVals(OutputStreamWriter writer, EnumsHelper.BEnumCollection prog)
			throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.CSharp);
		// write enum collections
		for (EnumsHelper.BEnum benum : prog.mEnums) {
			// write enum desc header
			indent.printf(
					"public static readonly System.Collections.Generic.Dictionary<%s, string> %s = new System.Collections.Generic.Dictionary<%s, string>() {",
					benum.mEnumName, benum.mEnumName, benum.mEnumName);
			indent.inc();

			// write enum desc entries
			for (EnumsHelper.BEnumEntry enumEntry : benum.mEntries) {
				indent.printf("{ %s.%s, \"%s\" },", benum.mEnumName, enumEntry.mEntryName, enumEntry.mEntryName);
			}

			// write enum tail
			indent.dec();
			indent.puts("};");
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
