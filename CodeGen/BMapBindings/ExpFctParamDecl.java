
/**
 * The class represent a single parameter (argument) of function. This class
 * usually is the member of {@linkplain ExpFctDecl}.
 */
public class ExpFctParamDecl {

	/**
	 * The type of this parameter.
	 */
	public VariableType mVarType;
	/**
	 * The name of this parameter.
	 */
	public String mVarName;
	/**
	 * True if this paramter is marked as input parameter, otherwise false.
	 * <p>
	 * Input parameter and output paramter is commonly used in C/C++ code. By using
	 * this feature, each function can receive multiple arguments and return
	 * multiple arguments without defining a struct to hold it.
	 * <p>
	 * The type of input parameter is itself. However, the type of output parameter
	 * is the pointer of itself. So you may need get its pointer type when
	 * processing output paramter, especially for the scenario that the target
	 * language do not support explicit output parameter keyword.
	 */
	public boolean mIsInput;
	/**
	 * The description of this parameter.
	 * <p>
	 * This description is generated by this program. It will indicate the
	 * underlying C++ type to tell end user how to treat this paramter because some
	 * target languages' native calling style can not represent these detail.
	 * <p>
	 * In this program, this field must be written as a annotation of corresponding
	 * function.
	 */
	public String mVarDesc;

	public ExpFctParamDecl() {
		mVarType = new VariableType();
		mVarName = "";
		mVarDesc = "";
		mIsInput = true;
	}

}
