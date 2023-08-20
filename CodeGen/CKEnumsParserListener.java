// Generated from CKEnumsParser.g4 by ANTLR 4.13.0
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link CKEnumsParser}.
 */
public interface CKEnumsParserListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link CKEnumsParser#prog}.
	 * @param ctx the parse tree
	 */
	void enterProg(CKEnumsParser.ProgContext ctx);
	/**
	 * Exit a parse tree produced by {@link CKEnumsParser#prog}.
	 * @param ctx the parse tree
	 */
	void exitProg(CKEnumsParser.ProgContext ctx);
	/**
	 * Enter a parse tree produced by {@link CKEnumsParser#enumBody}.
	 * @param ctx the parse tree
	 */
	void enterEnumBody(CKEnumsParser.EnumBodyContext ctx);
	/**
	 * Exit a parse tree produced by {@link CKEnumsParser#enumBody}.
	 * @param ctx the parse tree
	 */
	void exitEnumBody(CKEnumsParser.EnumBodyContext ctx);
	/**
	 * Enter a parse tree produced by {@link CKEnumsParser#entryPair}.
	 * @param ctx the parse tree
	 */
	void enterEntryPair(CKEnumsParser.EntryPairContext ctx);
	/**
	 * Exit a parse tree produced by {@link CKEnumsParser#entryPair}.
	 * @param ctx the parse tree
	 */
	void exitEntryPair(CKEnumsParser.EntryPairContext ctx);
	/**
	 * Enter a parse tree produced by the {@code entryDirectValue}
	 * labeled alternative in {@link CKEnumsParser#entryValue}.
	 * @param ctx the parse tree
	 */
	void enterEntryDirectValue(CKEnumsParser.EntryDirectValueContext ctx);
	/**
	 * Exit a parse tree produced by the {@code entryDirectValue}
	 * labeled alternative in {@link CKEnumsParser#entryValue}.
	 * @param ctx the parse tree
	 */
	void exitEntryDirectValue(CKEnumsParser.EntryDirectValueContext ctx);
	/**
	 * Enter a parse tree produced by the {@code entryRelativeValue}
	 * labeled alternative in {@link CKEnumsParser#entryValue}.
	 * @param ctx the parse tree
	 */
	void enterEntryRelativeValue(CKEnumsParser.EntryRelativeValueContext ctx);
	/**
	 * Exit a parse tree produced by the {@code entryRelativeValue}
	 * labeled alternative in {@link CKEnumsParser#entryValue}.
	 * @param ctx the parse tree
	 */
	void exitEntryRelativeValue(CKEnumsParser.EntryRelativeValueContext ctx);
}