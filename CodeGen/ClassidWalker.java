
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

/**
 * The specialized walker for collecting CK_CLASSID and its inherit relationship.
 */
public class ClassidWalker extends CKDefinesParserBaseListener {

	@Override
	public void enterProg(CKDefinesParser.ProgContext ctx) {
		// TODO Auto-generated method stub
		super.enterProg(ctx);
	}

	@Override
	public void exitProg(CKDefinesParser.ProgContext ctx) {
		// TODO Auto-generated method stub
		super.exitProg(ctx);
	}

	@Override
	public void exitDefinePair(CKDefinesParser.DefinePairContext ctx) {
		// TODO Auto-generated method stub
		super.exitDefinePair(ctx);
	}
	
}
