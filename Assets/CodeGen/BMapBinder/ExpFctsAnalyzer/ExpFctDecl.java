import java.util.Vector;

/**
 * The class represent an export BMap function.
 */
public class ExpFctDecl {

	/**
	 * The name of this function.
	 */
	public String mFctName;
	/**
	 * The return value type of this function.
	 */
	public VariableType mFctRetType;
	/**
	 * The parameters (arguments) list of this function. Each items are
	 * {@linkplain ExpFctParamDecl} and represent parameter one by one from left to
	 * right.
	 */
	public Vector<ExpFctParamDecl> mFctParams;

	public ExpFctDecl() {
		mFctName = "";
		mFctRetType = new VariableType();
		mFctParams = new Vector<ExpFctParamDecl>();
	}

}
