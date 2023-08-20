
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

/**
 * The generic walker for collecting defines as a enum.
 */
public class DefinesWalker extends CKDefinesParserBaseListener {

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
