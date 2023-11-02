import java.util.Vector;

public class ExpFctDecl {
	
	public String mFctName;
	public VariableType mFctRetType;
	public Vector<ExpFctParamDecl> mFctParams;
	
	public ExpFctDecl() {
		mFctName = "";
		mFctRetType = new VariableType();
		mFctParams = new Vector<ExpFctParamDecl>();
	}
	
}

