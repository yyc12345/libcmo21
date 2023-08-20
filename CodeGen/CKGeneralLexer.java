// Generated from CKGeneralLexer.g4 by ANTLR 4.13.0
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue"})
public class CKGeneralLexer extends Lexer {
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
		COMMENTS=2, WHITESPACE=3;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN", "COMMENTS", "WHITESPACE"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"CKGENERAL_TYPEDEF", "CKGENERAL_DEFINE", "CKGENERAL_ENUM", "CKGENERAL_LBRACKET", 
			"CKGENERAL_RBRACKET", "CKGENERAL_EQUAL", "CKGENERAL_SEMICOLON", "CKGENERAL_LSHIFT", 
			"CKGENERAL_OR", "CKGENERAL_COMMA", "CKGENERAL_ID", "CKGENERAL_NUM", "CKGENERAL_LINE_COMMENT", 
			"CKGENERAL_BLOCK_COMMENT", "CKGENERAL_WS"
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


	public CKGeneralLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "CKGeneralLexer.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\u0004\u0000\u000fz\u0006\uffff\uffff\u0002\u0000\u0007\u0000\u0002\u0001"+
		"\u0007\u0001\u0002\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002\u0004"+
		"\u0007\u0004\u0002\u0005\u0007\u0005\u0002\u0006\u0007\u0006\u0002\u0007"+
		"\u0007\u0007\u0002\b\u0007\b\u0002\t\u0007\t\u0002\n\u0007\n\u0002\u000b"+
		"\u0007\u000b\u0002\f\u0007\f\u0002\r\u0007\r\u0002\u000e\u0007\u000e\u0001"+
		"\u0000\u0001\u0000\u0001\u0000\u0001\u0000\u0001\u0000\u0001\u0000\u0001"+
		"\u0000\u0001\u0000\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0002\u0001\u0002\u0001"+
		"\u0002\u0001\u0002\u0001\u0002\u0001\u0003\u0001\u0003\u0001\u0004\u0001"+
		"\u0004\u0001\u0005\u0001\u0005\u0001\u0006\u0001\u0006\u0001\u0007\u0001"+
		"\u0007\u0001\u0007\u0001\b\u0001\b\u0001\t\u0001\t\u0001\n\u0001\n\u0005"+
		"\nF\b\n\n\n\f\nI\t\n\u0001\u000b\u0001\u000b\u0001\u000b\u0003\u000bN"+
		"\b\u000b\u0001\u000b\u0004\u000bQ\b\u000b\u000b\u000b\f\u000bR\u0001\u000b"+
		"\u0005\u000bV\b\u000b\n\u000b\f\u000bY\t\u000b\u0001\f\u0001\f\u0001\f"+
		"\u0001\f\u0005\f_\b\f\n\f\f\fb\t\f\u0001\f\u0001\f\u0001\r\u0001\r\u0001"+
		"\r\u0001\r\u0005\rj\b\r\n\r\f\rm\t\r\u0001\r\u0001\r\u0001\r\u0001\r\u0001"+
		"\r\u0001\u000e\u0004\u000eu\b\u000e\u000b\u000e\f\u000ev\u0001\u000e\u0001"+
		"\u000e\u0001k\u0000\u000f\u0001\u0001\u0003\u0002\u0005\u0003\u0007\u0004"+
		"\t\u0005\u000b\u0006\r\u0007\u000f\b\u0011\t\u0013\n\u0015\u000b\u0017"+
		"\f\u0019\r\u001b\u000e\u001d\u000f\u0001\u0000\u0007\u0003\u0000AZ__a"+
		"z\u0004\u000009AZ__az\u0002\u0000XXxx\u0003\u000009AFaf\u0004\u0000LL"+
		"UUlluu\u0002\u0000\n\n\r\r\u0003\u0000\t\n\r\r  \u0081\u0000\u0001\u0001"+
		"\u0000\u0000\u0000\u0000\u0003\u0001\u0000\u0000\u0000\u0000\u0005\u0001"+
		"\u0000\u0000\u0000\u0000\u0007\u0001\u0000\u0000\u0000\u0000\t\u0001\u0000"+
		"\u0000\u0000\u0000\u000b\u0001\u0000\u0000\u0000\u0000\r\u0001\u0000\u0000"+
		"\u0000\u0000\u000f\u0001\u0000\u0000\u0000\u0000\u0011\u0001\u0000\u0000"+
		"\u0000\u0000\u0013\u0001\u0000\u0000\u0000\u0000\u0015\u0001\u0000\u0000"+
		"\u0000\u0000\u0017\u0001\u0000\u0000\u0000\u0000\u0019\u0001\u0000\u0000"+
		"\u0000\u0000\u001b\u0001\u0000\u0000\u0000\u0000\u001d\u0001\u0000\u0000"+
		"\u0000\u0001\u001f\u0001\u0000\u0000\u0000\u0003\'\u0001\u0000\u0000\u0000"+
		"\u0005/\u0001\u0000\u0000\u0000\u00074\u0001\u0000\u0000\u0000\t6\u0001"+
		"\u0000\u0000\u0000\u000b8\u0001\u0000\u0000\u0000\r:\u0001\u0000\u0000"+
		"\u0000\u000f<\u0001\u0000\u0000\u0000\u0011?\u0001\u0000\u0000\u0000\u0013"+
		"A\u0001\u0000\u0000\u0000\u0015C\u0001\u0000\u0000\u0000\u0017M\u0001"+
		"\u0000\u0000\u0000\u0019Z\u0001\u0000\u0000\u0000\u001be\u0001\u0000\u0000"+
		"\u0000\u001dt\u0001\u0000\u0000\u0000\u001f \u0005t\u0000\u0000 !\u0005"+
		"y\u0000\u0000!\"\u0005p\u0000\u0000\"#\u0005e\u0000\u0000#$\u0005d\u0000"+
		"\u0000$%\u0005e\u0000\u0000%&\u0005f\u0000\u0000&\u0002\u0001\u0000\u0000"+
		"\u0000\'(\u0005#\u0000\u0000()\u0005d\u0000\u0000)*\u0005e\u0000\u0000"+
		"*+\u0005f\u0000\u0000+,\u0005i\u0000\u0000,-\u0005n\u0000\u0000-.\u0005"+
		"e\u0000\u0000.\u0004\u0001\u0000\u0000\u0000/0\u0005e\u0000\u000001\u0005"+
		"n\u0000\u000012\u0005u\u0000\u000023\u0005m\u0000\u00003\u0006\u0001\u0000"+
		"\u0000\u000045\u0005{\u0000\u00005\b\u0001\u0000\u0000\u000067\u0005}"+
		"\u0000\u00007\n\u0001\u0000\u0000\u000089\u0005=\u0000\u00009\f\u0001"+
		"\u0000\u0000\u0000:;\u0005;\u0000\u0000;\u000e\u0001\u0000\u0000\u0000"+
		"<=\u0005<\u0000\u0000=>\u0005<\u0000\u0000>\u0010\u0001\u0000\u0000\u0000"+
		"?@\u0005|\u0000\u0000@\u0012\u0001\u0000\u0000\u0000AB\u0005,\u0000\u0000"+
		"B\u0014\u0001\u0000\u0000\u0000CG\u0007\u0000\u0000\u0000DF\u0007\u0001"+
		"\u0000\u0000ED\u0001\u0000\u0000\u0000FI\u0001\u0000\u0000\u0000GE\u0001"+
		"\u0000\u0000\u0000GH\u0001\u0000\u0000\u0000H\u0016\u0001\u0000\u0000"+
		"\u0000IG\u0001\u0000\u0000\u0000JK\u00050\u0000\u0000KN\u0007\u0002\u0000"+
		"\u0000LN\u0005-\u0000\u0000MJ\u0001\u0000\u0000\u0000ML\u0001\u0000\u0000"+
		"\u0000MN\u0001\u0000\u0000\u0000NP\u0001\u0000\u0000\u0000OQ\u0007\u0003"+
		"\u0000\u0000PO\u0001\u0000\u0000\u0000QR\u0001\u0000\u0000\u0000RP\u0001"+
		"\u0000\u0000\u0000RS\u0001\u0000\u0000\u0000SW\u0001\u0000\u0000\u0000"+
		"TV\u0007\u0004\u0000\u0000UT\u0001\u0000\u0000\u0000VY\u0001\u0000\u0000"+
		"\u0000WU\u0001\u0000\u0000\u0000WX\u0001\u0000\u0000\u0000X\u0018\u0001"+
		"\u0000\u0000\u0000YW\u0001\u0000\u0000\u0000Z[\u0005/\u0000\u0000[\\\u0005"+
		"/\u0000\u0000\\`\u0001\u0000\u0000\u0000]_\b\u0005\u0000\u0000^]\u0001"+
		"\u0000\u0000\u0000_b\u0001\u0000\u0000\u0000`^\u0001\u0000\u0000\u0000"+
		"`a\u0001\u0000\u0000\u0000ac\u0001\u0000\u0000\u0000b`\u0001\u0000\u0000"+
		"\u0000cd\u0006\f\u0000\u0000d\u001a\u0001\u0000\u0000\u0000ef\u0005/\u0000"+
		"\u0000fg\u0005*\u0000\u0000gk\u0001\u0000\u0000\u0000hj\t\u0000\u0000"+
		"\u0000ih\u0001\u0000\u0000\u0000jm\u0001\u0000\u0000\u0000kl\u0001\u0000"+
		"\u0000\u0000ki\u0001\u0000\u0000\u0000ln\u0001\u0000\u0000\u0000mk\u0001"+
		"\u0000\u0000\u0000no\u0005*\u0000\u0000op\u0005/\u0000\u0000pq\u0001\u0000"+
		"\u0000\u0000qr\u0006\r\u0000\u0000r\u001c\u0001\u0000\u0000\u0000su\u0007"+
		"\u0006\u0000\u0000ts\u0001\u0000\u0000\u0000uv\u0001\u0000\u0000\u0000"+
		"vt\u0001\u0000\u0000\u0000vw\u0001\u0000\u0000\u0000wx\u0001\u0000\u0000"+
		"\u0000xy\u0006\u000e\u0001\u0000y\u001e\u0001\u0000\u0000\u0000\b\u0000"+
		"GMRW`kv\u0002\u0000\u0002\u0000\u0000\u0003\u0000";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}