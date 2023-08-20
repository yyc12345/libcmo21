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
		// read ck2 and vxmath enum first.
		// these files are originally is defined as enum
		EnumsHelper.EnumCollection_t ck2Enums = getEnumsCollection("src/CKENUMS.txt"),
				vxmathEnums = getEnumsCollection("src/VXENUMS.txt");
		OutputStreamWriter fs = null;
		// read define series and reorgainse it as a enum
		// create independent classid and ckerror for specialized processing.
		EnumsHelper.Enum_t classid = organiseClassid("src/CK_CLASSID.txt", "CK_CLASSID"),
				ckerror = organiseDefines("src/CKERROR.txt", "CKERROR");
		ck2Enums.mEnums.add(organiseDefines("src/CK_STATECHUNK_CHUNKVERSION.txt", "CK_STATECHUNK_CHUNKVERSION"));
		ck2Enums.mEnums.add(organiseDefines("src/CK_STATECHUNK_DATAVERSION.txt", "CK_STATECHUNK_DATAVERSION"));

		// write accessible value, except classid and ckerror.
		fs = CommonHelper.openOutputFile("../Unvirt/Generated/Enums.gen.hpp");
		EnumsWriter.writeAccessibleValues(fs, ck2Enums, vxmathEnums);
		fs.close();
		// class id and ckerror need special writer.
		fs = CommonHelper.openOutputFile("../Unvirt/Generated/CkError.gen.hpp");
		ErrorsWriter.writeErrors(fs, ckerror);
		fs.close();
		fs = CommonHelper.openOutputFile("../Unvirt/Generated/CkClassid.gen.hpp");
		ClassidWriter.writeClassids(fs, classid);
		fs.close();

		// re-add classid and ckerror enum data.
		// write ck2 and vxmath enum into file
		ck2Enums.mEnums.add(ckerror);
		ck2Enums.mEnums.add(classid);
		fs = CommonHelper.openOutputFile("../LibCmo/Generated/Enums.gen.hpp");
		EnumsWriter.writeEnums(fs, ck2Enums, vxmathEnums);
		fs.close();

		// print message.
		System.out.println("DONE!");
	}
}
