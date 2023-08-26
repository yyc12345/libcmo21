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
		OutputStreamWriter fs = null;
		
		// =========== CKERROR ===========
		EnumsHelper.Enum_t ckerror = organiseDefines("src/CKError.txt", "CKERROR");
		fs = CommonHelper.openOutputFile("dest/CKError.hpp");
		GeneralWriter.writeEnum(fs, ckerror);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/CKError.AccVal.hpp");
		ErrorsWriter.writeAccVals(fs, ckerror);
		fs.close();
		
		// =========== CK_CLASSID ===========
		EnumsHelper.Enum_t classid = organiseClassid("src/CK_CLASSID.txt");
		fs = CommonHelper.openOutputFile("dest/CK_CLASSID.hpp");
		GeneralWriter.writeEnum(fs, classid);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/CK_CLASSID.AccVal.hpp");
		ClassidWriter.writeAccVals(fs, classid);
		fs.close();
		
		// =========== Define2 ===========
		// Define2 do not need values.
		EnumsHelper.EnumCollection_t def2 = getEnumsCollection("src/Defines2.txt");
		fs = CommonHelper.openOutputFile("dest/CK_CLASSID.hpp");
		GeneralWriter.writeEnums(fs, def2);
		fs.close();
		
		// =========== Combined enums ===========
		EnumsHelper.EnumCollection_t ck2Enums = getEnumsCollection("src/CKEnums.txt"),
				vxEnums = getEnumsCollection("src/VxEnums.txt");
		fs = CommonHelper.openOutputFile("dest/CKEnums.hpp");
		GeneralWriter.writeEnums(fs, ck2Enums);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/CKEnums.AccVal.hpp");
		GeneralWriter.writeAccVals(fs, ck2Enums, CommonHelper.CKParts.CK2);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/VxEnums.hpp");
		GeneralWriter.writeEnums(fs, vxEnums);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/VxEnums.AccVal.hpp");
		GeneralWriter.writeAccVals(fs, vxEnums, CommonHelper.CKParts.VxMath);
		fs.close();
		
		// =========== Single enums ===========
		EnumsHelper.Enum_t single;
		
		single = organiseDefines("src/CK_STATECHUNK_CHUNKVERSION.txt", "CK_STATECHUNK_CHUNKVERSION");
		fs = CommonHelper.openOutputFile("dest/CK_STATECHUNK_CHUNKVERSION.hpp");
		GeneralWriter.writeEnum(fs, single);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/CK_STATECHUNK_CHUNKVERSION.AccVal.hpp");
		GeneralWriter.writeAccVal(fs, single, CommonHelper.CKParts.CK2);
		fs.close();

		single = organiseDefines("src/CK_STATECHUNK_DATAVERSION.txt", "CK_STATECHUNK_DATAVERSION");
		fs = CommonHelper.openOutputFile("dest/CK_STATECHUNK_DATAVERSION.hpp");
		GeneralWriter.writeEnum(fs, single);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/CK_STATECHUNK_DATAVERSION.AccVal.hpp");
		GeneralWriter.writeAccVal(fs, single, CommonHelper.CKParts.CK2);
		fs.close();
		
		// print message.
		System.out.println("DONE!");
	}
}
