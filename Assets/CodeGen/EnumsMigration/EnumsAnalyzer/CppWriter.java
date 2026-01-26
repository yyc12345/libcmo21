import java.io.OutputStreamWriter;
import java.util.stream.Collectors;

/**
 * Write enum declarations and accessible value into C++ format.
 */
public class CppWriter {

	// =========== C++ Enum Declaration Writer ===========

	/**
	 * Get corredponding C++ underlying type of given enum.
	 * <p>
	 * This is C++ specific function.
	 * 
	 * @param canUnsigned The parameter stored in Enum_t that indicate whether this
	 *                    enum can use unsigned int as its underlying type.
	 * @return The string form of its underlying type.
	 */
	private static String getEnumUnderlyingType(boolean canUnsigned) {
		return canUnsigned ? "CKDWORD" : "CKINT";
	}

	/**
	 * Internal real enum declarations writer.
	 * 
	 * @param writer {@linkplain java.io.OutputStreamWriter} instance for writing.
	 * @param prog   {@linkplain EnumsHelper.BEnumCollection} instance for writing.
	 * @throws Exception
	 */
	private static void internalWriteEnums(OutputStreamWriter writer, EnumsHelper.BEnumCollection prog)
			throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Cpp);
		for (EnumsHelper.BEnum benum : prog.mEnums) {
			// write enum comment
			indent.briefComment(benum.mEnumComment);

			// write enum start
			indent.printf("enum class %s : %s {", benum.mEnumName, getEnumUnderlyingType(benum.mCanUnsigned));
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
			indent.puts("};");
		}
	}

	/**
	 * Write an enum collection into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum collection writer.
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
	 * Write a single enum into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum collection writer.
	 * 
	 * @param filename The name of written file.
	 * @param _enum    {@linkplain EnumsHelper.BEnum} instance for writing.
	 * @throws Exception
	 */
	public static void writeEnum(String filename, EnumsHelper.BEnum _enum) throws Exception {
		// create an collection from single enum declaration
		// for suit the argument requirement of real writer.
		EnumsHelper.BEnumCollection col = new EnumsHelper.BEnumCollection();
		col.mEnums.add(_enum);
		// open file and write
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		internalWriteEnums(fs, col);
		fs.close();
	}

	// =========== C++ Enum Accessible Value Writer ===========

	/**
	 * Internal real enum collection accessible value writer.
	 * 
	 * @param writer {@linkplain java.io.OutputStreamWriter} instance for writing.
	 * @param prog   {@linkplain EnumsHelper.BEnumCollection} instance for writing.
	 * @param parts  The part of these enum declarations. It will indicate the
	 *               namespace where find given enum collection.
	 * @throws Exception
	 */
	private static void internalWriteAccVals(OutputStreamWriter writer, EnumsHelper.BEnumCollection prog,
			CommonHelper.CKParts parts) throws Exception {
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Cpp);
		
		// write type defination (just to let user know what the type is)
		indent.puts("// struct GeneralReflection { const char8_t* mName; };");
		indent.puts("// template<typename _Ty, std::enable_if_t<std::is_enum_v<_Ty>, int> = 0>");
		indent.puts("// using GeneralReflectionArray = std::vector<std::pair<_Ty, GeneralReflection>>;");
		
		indent.puts("");
		indent.puts("");
		indent.puts("");

		// write declarations
		for (EnumsHelper.BEnum benum : prog.mEnums) {
			indent.printf("extern const GeneralReflectionArray<LibCmo::%s::%s> %s;",
					CommonHelper.getCKPartsNamespace(parts), benum.mEnumName, benum.mEnumName);
		}

		indent.puts("");
		indent.puts("");
		indent.puts("");

		// write implements
		for (EnumsHelper.BEnum benum : prog.mEnums) {
			// write enum desc header
			indent.printf("const GeneralReflectionArray<LibCmo::%s::%s> %s {", CommonHelper.getCKPartsNamespace(parts),
					benum.mEnumName, benum.mEnumName);
			indent.inc();

			// write enum desc entries
			for (EnumsHelper.BEnumEntry enumEntry : benum.mEntries) {
				indent.printf("{ LibCmo::%s::%s::%s, { u8\"%s\" } },", CommonHelper.getCKPartsNamespace(parts),
						benum.mEnumName, enumEntry.mEntryName, enumEntry.mEntryName);
			}

			// write enum tail
			indent.dec();
			indent.puts("};");
		}
	}

	/**
	 * Write an enum collection accessible value into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum collection accessible value
	 * writer.
	 * 
	 * @param filename The name of written file.
	 * @param prog     {@linkplain EnumsHelper.BEnumCollection} instance for
	 *                 writing.
	 * @param parts    The part of these enum declarations.
	 * @throws Exception
	 */
	public static void writeAccVals(String filename, EnumsHelper.BEnumCollection prog, CommonHelper.CKParts parts)
			throws Exception {
		// open file and write
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		internalWriteAccVals(fs, prog, parts);
		fs.close();
	}

	/**
	 * Write a single enum accessible value into given file.
	 * <p>
	 * Actually this is a wrapper of internal enum collection accessible value
	 * writer.
	 * 
	 * @param filename The name of written file.
	 * @param _enum    {@linkplain EnumsHelper.BEnum} instance for writing.
	 * @param parts    The part of these enum declarations.
	 * @throws Exception
	 */
	public static void writeAccVal(String filename, EnumsHelper.BEnum _enum, CommonHelper.CKParts parts)
			throws Exception {
		// create a enum collection to fulfill the requirement of internal writer.
		EnumsHelper.BEnumCollection col = new EnumsHelper.BEnumCollection();
		col.mEnums.add(_enum);
		// open file and write
		OutputStreamWriter fs = CommonHelper.openOutputFile(filename);
		internalWriteAccVals(fs, col, parts);
		fs.close();
	}

	// =========== Specialized C++ Enum Accessible Value Writer ===========
	// Only accessible value part of CERROR and CK_CLASSID need to be specialized.
	// The enum self do not need special treat. Just write them normally.

	/**
	 * Specialized CKERROR accessible value writer.
	 * <p>
	 * The declaration of CKERROR do not need special treat. It is okey to use
	 * common writer.
	 * 
	 * @param filename The name of written file.
	 * @param errors   The {@linkplain EnumsHelper.BEnum} instance storing CKERROR.
	 * @throws Exception
	 */
	public static void writeCkErrorAccVal(String filename, EnumsHelper.BEnum errors) throws Exception {
		OutputStreamWriter writer = CommonHelper.openOutputFile(filename);
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Cpp);

		// write type defination (just to let user know what the type is)
		indent.puts("// struct CkErrorReflection { const char8_t* mName; const char8_t* mDescription; };");
		indent.puts("// using CkErrorReflectionArray = std::vector<std::pair<LibCmo::CK2::CKERROR, CkErrorReflection>>;");
		
		indent.puts("");
		indent.puts("");
		indent.puts("");

		// write implementation
		indent.puts("const CkErrorReflectionArray CKERROR {");
		indent.inc();
		for (EnumsHelper.BEnumEntry entry : errors.mEntries) {
			String comment = CommonHelper.escapeString(entry.mEntryComment);
			if (comment == null)
				comment = "";

			indent.printf("{ LibCmo::CK2::CKERROR::%s, { u8\"%s\", u8\"%s\" } },", entry.mEntryName, entry.mEntryName,
					comment);
		}
		indent.dec();
		indent.puts("};");

		writer.close();
	}

	/**
	 * Specialized CK_CLASSID accessible value writer.
	 * <p>
	 * The declaration of CK_CLASSID do not need special treat. It is okey to use
	 * common writer.
	 * 
	 * @param filename The name of written file.
	 * @param classids The {@linkplain EnumsHelper.BEnum} instance storing
	 *                 CK_CLASSID.
	 * @throws Exception
	 */
	public static void writeCkClassidAccVal(String filename, EnumsHelper.BEnum classids) throws Exception {
		OutputStreamWriter writer = CommonHelper.openOutputFile(filename);
		IndentHelper indent = new IndentHelper(writer, CommonHelper.LangType.Cpp);

		// write type defination (just to let user know what the type is)
		indent.puts("// struct CkClassidReflection { std::vector<const char8_t*> mHierarchy; };");
		indent.puts("// using CkClassidReflectionArray = std::vector<std::pair<LibCmo::CK2::CK_CLASSID, CkClassidReflection>>;");
		
		indent.puts("");
		indent.puts("");
		indent.puts("");

		indent.puts("const CkClassidReflectionArray CK_CLASSID {");
		indent.inc();
		for (EnumsHelper.BEnumEntry entry : classids.mEntries) {
			EnumsHelper.BHierarchyEnumEntry specialized = (EnumsHelper.BHierarchyEnumEntry) entry;

			String hierarchy = specialized.mHierarchy.stream().map(value -> value.mEntryName)
					.collect(Collectors.joining("\", u8\""));
			indent.printf("{ LibCmo::CK2::CK_CLASSID::%s, { { u8\"%s\" } } },", entry.mEntryName, hierarchy);
		}
		indent.dec();
		indent.puts("};");

		writer.close();
	}
}
