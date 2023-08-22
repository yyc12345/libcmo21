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

	private static EnumsHelper.Enum_t organiseClassid(String infile, String assignedEnumName) throws Exception {
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
		result.mEnumName = assignedEnumName;
		return result;
	}

	public static void main(String[] args) throws Exception {
		OutputStreamWriter fs = null;
		
		// =========== Read Data ===========
		// read enum series first.
		// these file are originally is enum in Virtools SDK
		EnumsHelper.EnumCollection_t ck2Enums = getEnumsCollection("src/CKEnums.txt"),
				vxEnums = getEnumsCollection("src/VxEnums.txt"),
				def2 = getEnumsCollection("src/Defines2.txt");
		
		// read and reorganize #define type enum
		
		// read CKERROR standalone. because we need treat it specialized.
		EnumsHelper.Enum_t ckerror = organiseDefines("src/CKError.txt", "CKERROR");
		// read CK_CLASSID standalone in another way.
		EnumsHelper.Enum_t classid = organiseClassid("src/Classid.txt", "CK_CLASSID");
		// these enum can be directly merge into previous read enum collection
		// because we do not need process it independently.
		ck2Enums.mEnums.add(organiseDefines("src/CK_STATECHUNK_CHUNKVERSION.txt", "CK_STATECHUNK_CHUNKVERSION"));
		ck2Enums.mEnums.add(organiseDefines("src/CK_STATECHUNK_DATAVERSION.txt", "CK_STATECHUNK_DATAVERSION"));
		
		// =========== Write Nameof Values ===========
		// write general accessible values
		fs = CommonHelper.openOutputFile("dest/CKEnums.NameofValue.hpp");
		GeneralWriter.writeNameofValues(fs, ck2Enums, CommonHelper.CKParts.CK2);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/VxEnums.NameofValue.hpp");
		GeneralWriter.writeNameofValues(fs, vxEnums, CommonHelper.CKParts.VxMath);
		fs.close();
		// write CKERROR
		fs = CommonHelper.openOutputFile("dest/CKError.NameofValue.hpp");
		ErrorsWriter.writeNameofError(fs, ckerror);
		fs.close();
		// write CK_CLASSID
		fs = CommonHelper.openOutputFile("dest/CKClassid.NameofValue.hpp");
		ClassidWriter.writeNameofClassid(fs, classid);
		fs.close();
		
		// write data type defines
		// CKERROR and CK_CLASSID should be grouped into ck2Enums
		ck2Enums.mEnums.add(ckerror);
		ck2Enums.mEnums.add(classid);
		fs = CommonHelper.openOutputFile("dest/CKEnums.hpp");
		GeneralWriter.writeEnums(fs, ck2Enums);
		fs.close();
		fs = CommonHelper.openOutputFile("dest/VxEnums.hpp");
		GeneralWriter.writeEnums(fs, vxEnums);
		fs.close();
		// Defines2.h need independentlt write
		fs = CommonHelper.openOutputFile("dest/Def2.hpp");
		GeneralWriter.writeEnums(fs, def2);
		fs.close();
		
		// print message.
		System.out.println("DONE!");
	}
}
