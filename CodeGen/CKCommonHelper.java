import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.nio.charset.Charset;
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

	public static String getEnumUnderlayingType(boolean canUnsigned) {
		return canUnsigned ? "uint32_t" : "int32_t";
	}
	
	public static OutputStreamWriter openOutputFile(String filename) throws Exception {
		FileOutputStream fs = new FileOutputStream(filename);
		return new OutputStreamWriter(fs, StandardCharsets.UTF_8);
	}

	public static String escapeString(String strl) {
		
	}
	
}
