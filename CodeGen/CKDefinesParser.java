// Generated from CKDefinesParser.g4 by ANTLR 4.13.0
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue"})
public class CKDefinesParser extends Parser {
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
		RULE_prog = 0, RULE_definePair = 1;
	private static String[] makeRuleNames() {
		return new String[] {
			"prog", "definePair"
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
	public String getGrammarFileName() { return "CKDefinesParser.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public CKDefinesParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ProgContext extends ParserRuleContext {
		public List<DefinePairContext> definePair() {
			return getRuleContexts(DefinePairContext.class);
		}
		public DefinePairContext definePair(int i) {
			return getRuleContext(DefinePairContext.class,i);
		}
		public ProgContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_prog; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof CKDefinesParserListener ) ((CKDefinesParserListener)listener).enterProg(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof CKDefinesParserListener ) ((CKDefinesParserListener)listener).exitProg(this);
		}
	}

	public final ProgContext prog() throws RecognitionException {
		ProgContext _localctx = new ProgContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_prog);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(5); 
			_errHandler.sync(this);
			_la = _input.LA(1);
			do {
				{
				{
				setState(4);
				definePair();
				}
				}
				setState(7); 
				_errHandler.sync(this);
				_la = _input.LA(1);
			} while ( _la==CKGENERAL_DEFINE );
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
	public static class DefinePairContext extends ParserRuleContext {
		public TerminalNode CKGENERAL_DEFINE() { return getToken(CKDefinesParser.CKGENERAL_DEFINE, 0); }
		public TerminalNode CKGENERAL_ID() { return getToken(CKDefinesParser.CKGENERAL_ID, 0); }
		public TerminalNode CKGENERAL_NUM() { return getToken(CKDefinesParser.CKGENERAL_NUM, 0); }
		public DefinePairContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_definePair; }
		@Override
		public void enterRule(ParseTreeListener listener) {
			if ( listener instanceof CKDefinesParserListener ) ((CKDefinesParserListener)listener).enterDefinePair(this);
		}
		@Override
		public void exitRule(ParseTreeListener listener) {
			if ( listener instanceof CKDefinesParserListener ) ((CKDefinesParserListener)listener).exitDefinePair(this);
		}
	}

	public final DefinePairContext definePair() throws RecognitionException {
		DefinePairContext _localctx = new DefinePairContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_definePair);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(9);
			match(CKGENERAL_DEFINE);
			setState(10);
			match(CKGENERAL_ID);
			setState(11);
			match(CKGENERAL_NUM);
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
		"\u0004\u0001\u000f\u000e\u0002\u0000\u0007\u0000\u0002\u0001\u0007\u0001"+
		"\u0001\u0000\u0004\u0000\u0006\b\u0000\u000b\u0000\f\u0000\u0007\u0001"+
		"\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0000\u0000\u0002"+
		"\u0000\u0002\u0000\u0000\f\u0000\u0005\u0001\u0000\u0000\u0000\u0002\t"+
		"\u0001\u0000\u0000\u0000\u0004\u0006\u0003\u0002\u0001\u0000\u0005\u0004"+
		"\u0001\u0000\u0000\u0000\u0006\u0007\u0001\u0000\u0000\u0000\u0007\u0005"+
		"\u0001\u0000\u0000\u0000\u0007\b\u0001\u0000\u0000\u0000\b\u0001\u0001"+
		"\u0000\u0000\u0000\t\n\u0005\u0002\u0000\u0000\n\u000b\u0005\u000b\u0000"+
		"\u0000\u000b\f\u0005\f\u0000\u0000\f\u0003\u0001\u0000\u0000\u0000\u0001"+
		"\u0007";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}