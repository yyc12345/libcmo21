import java.io.OutputStreamWriter;
import java.util.stream.Collectors;

/**
 * The nameof values writer for CK_CLASSID.
 */
public class ClassidWriter {
	public static void writeNameofClassid(OutputStreamWriter writer, EnumsHelper.Enum_t classids) throws Exception {
		IndentHelper indent = new IndentHelper(writer);
		indent.puts("struct CkClassidReflection { std::vector<const char*> mHierarchy; };");
		indent.puts(
				"using CkClassidReflectionArray = std::vector<std::pair<LibCmo::CK2::CK_CLASSID, CkClassidReflection>>;");
		indent.puts("");

		indent.puts("const CkClassidReflectionArray CK_CLASSID {");
		indent.inc();
		for (EnumsHelper.EnumEntry_t entry : classids.mEntries) {
			EnumsHelper.EnumEntryWithHierarchy_t specialized = (EnumsHelper.EnumEntryWithHierarchy_t) entry;

			String hierarchy = specialized.mHierarchy.stream().map(value -> value.mEntryName)
					.collect(Collectors.joining("\", \""));
			indent.printf("{ LibCmo::CK2::CK_CLASSID::%s, { { \"%s\" } } },", entry.mEntryName, hierarchy);
		}
		indent.dec();
		indent.puts("};");

	}
}
