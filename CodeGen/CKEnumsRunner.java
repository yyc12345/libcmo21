import java.io.OutputStreamWriter;
import java.util.List;
import java.util.Vector;
import java.util.stream.Collectors;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class CKEnumsRunner {
	
	private static List<Enum_t> getProg(String infile) throws Exception {
		CKCommonHelper.InputFilePair pair = CKCommonHelper.openInputFile(infile);
		CKGeneralLexer lexer = new CKGeneralLexer(pair.mAntlrStream);
		CommonTokenStream tokens = new CommonTokenStream(lexer);
		CKEnumParser parser = new CKEnumParser(tokens);
		
		ParseTree tree = parser.prog();
		ParseTreeWalker walker = new ParseTreeWalker();
		EnumWalker worker = new EnumWalker(tokens);
		walker.walk(worker, tree);
		
		pair.mUnderlyingStream.close();
		return worker.getResult();
	}
	public static void run(String inCk2Enums, String inVxEnums, String outEnums, String outAccessibleValues) throws Exception {
		List<Enum_t> ck2prog = getProg(inCk2Enums);
		List<Enum_t> vxprog = getProg(inVxEnums);
		
		OutputStreamWriter fs = CKCommonHelper.openOutputFile(outEnums);
		EnumWriter.writeEnums(fs, ck2prog, vxprog);
		fs.close();
		
		fs = CKCommonHelper.openOutputFile(outAccessibleValues);
		EnumWriter.writeAccessibleValues(fs, ck2prog, vxprog);
		fs.close();
	}
}
