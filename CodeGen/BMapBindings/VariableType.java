import java.util.Collections;
import java.util.Vector;
import java.util.stream.Collectors;

public class VariableType {
	/**
	 * The base type of this variable which remove all ending stars. Each item is a
	 * part of namespace string. If no namespace, this Vector will only have one
	 * item.
	 */
	private Vector<String> mBaseType;
	/**
	 * The pointer level of this type. It is equal with the count of stars.
	 */
	private int mPointerLevel;

	public VariableType() {
		mBaseType = new Vector<String>();
		mPointerLevel = 0;
	}

	private VariableType(Vector<String> base_type, int pointer_level) {
		mBaseType = (Vector<String>) base_type.clone();
		mPointerLevel = pointer_level;
	}

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

	public String toCType() {
		return mBaseType.stream().collect(Collectors.joining("::"))
				+ String.join("", Collections.nCopies(mPointerLevel, "*"));
	}

	public String getBaseType() {
		return mBaseType.lastElement();
	}

	public boolean isPointer() {
		return mPointerLevel != 0;
	}
	
	public int getPointerLevel() {
		return mPointerLevel;
	}
	
	public Vector<String> getBaseTypeHierarchy() {
		return (Vector<String>) mBaseType.clone();
	}

	public boolean isValid() {
		return mBaseType.size() != 0;
	}

	public VariableType getPointerOfThis() {
		return new VariableType(mBaseType, mPointerLevel + 1);
	}

}
