import java.io.OutputStreamWriter;

/**
 * Generic Enum Writer
 */
public class EnumsWriter {
	private static void writeEnum(IndentHelper indent, EnumsHelper.EnumCollection_t prog) throws Exception {
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

	public static void writeEnums(OutputStreamWriter writer, EnumsHelper.EnumCollection_t ck2_prog,
			EnumsHelper.EnumCollection_t vxmath_prog) throws Exception {
		IndentHelper indent = new IndentHelper(writer);
		indent.puts("#pragma once");
		indent.puts("#include <cstdint>");

		indent.puts("namespace LibCmo::CK2 {");
		indent.inc();
		writeEnum(indent, ck2_prog);
		indent.dec();
		indent.puts("}");

		indent.puts("namespace LibCmo::VxMath {");
		indent.inc();
		writeEnum(indent, vxmath_prog);
		indent.dec();
		indent.puts("}");
	}

	private static void writeAccessibleValue(IndentHelper indent, CommonHelper.CKParts parts,
			EnumsHelper.EnumCollection_t prog) throws Exception {
		for (EnumsHelper.Enum_t enum_t : prog.mEnums) {
			// write enum desc header
			indent.printf("const EnumNameofArray<LibCmo::%s::%s> {} {", CommonHelper.getCKPartsNamespace(parts),
					enum_t.mEnumName, enum_t.mEnumName);
			indent.inc();

			// write enum desc entries
			for (EnumsHelper.EnumEntry_t enumEntry_t : enum_t.mEntries) {
				indent.printf("{ LibCmo::%s::%s::%s, \"%s\" },", parts, enum_t.mEnumName, enumEntry_t.mEntryName,
						enumEntry_t.mEntryName);
			}

			// write enum tail
			indent.dec();
			indent.puts("};");
		}
	}

	public static void writeAccessibleValues(OutputStreamWriter writer, EnumsHelper.EnumCollection_t ck2_prog,
			EnumsHelper.EnumCollection_t vxmath_prog) throws Exception {
		IndentHelper indent = new IndentHelper(writer);
		indent.puts("#pragma once");
		indent.puts("#include \"CKEnums.hpp\"");
		indent.puts("#include <cstdint>");
		indent.puts("#include <string>");
		indent.puts("#include <vector>");
		indent.puts("namespace Unvirt::AccessibleValue::EnumDesc {");
		indent.inc();

		indent.puts("namespace CK2 {");
		indent.inc();
		writeAccessibleValue(indent, CommonHelper.CKParts.CK2, ck2_prog);
		indent.dec();
		indent.puts("}");

		indent.puts("namespace VxMath {");
		indent.inc();
		writeAccessibleValue(indent, CommonHelper.CKParts.VxMath, vxmath_prog);
		indent.dec();
		indent.puts("}");

		indent.dec();
		indent.puts("}");
	}
}
