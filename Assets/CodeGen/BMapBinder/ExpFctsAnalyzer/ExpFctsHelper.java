import java.util.Collections;
import java.util.Vector;
import java.util.stream.Collectors;

public class ExpFctsHelper {

	/**
	 * The class represent the type of each parameters and function return value.
	 */
	public static class VariableType {
		/**
		 * The base type of this variable removing all ending stars (remove all pointer levels).
		 * Each item in this Vector is a part of namespace and the last one must be the type itself
		 * (without any namespace constraint).
		 * If no namespace constraint for this type, this Vector will only have one item.
		 * <p>
		 * For end user, it is enough that knowing the last item is type itself.
		 */
		private Vector<String> mBaseType;
		/**
		 * The pointer level of this type. It is equal to the count of trailing star of
		 * this field in C style representation.
		 */
		private int mPointerLevel;

		/**
		 * Construct an empty varible type. This is commonly used constructor.
		 */
		public VariableType() {
			mBaseType = new Vector<String>();
			mPointerLevel = 0;
		}

		/**
		 * The constructor used for cloning self. This constructor is only can be used
		 * by self.
		 *
		 * @param base_type     The hierarchy of the variable type.
		 * @param pointer_level The pointer level of new created variable type.
		 */
		private VariableType(Vector<String> base_type, int pointer_level) {
			mBaseType = (Vector<String>) base_type.clone();
			mPointerLevel = pointer_level;
		}

		/**
		 * Set this variable type with a type string in C/C++ style. For example
		 * "NSTest::NSTest2::MyType**".
		 *
		 * @param ctype The type string in C/C++ style.
		 */
		public void fromCType(String ctype) {
			if (ctype.isEmpty())
				throw new IllegalArgumentException("empty string can not be parsed.");

			// get pointer part and name part
			int len = ctype.length();
			int star_pos = ctype.indexOf('*');
			String namepart;
			if (star_pos == -1) {
				// no star
				namepart = ctype;
				mPointerLevel = 0;
			} else {
				// has star
				if (star_pos == 0)
					throw new IllegalArgumentException("base type not found.");
				namepart = ctype.substring(0, star_pos);
				mPointerLevel = len - star_pos;
			}

			// resolve name part
			mBaseType.clear();
			for (String item : namepart.split("::")) {
				mBaseType.add(item);
			}
		}

		/**
		 * Build a type string represented by this variable type in C/C++ style.
		 *
		 * @return The type string in C/C++ style.
		 */
		public String toCType() {
			return mBaseType.stream().collect(Collectors.joining("::"))
					+ String.join("", Collections.nCopies(mPointerLevel, "*"));
		}

		/**
		 * Get the base type of this variable type without any namespace. It just simply
		 * get the last entry in type hierarchy.
		 *
		 * @return The base type string without namespace prefix.
		 */
		public String getBaseType() {
			return mBaseType.lastElement();
		}

		/**
		 * Check whether this variable type is a pointer. This function just check
		 * whether the pointer level of this variavle type is zero.
		 *
		 * @return True if it is pointer, otherwise false.
		 */
		public boolean isPointer() {
			return mPointerLevel != 0;
		}

		/**
		 * Return the pointer level of this variable type. You can simply assume the
		 * pointer level is equal to the count of trailing star.
		 *
		 * @return The pointer level integer. Zero means that this type is not a
		 *         pointer.
		 */
		public int getPointerLevel() {
			return mPointerLevel;
		}

		/**
		 * Return the clone of the type hierarchy of this variable type.
		 * <p>
		 * It is rarely used. This only should be used when you need the namespace
		 * hierarchy of this variable type.
		 *
		 * @return The clone of current variable type hierarchy.
		 */
		public Vector<String> getBaseTypeHierarchy() {
			return (Vector<String>) mBaseType.clone();
		}

		/**
		 * Check whether this type is a valid one. It actually check whether type
		 * hierarchy include at least one entry.
		 *
		 * @return True if no problem of this type, otherwise false.
		 */
		public boolean isValid() {
			return mBaseType.size() != 0;
		}

		/**
		 * Return a new created variable type which is the pointer of this variable
		 * type.
		 * <p>
		 * In internal implementation, it just create a clone of current variable type
		 * with the increase of pointer level by 1.
		 *
		 * @return The new created pointer type of this variable type.
		 */
		public VariableType getPointerOfThis() {
			return new VariableType(mBaseType, mPointerLevel + 1);
		}

	}


	/**
	 * The class represent a single parameter (argument) of function. This class
	 * usually is the member of {@linkplain ExpFct}.
	 */
	public static class ExpFctParam {

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

		public ExpFctParam() {
			mVarType = new VariableType();
			mVarName = "";
			mVarDesc = "";
			mIsInput = true;
		}

	}

	/**
	 * The class represent an export BMap function.
	 */
	public static class ExpFct {

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
		 * {@linkplain ExpFctParam} and represent parameter one by one from left to
		 * right.
		 */
		public Vector<ExpFctParam> mFctParams;

		public ExpFct() {
			mFctName = "";
			mFctRetType = new VariableType();
			mFctParams = new Vector<ExpFctParam>();
		}

	}

	/**
	 * The class represent a collection of export BMap functions.
	 */
	public static class ExpFctCollection {
		/**
		 * The collection of exported BMap functions.
		 */
		public Vector<ExpFct> mFcts;

		public ExpFctCollection() {
			mFcts = new Vector<ExpFct>();
		}
	}

}
