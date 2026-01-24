import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class MainRunner {

	/**
	 * Extract an enums collection from given file.
	 * <p>
	 * This function is the most commonly used function for extracting enums.
	 * <p>
	 * This function is used for a file which only contain enum declarations. This
	 * is not suit for extracting CKERROR and CK_CLASSID. For these declarations,
	 * please use their specialized extractor as described following.
	 * 
	 * @param infile The file for reading.
	 * @return An {@linkplain EnumsHelper.EnumCollection_t} instance.
	 * @throws Exception
	 */
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

	/**
	 * Extract a series of "#define" syntax as an enum.
	 * <p>
	 * This function will assume that given file only contain C++ "#define" syntax.
	 * After reading it, it will re-organize it as an enum and return. This only is
	 * used by CKERROR now. But it suit for more scenarios if there are something
	 * like CKERROR in future.
	 *
	 * @param infile           The file for reading.
	 * @param assignedEnumName The desired name of organized enum instance.
	 *                         Contemporary this field should always be "CKERROR"
	 *                         because no one else is using it.
	 * @return An {@linkplain EnumsHelper.Enum_t} instance.
	 * @throws Exception
	 */
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

	/**
	 * Extract a series of macro define as an enum, considering its indent to build
	 * hierarchy.
	 * <p>
	 * This is specialized enum extractor of CK_CLASSID. The given file should use a
	 * series "#define" syntax to describe enum, and use Tab as the indent before
	 * each "#define" syntax to indicate its hierarchy.
	 * 
	 * @param infile The file for reading.
	 * @return An {@linkplain EnumsHelper.Enum_t} instance. Actually it is an
	 *         instance to {@linkplain EnumsHelper.Enum_t} whose entries is
	 *         {@linkplain EnumsHelper.EnumEntryWithHierarchy_t}, the child class of
	 *         {@linkplain EnumsHelper.EnumEntry_t} (the entry type of common
	 *         {@linkplain EnumsHelper.Enum_t}) with extra hierarchy infos.
	 * @throws Exception
	 */
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
		EnumsHelper.Enum_t ckerror = organiseDefines("src/CKERROR.txt", "CKERROR");
		CppWriter.writeEnum("dest/CKERROR.hpp", ckerror);
		PythonWriter.writeEnum("dest/CKERROR.py", ckerror);	
		CSharpWriter.writeEnum("dest/CKERROR.cs", ckerror);
		CppWriter.writeCkErrorAccVal("dest/CKERROR.AccVal.hpp", ckerror);
		PythonWriter.writeAccVal("dest/CKERROR.AccVal.py", ckerror);
		CSharpWriter.writeAccVal("dest/CKERROR.AccVal.cs", ckerror);

		// =========== CK_CLASSID ===========
		EnumsHelper.Enum_t classid = organiseClassid("src/CK_CLASSID.txt");
		CppWriter.writeEnum("dest/CK_CLASSID.hpp", classid);
		PythonWriter.writeEnum("dest/CK_CLASSID.py", classid);
		CSharpWriter.writeEnum("dest/CK_CLASSID.cs", classid);
		CppWriter.writeCkClassidAccVal("dest/CK_CLASSID.AccVal.hpp", classid);
		PythonWriter.writeAccVal("dest/CK_CLASSID.AccVal.py", classid);

		// =========== Define2 ===========
		// Define2 do not need annotation output.
		// Because they are CKStateChunk used value which are not exposed to outside.
		EnumsHelper.EnumCollection_t def2 = getEnumsCollection("src/Defines2.txt");
		CppWriter.writeEnums("dest/Defines2.hpp", def2);
		PythonWriter.writeEnums("dest/Defines2.py", def2);
		CSharpWriter.writeEnums("dest/Defines2.cs", def2);

		// =========== Combined enums ===========
		EnumsHelper.EnumCollection_t ck2Enums = getEnumsCollection("src/CKEnums.txt"),
				vxEnums = getEnumsCollection("src/VxEnums.txt");
		
		CppWriter.writeEnums("dest/CKEnums.hpp", ck2Enums);
		PythonWriter.writeEnums("dest/CKEnums.py", ck2Enums);
		CSharpWriter.writeEnums("dest/CKEnums.cs", ck2Enums);
		CppWriter.writeAccVals("dest/CKEnums.AccVal.hpp", ck2Enums, CommonHelper.CKParts.CK2);
		PythonWriter.writeAccVals("dest/CKEnums.AccVal.py", ck2Enums);
		CSharpWriter.writeAccVals("dest/CKEnums.AccVal.cs", ck2Enums);
		
		CppWriter.writeEnums("dest/VxEnums.hpp", vxEnums);
		PythonWriter.writeEnums("dest/VxEnums.py", vxEnums);
		CSharpWriter.writeEnums("dest/VxEnums.cs", vxEnums);
		CppWriter.writeAccVals("dest/VxEnums.AccVal.hpp", vxEnums, CommonHelper.CKParts.VxMath);
		PythonWriter.writeAccVals("dest/VxEnums.AccVal.py", vxEnums);
		CSharpWriter.writeAccVals("dest/VxEnums.AccVal.cs", vxEnums);

		// =========== Single enums ===========
		EnumsHelper.Enum_t single;

		single = organiseDefines("src/CK_STATECHUNK_CHUNKVERSION.txt", "CK_STATECHUNK_CHUNKVERSION");
		CppWriter.writeEnum("dest/CK_STATECHUNK_CHUNKVERSION.hpp", single);
		PythonWriter.writeEnum("dest/CK_STATECHUNK_CHUNKVERSION.py", single);
		CSharpWriter.writeEnum("dest/CK_STATECHUNK_CHUNKVERSION.cs", single);
		CppWriter.writeAccVal("dest/CK_STATECHUNK_CHUNKVERSION.AccVal.hpp", single, CommonHelper.CKParts.CK2);
		PythonWriter.writeAccVal("dest/CK_STATECHUNK_CHUNKVERSION.AccVal.py", single);
		CSharpWriter.writeAccVal("dest/CK_STATECHUNK_CHUNKVERSION.AccVal.cs", single);

		single = organiseDefines("src/CK_STATECHUNK_DATAVERSION.txt", "CK_STATECHUNK_DATAVERSION");
		CppWriter.writeEnum("dest/CK_STATECHUNK_DATAVERSION.hpp", single);
		PythonWriter.writeEnum("dest/CK_STATECHUNK_DATAVERSION.py", single);
		CSharpWriter.writeEnum("dest/CK_STATECHUNK_DATAVERSION.cs", single);
		CppWriter.writeAccVal("dest/CK_STATECHUNK_DATAVERSION.AccVal.hpp", single, CommonHelper.CKParts.CK2);
		PythonWriter.writeAccVal("dest/CK_STATECHUNK_DATAVERSION.AccVal.py", single);
		CSharpWriter.writeAccVal("dest/CK_STATECHUNK_DATAVERSION.AccVal.cs", single);

		single = organiseDefines("src/CK_BITMAPDATA_FLAGS.txt", "CK_BITMAPDATA_FLAGS");
		CppWriter.writeEnum("dest/CK_BITMAPDATA_FLAGS.hpp", single);
		PythonWriter.writeEnum("dest/CK_BITMAPDATA_FLAGS.py", single);
		CSharpWriter.writeEnum("dest/CK_BITMAPDATA_FLAGS.cs", single);
		CppWriter.writeAccVal("dest/CK_BITMAPDATA_FLAGS.AccVal.hpp", single, CommonHelper.CKParts.CK2);
		PythonWriter.writeAccVal("dest/CK_BITMAPDATA_FLAGS.AccVal.py", single);
		CSharpWriter.writeAccVal("dest/CK_BITMAPDATA_FLAGS.AccVal.cs", single);

		single = organiseDefines("src/CK_CAMERA_PROJECTION.txt", "CK_CAMERA_PROJECTION");
		CppWriter.writeEnum("dest/CK_CAMERA_PROJECTION.hpp", single);
		PythonWriter.writeEnum("dest/CK_CAMERA_PROJECTION.py", single);
		CSharpWriter.writeEnum("dest/CK_CAMERA_PROJECTION.cs", single);
		CppWriter.writeAccVal("dest/CK_CAMERA_PROJECTION.AccVal.hpp", single, CommonHelper.CKParts.CK2);
		PythonWriter.writeAccVal("dest/CK_CAMERA_PROJECTION.AccVal.py", single);
		CSharpWriter.writeAccVal("dest/CK_CAMERA_PROJECTION.AccVal.cs", single);

		// print message.
		System.out.println("DONE!");
	}
}
