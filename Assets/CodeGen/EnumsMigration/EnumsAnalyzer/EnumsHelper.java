import java.util.Vector;

public class EnumsHelper {
	/**
	 * The struct to describe the entry of an enum.
	 */
	public static class BEnumEntry {
		public BEnumEntry() {
			mEntryName = null;
			mEntryValue = null;
			mEntryComment = null;
		}

		/** The name of this entry. Can not be null. */
		public String mEntryName;
		/** The value of this entry. null if this entry do not have explicit value. */
		public String mEntryValue;
		/** The comment of this entry. null if no comment. */
		public String mEntryComment;
	}
	
	/**
	 * The specialized EnumEntry type which can store extra hierarchy info.
	 * Used in CK_CLASSID parsing.
	 */
	public static class BHierarchyEnumEntry extends BEnumEntry {
		public BHierarchyEnumEntry() {
			super();
			mHierarchy = new Vector<BHierarchyEnumEntry>();
		}
		
		/** The list to store this CK_CLASSID inheritance relationship. */
		public Vector<BHierarchyEnumEntry> mHierarchy;
	}
	
	/**
	 * The struct to describe an enum.
	 */
	public static class BEnum {
		public BEnum() {
			mEnumName = null;
			mEnumComment = null;
			mCanUnsigned = true;
			mUseFlags = false;
			mEntries = new Vector<BEnumEntry>();
		}

		/** The name of this enum. Can not be null. */
		public String mEnumName;
		/** The comment of this enum. null if no comment. */
		public String mEnumComment;
		/** True if this enum can use unsigned integer as its underlying type. */
		public boolean mCanUnsigned;
		/** True if this enum will use flags feature (supporting OR, AND, operators). */
		public boolean mUseFlags;
		/** The list to store entries of this enum. */
		public Vector<BEnumEntry> mEntries;
	}

	/**
	 * The struct to describe a collection of enums.
	 */
	public static class BEnumCollection {
		public BEnumCollection() {
			mEnums = new Vector<BEnum>();
		}

		/** The list to store enums. */
		public Vector<BEnum> mEnums;
	}

}
