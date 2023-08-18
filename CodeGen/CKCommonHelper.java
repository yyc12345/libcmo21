import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.util.List;

import org.antlr.v4.runtime.*;

public class CKCommonHelper {

	public static Token getPreChannelToken(BufferedTokenStream stream, Token token, int channel) {
		List<Token> tokens = stream.getHiddenTokensToLeft(token.getTokenIndex(), channel);
		if (tokens == null)
			return null;
		return tokens.get(0);
	}

	public static Token getPostChannelToken(BufferedTokenStream stream, Token token, int channel) {
		List<Token> tokens = stream.getHiddenTokensToRight(token.getTokenIndex(), channel);
		if (tokens == null)
			return null;
		return tokens.get(0);
	}

	public static String cutComment(Token comment) {
		if (comment == null)
			return null;

		switch (comment.getType()) {
		case CKGeneralLexer.CKGENERAL_LINE_COMMENT: {
			return comment.getText().substring(2);
		}
		case CKGeneralLexer.CKGENERAL_BLOCK_COMMENT: {
			String cache = comment.getText();
			return cache.substring(2, cache.length() - 4);
		}
		default:
			return comment.getText();
		}
	}

	public static boolean isNegtiveNumber(String numstr) {
		return numstr.startsWith("-");
	}

	enum CKParts {
		CK2, VxMath
	}

	public static String getCKPartsNamespace(CKParts parts) {
		switch (parts) {
		case CK2:
			return "CK2";
		case VxMath:
			return "VxMath";
		default:
			throw new IllegalArgumentException("Unexpected value: " + parts);
		}
	}

	/**
	 * Get underlying type of enum.
	 * @param canUnsigned The parameter stored in Enum_t that indiccate 
	 * whether this enum can use unsigned int as its underlying type.
	 * @return The string form of its underlying type.
	 */
	public static String getEnumUnderlyingType(boolean canUnsigned) {
		return canUnsigned ? "uint32_t" : "int32_t";
	}

	// =========== File Operations ===========
	
	public static class InputFilePair {
		public CharStream mAntlrStream;
		public FileInputStream mUnderlyingStream;
	}
	public static InputFilePair openInputFile(String filename) throws Exception {
		InputFilePair pair = new InputFilePair();
		pair.mUnderlyingStream = new FileInputStream(filename);
		pair.mAntlrStream = CharStreams.fromStream(pair.mUnderlyingStream, StandardCharsets.UTF_8);
		return pair;
	}
	
	/**
	 * Get output file for writing.
	 * @param filename The name of file opening.
	 * @return An OutputStreamWriter.
	 * @throws Exception
	 */
	public static OutputStreamWriter openOutputFile(String filename) throws Exception {
		FileOutputStream fs = new FileOutputStream(filename);
		return new OutputStreamWriter(fs, StandardCharsets.UTF_8);
	}

	// =========== String Process ===========

	/**
	 * Escape String
	 * 
	 * Escape all characters which are invalid in string quote.
	 * 
	 * @param strl The string need to be escaped.
	 * @return The escaped string.
	 * @see removeEol
	 */
	public static String escapeString(String strl) {
		return strl.replace("\\", "\\\\").replace("\t", "\\t").replace("\b", "\\b").replace("\n", "\\n")
				.replace("\r", "\\r").replace("\f", "\\f").replace("\"", "\\\"");
	}

	/**
	 * Remove all EOL (End of Line) characters.
	 * 
	 * @param strl The string need to be processed.
	 * @return The string eliminated all EOL.
	 * @see escapeString
	 */
	public static String removeEol(String strl) {
		return strl.replace("\n", "").replace("\r", "");
	}

}
