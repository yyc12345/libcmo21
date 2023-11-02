import java.io.FileInputStream;
import java.nio.charset.StandardCharsets;
import java.util.Vector;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class MainRunner {

	public static void main(String[] args) throws Exception {
		// get interface structture
		FileInputStream fs = new FileInputStream("dest/BMExports.hpp");
		CharStream antlrfs = CharStreams.fromStream(fs, StandardCharsets.UTF_8);
		ExpFctsLexer lexer = new ExpFctsLexer(antlrfs);
		CommonTokenStream tokens = new CommonTokenStream(lexer);
		ExpFctsParser parser = new ExpFctsParser(tokens);

		ParseTree tree = parser.program();
		ParseTreeWalker walker = new ParseTreeWalker();
		ExpFctsWalker worker = new ExpFctsWalker();
		walker.walk(worker, tree);
		fs.close();

		Vector<ExpFctDecl> result = worker.getResult();
		
		// print message.
		System.out.println("DONE!");
	}
}
