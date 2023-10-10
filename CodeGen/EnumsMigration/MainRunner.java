import java.io.OutputStreamWriter;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class MainRunner {

	private static EnumsHelper.EnumCollection_t getEnumsCollection(String infile) throws Exception {
		CommonHelper.InputFilePair pair = CommonHelper.openInputFile(infile);
		CKGeneralLexer lexer = new CKGeneralLexer(pair.mAntlrStream);
		CommonTokenStream tokens = new CommonTokenStream(lexer);
		CKEnumsParser parser = new CKEnumsParser(tokens);

		ParseTree tree = parser.prog();
		ParseTreeWalker walker = new ParseTreeWalker();
		EnumsWalker worker = new EnumsWalker(tokens);
		walker.walk(worker, tree);

		pair.mUnderlyingStream.close();
		return worker.getEnums();
	}

	private static EnumsHelper.Enum_t organiseDefines(String infile, String assignedEnumName) throws Exception {
		CommonHelper.InputFilePair pair = CommonHelper.openInputFile(infile);
		CKGeneralLexer lexer = new CKGeneralLexer(pair.mAntlrStream);
		CommonTokenStream tokens = new CommonTokenStream(lexer);
		CKDefinesParser parser = new CKDefinesParser(tokens);

		ParseTree tree = parser.prog();
		ParseTreeWalker walker = new ParseTreeWalker();
		DefinesWalker worker = new DefinesWalker(tokens);
		walker.walk(worker, tree);

		pair.mUnderlyingStream.close();

		EnumsHelper.Enum_t result = worker.getEnum();
		result.mEnumName = assignedEnumName;
		return result;
	}

	private static EnumsHelper.Enum_t organiseClassid(String infile) throws Exception {
		CommonHelper.InputFilePair pair = CommonHelper.openInputFile(infile);
		CKGeneralLexer lexer = new CKGeneralLexer(pair.mAntlrStream);
		CommonTokenStream tokens = new CommonTokenStream(lexer);
		CKDefinesParser parser = new CKDefinesParser(tokens);

		ParseTree tree = parser.prog();
		ParseTreeWalker walker = new ParseTreeWalker();
		ClassidWalker worker = new ClassidWalker(tokens);
		walker.walk(worker, tree);

		pair.mUnderlyingStream.close();

		EnumsHelper.Enum_t result = worker.getEnum();
		result.mEnumName = "CK_CLASSID";
		return result;
	}

	public static void main(String[] args) throws Exception {

		// =========== CKERROR ===========
		EnumsHelper.Enum_t ckerror = organiseDefines("src/CKError.txt", "CKERROR");
		GeneralWriter.writeEnum("dest/CKError.hpp", ckerror);
		GeneralWriter.writePyEnum("dest/CKError.py", ckerror);
		ErrorsWriter.writeAccVal("dest/CKError.AccVal.hpp", ckerror);
		GeneralWriter.writePyAccVal("dest/CKError.AccVal.py", ckerror);

		// =========== CK_CLASSID ===========
		EnumsHelper.Enum_t classid = organiseClassid("src/CK_CLASSID.txt");
		GeneralWriter.writeEnum("dest/CK_CLASSID.hpp", classid);
		GeneralWriter.writePyEnum("dest/CK_CLASSID.py", classid);
		ClassidWriter.writeAccVal("dest/CK_CLASSID.AccVal.hpp", classid);
		GeneralWriter.writePyAccVal("dest/CK_CLASSID.AccVal.py", classid);

		// =========== Define2 ===========
		// Define2 do not need values.
		EnumsHelper.EnumCollection_t def2 = getEnumsCollection("src/Defines2.txt");
		GeneralWriter.writeEnums("dest/CK_CLASSID.hpp", def2);
		GeneralWriter.writePyEnums("dest/CK_CLASSID.py", def2);

		// =========== Combined enums ===========
		EnumsHelper.EnumCollection_t ck2Enums = getEnumsCollection("src/CKEnums.txt"),
				vxEnums = getEnumsCollection("src/VxEnums.txt");
		GeneralWriter.writeEnums("dest/CKEnums.hpp", ck2Enums);
		GeneralWriter.writePyEnums("dest/CKEnums.py", ck2Enums);
		GeneralWriter.writeAccVals("dest/CKEnums.AccVal.hpp", ck2Enums, CommonHelper.CKParts.CK2);
		GeneralWriter.writePyAccVals("dest/CKEnums.AccVal.py", ck2Enums);
		GeneralWriter.writeEnums("dest/VxEnums.hpp", vxEnums);
		GeneralWriter.writePyEnums("dest/VxEnums.py", vxEnums);
		GeneralWriter.writeAccVals("dest/VxEnums.AccVal.hpp", vxEnums, CommonHelper.CKParts.VxMath);
		GeneralWriter.writePyAccVals("dest/VxEnums.AccVal.py", vxEnums);

		// =========== Single enums ===========
		EnumsHelper.Enum_t single;

		single = organiseDefines("src/CK_STATECHUNK_CHUNKVERSION.txt", "CK_STATECHUNK_CHUNKVERSION");
		GeneralWriter.writeEnum("dest/CK_STATECHUNK_CHUNKVERSION.hpp", single);
		GeneralWriter.writePyEnum("dest/CK_STATECHUNK_CHUNKVERSION.py", single);
		GeneralWriter.writeAccVal("dest/CK_STATECHUNK_CHUNKVERSION.AccVal.hpp", single, CommonHelper.CKParts.CK2);
		GeneralWriter.writePyAccVal("dest/CK_STATECHUNK_CHUNKVERSION.AccVal.py", single);

		single = organiseDefines("src/CK_STATECHUNK_DATAVERSION.txt", "CK_STATECHUNK_DATAVERSION");
		GeneralWriter.writeEnum("dest/CK_STATECHUNK_DATAVERSION.hpp", single);
		GeneralWriter.writePyEnum("dest/CK_STATECHUNK_DATAVERSION.py", single);
		GeneralWriter.writeAccVal("dest/CK_STATECHUNK_DATAVERSION.AccVal.hpp", single, CommonHelper.CKParts.CK2);
		GeneralWriter.writePyAccVal("dest/CK_STATECHUNK_DATAVERSION.AccVal.py", single);

		single = organiseDefines("src/CK_BITMAPDATA_FLAGS.txt", "CK_BITMAPDATA_FLAGS");
		GeneralWriter.writeEnum("dest/CK_BITMAPDATA_FLAGS.hpp", single);
		GeneralWriter.writePyEnum("dest/CK_BITMAPDATA_FLAGS.py", single);
		GeneralWriter.writeAccVal("dest/CK_BITMAPDATA_FLAGS.AccVal.hpp", single, CommonHelper.CKParts.CK2);
		GeneralWriter.writePyAccVal("dest/CK_BITMAPDATA_FLAGS.AccVal.py", single);

		// print message.
		System.out.println("DONE!");
	}
}
