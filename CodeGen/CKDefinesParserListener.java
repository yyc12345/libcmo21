// Generated from CKDefinesParser.g4 by ANTLR 4.13.0
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link CKDefinesParser}.
 */
public interface CKDefinesParserListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link CKDefinesParser#prog}.
	 * @param ctx the parse tree
	 */
	void enterProg(CKDefinesParser.ProgContext ctx);
	/**
	 * Exit a parse tree produced by {@link CKDefinesParser#prog}.
	 * @param ctx the parse tree
	 */
	void exitProg(CKDefinesParser.ProgContext ctx);
	/**
	 * Enter a parse tree produced by {@link CKDefinesParser#definePair}.
	 * @param ctx the parse tree
	 */
	void enterDefinePair(CKDefinesParser.DefinePairContext ctx);
	/**
	 * Exit a parse tree produced by {@link CKDefinesParser#definePair}.
	 * @param ctx the parse tree
	 */
	void exitDefinePair(CKDefinesParser.DefinePairContext ctx);
}