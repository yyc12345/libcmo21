import java.io.OutputStreamWriter;

/**
 * The accessible values writer of CKERROR
 */
public class ErrorsWriter {
	public static void writeErrors(OutputStreamWriter writer, EnumsHelper.Enum_t errors) throws Exception {
		IndentHelper indent = new IndentHelper(writer);
		indent.puts("#pragma once");
		indent.puts("#include \"CKEnums.hpp\"");
		indent.puts("#include <cstdint>");
		indent.puts("#include <string>");
		indent.puts("#include <vector>");
		indent.puts("namespace Unvirt::AccessibleValue::EnumReflection {");
		indent.inc();

		indent.puts("struct CkErrorReflection { const char* mName; const char* mDescription; };");
		indent.puts("using CkErrorReflectionArray = std::vector<std::pair<LibCmo::CK2::CKERROR, CkErrorReflection>>;");

		indent.puts("namespace CK2 {");
		indent.inc();
		
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
		
		indent.dec();
		indent.puts("}");

		indent.dec();
		indent.puts("}");
	}
}
