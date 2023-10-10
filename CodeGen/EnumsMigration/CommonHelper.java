import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.stream.Collectors;

import org.antlr.v4.runtime.*;

public class CommonHelper {

	// =========== Token Finder & Comments Processing ===========

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
	
	public static List<Token> getPreChannelTokens(BufferedTokenStream stream, Token token, int channel) {
		return stream.getHiddenTokensToLeft(token.getTokenIndex(), channel);
	}

	public static List<Token> getPostChannelTokens(BufferedTokenStream stream, Token token, int channel) {
		return stream.getHiddenTokensToRight(token.getTokenIndex(), channel);
	}

	/**
	 * Cut the head and tail of comment
	 * 
	 * @param comment The comment need to be cut.
	 * @return The cut comment.
	 */
	public static String cutComment(Token comment) {
		if (comment == null)
			return null;

		switch (comment.getType()) {
		case CKGeneralLexer.CKGENERAL_LINE_COMMENT: {
			return removeStars(comment.getText().substring(2));
		}
		case CKGeneralLexer.CKGENERAL_BLOCK_COMMENT: {
			String cache = comment.getText();
			return removeStars(cache.substring(2, cache.length() - 4));
		}
		default:
			return comment.getText();
		}
	}

	/**
	 * Cut multiple comments.
	 * <p>
	 * Each comment will be cut the head and tail first.
	 * And strip all whitespace.
	 * Then join together.
	 * 
	 * @param tokens Multiple comments.
	 * @return The joined comment.
	 */
	public static String cutComments(List<Token> tokens) {
		if (tokens == null)
			return null;

		return tokens.stream().map(value -> {
			String text = cutComment(value).strip();
			return text + " ";
		}).collect(Collectors.joining(""));
	}
	
	// =========== Number Operations ===========

	/**
	 * Check whether Antlr captured CKGENERAL_NUM is a negative number.
	 * 
	 * @param numstr The captured number.
	 * @return true if it is negative number.
	 */
	public static boolean isNegativeNumber(String numstr) {
		return numstr.startsWith("-");
	}

	/**
	 * Check whether Altlr captured CKGENERAL_NUM is a hex number.
	 * @param numstr The captured number.
	 * @return true if it is hex number.
	 */
	public static boolean isHexNumber(String numstr) {
		return numstr.startsWith("0x") || numstr.startsWith("0X");
	}
	
	/**
	 * Get underlying type of enum.
	 * 
	 * @param canUnsigned The parameter stored in Enum_t that indiccate whether this
	 *                    enum can use unsigned int as its underlying type.
	 * @return The string form of its underlying type.
	 */
	public static String getEnumUnderlyingType(boolean canUnsigned) {
		return canUnsigned ? "CKDWORD" : "CKINT";
	}

	// =========== Parts ===========

	enum CKParts {
		CK2, VxMath
	}
	
	enum LangType {
		CPP, Python
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
	 * 
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

	/**
	 * Remove redundent star '*' (at least 5 continuous star chars)
	 * @param cmt The string provided.
	 * @return The string processed.
	 */
	public static String removeStars(String cmt) {
		// only remove at least 5 continuous star chars.
		return cmt.replaceAll("\\*{5,}", "");
	}
	
}
