import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.nio.file.Paths;
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
			// For line comment, we start to remove "//" prefix first
			String slashRemoved = comment.getText().substring(2);
			// Then remove successive starts
			String starsRemoved = removeSeperator(slashRemoved);
			// Do a strip for it.
			String stripped = starsRemoved.strip();
			// Then remove EOL to avoid unexpected line break.
			String eolRemoved = removeEol(stripped);
			// Okey
			return eolRemoved;
		}
		case CKGeneralLexer.CKGENERAL_BLOCK_COMMENT: {
			// For block comment, we first cut "/*" head and "*/" tail.
			String blockComment = comment.getText();
			String slashRemoved = blockComment.substring(2, blockComment.length() - 4);

			// Then we break it at line breaker and process each line one by one.
			String beautyComment = slashRemoved.lines().map((String line) -> {
				// Remove successive starts
				String starsRemoved = removeSeperator(line);
				// Do a strip for it.
				String stripped = starsRemoved.strip();
				// Then remove EOL to avoid unexpected line break.
				String eolRemoved = removeEol(stripped);
				// Line process is okey now
				return eolRemoved;
			}).collect(Collectors.joining("\n")); // Then re-join with fresh line breaker
			// Then return
			return beautyComment;
		}
		default:
			return comment.getText();
		}
	}

	/**
	 * Cut multiple comments.
	 * <p>
	 * Each comment will be cut the head and tail first. And strip all whitespace.
	 * Then join together.
	 * 
	 * @param tokens Multiple comments.
	 * @return The joined comment.
	 */
	public static String cutComments(List<Token> tokens) {
		if (tokens == null)
			return null;

		return tokens.stream().map(value -> {
			return cutComment(value);
		}).collect(Collectors.joining("\n"));
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
	 *
	 * @param numstr The captured number.
	 * @return true if it is hex number.
	 */
	public static boolean isHexNumber(String numstr) {
		return numstr.startsWith("0x") || numstr.startsWith("0X");
	}

	// =========== File Operations ===========

	private static Path getRootDirectoryPath() throws Exception {
		String rootDir = System.getenv("ENUMS_MIGRATION_ROOT");
		if (rootDir == null) {
			throw new RuntimeException("Can not find essential environment variable ENUMS_MIGRATION_ROOT");
		} else {
			return Paths.get(rootDir);
		}
	}

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

	public static String getInputFilePath(String filename) throws Exception {
		Path rootDir = getRootDirectoryPath();
		Path filePath = rootDir.resolve("Input").resolve(filename);
		return filePath.toString();
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

	public static String getOutputFilePath(String filename) throws Exception {
		Path rootDir = getRootDirectoryPath();
		Path filePath = rootDir.resolve("Intermediate").resolve(filename);
		return filePath.toString();
	}

	// =========== String Process ===========

	/**
	 * Remove all EOL (End of Line) characters.
	 * 
	 * @param strl The string need to be processed.
	 * @return The string eliminated all EOL.
	 */
	public static String removeEol(String strl) {
		return strl.replace("\n", "").replace("\r", "");
	}

	/**
	 * Remove seperator bar consisted by '*' or '-' (at least 5 successive chars)
	 * 
	 * @param cmt The string provided.
	 * @return The string processed.
	 */
	public static String removeSeperator(String cmt) {
		// only remove at least 5 continuous star chars.
		return cmt.replaceAll("[\\*\\-]{5,}", "");
	}

}
