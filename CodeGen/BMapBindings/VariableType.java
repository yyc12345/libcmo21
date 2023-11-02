import java.util.Collections;

public class VariableType {
	/**
	 * The base type of this variable which remove all ending stars.
	 */
	private String mBaseType;
	/**
	 * The pointer level of this type. It is equal with the count of stars.
	 */
	private int mPointerLevel;

	public VariableType(String ctype) {
		fromCType(ctype);
	}
	
	private VariableType(String base_type, int pointer_level) {
		mBaseType = base_type;
		mPointerLevel = pointer_level;
	}

	public void fromCType(String ctype) {
		if (ctype.isEmpty())
			throw new IllegalArgumentException("empty string can not be parsed.");

		int len = ctype.length();
		int star_pos = ctype.indexOf('*');
		if (star_pos == -1) {
			// no star
			mBaseType = ctype;
			mPointerLevel = 0;
		} else {
			// has star
			if (star_pos == 0)
				throw new IllegalArgumentException("base type not found.");
			mBaseType = ctype.substring(0, star_pos);
			mPointerLevel = len - star_pos;
		}
	}

	public String toCType() {
		return mBaseType + String.join("", Collections.nCopies(mPointerLevel, "*"));
	}

	public String getBaseType() {
		return mBaseType;
	}

	public boolean isPointer() {
		return mPointerLevel != 0;
	}

	public VariableType getPointerOfThis() {
		return new VariableType(mBaseType, mPointerLevel);
	}
	
}
