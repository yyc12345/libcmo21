// Generated from CKEnumsParser.g4 by ANTLR 4.13.0
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue"})
public class CKEnumsParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.13.0", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		CKGENERAL_TYPEDEF=1, CKGENERAL_DEFINE=2, CKGENERAL_ENUM=3, CKGENERAL_LBRACKET=4, 
		CKGENERAL_RBRACKET=5, CKGENERAL_EQUAL=6, CKGENERAL_SEMICOLON=7, CKGENERAL_LSHIFT=8, 
		CKGENERAL_OR=9, CKGENERAL_COMMA=10, CKGENERAL_ID=11, CKGENERAL_NUM=12, 
		CKGENERAL_LINE_COMMENT=13, CKGENERAL_BLOCK_COMMENT=14, CKGENERAL_WS=15;
	public static final int
		RULE_prog = 0, RULE_enumBody = 1, RULE_entryPair = 2, RULE_entryValue = 3;
	private static String[] makeRuleNames() {
		return new String[] {
			"prog", "enumBody", "entryPair", "entryValue"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'typedef'", "'#define'", "'enum'", "'{'", "'}'", "'='", "';'", 
			"'<<'", "'|'", "','"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "CKGENERAL_TYPEDEF", "CKGENERAL_DEFINE", "CKGENERAL_ENUM", "CKGENERAL_LBRACKET", 
			"CKGENERAL_RBRACKET", "CKGENERAL_EQUAL", "CKGENERAL_SEMICOLON", "CKGENERAL_LSHIFT", 
			"CKGENERAL_OR", "CKGENERAL_COMMA", "CKGENERAL_ID", "CKGENERAL_NUM", "CKGENERAL_LINE_COMMENT", 
			"CKGENERAL_BLOCK_COMMENT", "CKGENERAL_WS"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "CKEnumsParser.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public CKEnumsParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ProgContext extends ParserRuleContext {
		public List<EnumBodyContext> enumBody() {
			return getRuleContexts(EnumBodyContext.class);
		}
		public EnumBodyContext enumBody(int i) {
			return getRuleContext(EnumBodyContext.class,i);
		}
		public ProgContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_prog; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).enterProg(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).exitProg(this);
		}
	}

	public final ProgContext prog() throws RecognitionException {
		ProgContext _localctx = new ProgContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_prog);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(11);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==CKGENERAL_TYPEDEF || _la==CKGENERAL_ENUM) {
				{
				{
				setState(8);
				enumBody();
				}
				}
				setState(13);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class EnumBodyContext extends ParserRuleContext {
		public TerminalNode CKGENERAL_ENUM() { return getToken(CKEnumsParser.CKGENERAL_ENUM, 0); }
		public List<TerminalNode> CKGENERAL_ID() { return getTokens(CKEnumsParser.CKGENERAL_ID); }
		public TerminalNode CKGENERAL_ID(int i) {
			return getToken(CKEnumsParser.CKGENERAL_ID, i);
		}
		public TerminalNode CKGENERAL_LBRACKET() { return getToken(CKEnumsParser.CKGENERAL_LBRACKET, 0); }
		public TerminalNode CKGENERAL_RBRACKET() { return getToken(CKEnumsParser.CKGENERAL_RBRACKET, 0); }
		public TerminalNode CKGENERAL_SEMICOLON() { return getToken(CKEnumsParser.CKGENERAL_SEMICOLON, 0); }
		public TerminalNode CKGENERAL_TYPEDEF() { return getToken(CKEnumsParser.CKGENERAL_TYPEDEF, 0); }
		public List<EntryPairContext> entryPair() {
			return getRuleContexts(EntryPairContext.class);
		}
		public EntryPairContext entryPair(int i) {
			return getRuleContext(EntryPairContext.class,i);
		}
		public EnumBodyContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_enumBody; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).enterEnumBody(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).exitEnumBody(this);
		}
	}

	public final EnumBodyContext enumBody() throws RecognitionException {
		EnumBodyContext _localctx = new EnumBodyContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_enumBody);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(15);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==CKGENERAL_TYPEDEF) {
				{
				setState(14);
				match(CKGENERAL_TYPEDEF);
				}
			}

			setState(17);
			match(CKGENERAL_ENUM);
			setState(18);
			match(CKGENERAL_ID);
			setState(19);
			match(CKGENERAL_LBRACKET);
			setState(21); 
			_errHandler.sync(this);
			_la = _input.LA(1);
			do {
				{
				{
				setState(20);
				entryPair();
				}
				}
				setState(23); 
				_errHandler.sync(this);
				_la = _input.LA(1);
			} while ( _la==CKGENERAL_ID );
			setState(25);
			match(CKGENERAL_RBRACKET);
			setState(27);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==CKGENERAL_ID) {
				{
				setState(26);
				match(CKGENERAL_ID);
				}
			}

			setState(29);
			match(CKGENERAL_SEMICOLON);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class EntryPairContext extends ParserRuleContext {
		public TerminalNode CKGENERAL_ID() { return getToken(CKEnumsParser.CKGENERAL_ID, 0); }
		public TerminalNode CKGENERAL_EQUAL() { return getToken(CKEnumsParser.CKGENERAL_EQUAL, 0); }
		public EntryValueContext entryValue() {
			return getRuleContext(EntryValueContext.class,0);
		}
		public TerminalNode CKGENERAL_COMMA() { return getToken(CKEnumsParser.CKGENERAL_COMMA, 0); }
		public EntryPairContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_entryPair; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).enterEntryPair(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).exitEntryPair(this);
		}
	}

	public final EntryPairContext entryPair() throws RecognitionException {
		EntryPairContext _localctx = new EntryPairContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_entryPair);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(31);
			match(CKGENERAL_ID);
			setState(34);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==CKGENERAL_EQUAL) {
				{
				setState(32);
				match(CKGENERAL_EQUAL);
				setState(33);
				entryValue();
				}
			}

			setState(37);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==CKGENERAL_COMMA) {
				{
				setState(36);
				match(CKGENERAL_COMMA);
				}
			}

			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class EntryValueContext extends ParserRuleContext {
		public EntryValueContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_entryValue; }
	 
		public EntryValueContext() { }
		public void copyFrom(EntryValueContext ctx) {
			super.copyFrom(ctx);
		}
	}
	@SuppressWarnings("CheckReturnValue")
	public static class EntryDirectValueContext extends EntryValueContext {
		public List<TerminalNode> CKGENERAL_NUM() { return getTokens(CKEnumsParser.CKGENERAL_NUM); }
		public TerminalNode CKGENERAL_NUM(int i) {
			return getToken(CKEnumsParser.CKGENERAL_NUM, i);
		}
		public TerminalNode CKGENERAL_LSHIFT() { return getToken(CKEnumsParser.CKGENERAL_LSHIFT, 0); }
		public EntryDirectValueContext(EntryValueContext ctx) { copyFrom(ctx); }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).enterEntryDirectValue(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).exitEntryDirectValue(this);
		}
	}
	@SuppressWarnings("CheckReturnValue")
	public static class EntryRelativeValueContext extends EntryValueContext {
		public List<TerminalNode> CKGENERAL_ID() { return getTokens(CKEnumsParser.CKGENERAL_ID); }
		public TerminalNode CKGENERAL_ID(int i) {
			return getToken(CKEnumsParser.CKGENERAL_ID, i);
		}
		public List<TerminalNode> CKGENERAL_OR() { return getTokens(CKEnumsParser.CKGENERAL_OR); }
		public TerminalNode CKGENERAL_OR(int i) {
			return getToken(CKEnumsParser.CKGENERAL_OR, i);
		}
		public EntryRelativeValueContext(EntryValueContext ctx) { copyFrom(ctx); }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).enterEntryRelativeValue(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof CKEnumsParserListener ) ((CKEnumsParserListener)listener).exitEntryRelativeValue(this);
		}
	}

	public final EntryValueContext entryValue() throws RecognitionException {
		EntryValueContext _localctx = new EntryValueContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_entryValue);
		int _la;
		try {
			setState(52);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case CKGENERAL_NUM:
				_localctx = new EntryDirectValueContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(39);
				match(CKGENERAL_NUM);
				setState(42);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==CKGENERAL_LSHIFT) {
					{
					setState(40);
					match(CKGENERAL_LSHIFT);
					setState(41);
					match(CKGENERAL_NUM);
					}
				}

				}
				break;
			case CKGENERAL_ID:
				_localctx = new EntryRelativeValueContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(44);
				match(CKGENERAL_ID);
				setState(49);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==CKGENERAL_OR) {
					{
					{
					setState(45);
					match(CKGENERAL_OR);
					setState(46);
					match(CKGENERAL_ID);
					}
					}
					setState(51);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static final String _serializedATN =
		"\u0004\u0001\u000f7\u0002\u0000\u0007\u0000\u0002\u0001\u0007\u0001\u0002"+
		"\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0001\u0000\u0005\u0000\n\b"+
		"\u0000\n\u0000\f\u0000\r\t\u0000\u0001\u0001\u0003\u0001\u0010\b\u0001"+
		"\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0004\u0001\u0016\b\u0001"+
		"\u000b\u0001\f\u0001\u0017\u0001\u0001\u0001\u0001\u0003\u0001\u001c\b"+
		"\u0001\u0001\u0001\u0001\u0001\u0001\u0002\u0001\u0002\u0001\u0002\u0003"+
		"\u0002#\b\u0002\u0001\u0002\u0003\u0002&\b\u0002\u0001\u0003\u0001\u0003"+
		"\u0001\u0003\u0003\u0003+\b\u0003\u0001\u0003\u0001\u0003\u0001\u0003"+
		"\u0005\u00030\b\u0003\n\u0003\f\u00033\t\u0003\u0003\u00035\b\u0003\u0001"+
		"\u0003\u0000\u0000\u0004\u0000\u0002\u0004\u0006\u0000\u0000;\u0000\u000b"+
		"\u0001\u0000\u0000\u0000\u0002\u000f\u0001\u0000\u0000\u0000\u0004\u001f"+
		"\u0001\u0000\u0000\u0000\u00064\u0001\u0000\u0000\u0000\b\n\u0003\u0002"+
		"\u0001\u0000\t\b\u0001\u0000\u0000\u0000\n\r\u0001\u0000\u0000\u0000\u000b"+
		"\t\u0001\u0000\u0000\u0000\u000b\f\u0001\u0000\u0000\u0000\f\u0001\u0001"+
		"\u0000\u0000\u0000\r\u000b\u0001\u0000\u0000\u0000\u000e\u0010\u0005\u0001"+
		"\u0000\u0000\u000f\u000e\u0001\u0000\u0000\u0000\u000f\u0010\u0001\u0000"+
		"\u0000\u0000\u0010\u0011\u0001\u0000\u0000\u0000\u0011\u0012\u0005\u0003"+
		"\u0000\u0000\u0012\u0013\u0005\u000b\u0000\u0000\u0013\u0015\u0005\u0004"+
		"\u0000\u0000\u0014\u0016\u0003\u0004\u0002\u0000\u0015\u0014\u0001\u0000"+
		"\u0000\u0000\u0016\u0017\u0001\u0000\u0000\u0000\u0017\u0015\u0001\u0000"+
		"\u0000\u0000\u0017\u0018\u0001\u0000\u0000\u0000\u0018\u0019\u0001\u0000"+
		"\u0000\u0000\u0019\u001b\u0005\u0005\u0000\u0000\u001a\u001c\u0005\u000b"+
		"\u0000\u0000\u001b\u001a\u0001\u0000\u0000\u0000\u001b\u001c\u0001\u0000"+
		"\u0000\u0000\u001c\u001d\u0001\u0000\u0000\u0000\u001d\u001e\u0005\u0007"+
		"\u0000\u0000\u001e\u0003\u0001\u0000\u0000\u0000\u001f\"\u0005\u000b\u0000"+
		"\u0000 !\u0005\u0006\u0000\u0000!#\u0003\u0006\u0003\u0000\" \u0001\u0000"+
		"\u0000\u0000\"#\u0001\u0000\u0000\u0000#%\u0001\u0000\u0000\u0000$&\u0005"+
		"\n\u0000\u0000%$\u0001\u0000\u0000\u0000%&\u0001\u0000\u0000\u0000&\u0005"+
		"\u0001\u0000\u0000\u0000\'*\u0005\f\u0000\u0000()\u0005\b\u0000\u0000"+
		")+\u0005\f\u0000\u0000*(\u0001\u0000\u0000\u0000*+\u0001\u0000\u0000\u0000"+
		"+5\u0001\u0000\u0000\u0000,1\u0005\u000b\u0000\u0000-.\u0005\t\u0000\u0000"+
		".0\u0005\u000b\u0000\u0000/-\u0001\u0000\u0000\u000003\u0001\u0000\u0000"+
		"\u00001/\u0001\u0000\u0000\u000012\u0001\u0000\u0000\u000025\u0001\u0000"+
		"\u0000\u000031\u0001\u0000\u0000\u00004\'\u0001\u0000\u0000\u00004,\u0001"+
		"\u0000\u0000\u00005\u0007\u0001\u0000\u0000\u0000\t\u000b\u000f\u0017"+
		"\u001b\"%*14";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}