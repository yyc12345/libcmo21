import java.util.Vector;

public class ExpFctDecl {
	
	public String mFctName;
	public VariableType mFctRetType;
	public Vector<ExpFctParamDecl> mFctParams;
	
	public ExpFctDecl(String fct_name, VariableType ret_type) {
		mFctName = fct_name;
		mFctRetType = ret_type;
		mFctParams = new Vector<ExpFctParamDecl>();
	}
	
}

