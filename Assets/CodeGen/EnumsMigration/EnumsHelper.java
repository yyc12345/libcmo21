import java.util.Vector;

public class EnumsHelper {
	/**
	 * The struct to describe the entry of an enum.
	 */
	public static class EnumEntry_t {
		public EnumEntry_t() {
			mEntryName = null;
			mEntryValue = null;
			mEntryComment = null;
		}

		public String mEntryName; ///< The name of this entry. Can not be null.
		public String mEntryValue; ///< The value of this entry. null if this entry do not have explicit value.
		public String mEntryComment; ///< The comment of this entry. null if no comment.
	}
	
	/**
	 * The specialized EnumEntry type which can store extra hierarchy info.
	 * Used in CK_CLASSID parsing.
	 */
	public static class EnumEntryWithHierarchy_t extends EnumEntry_t {
		public EnumEntryWithHierarchy_t() {
			super();
			mHierarchy = new Vector<EnumEntryWithHierarchy_t>();
		}
		
		public Vector<EnumEntryWithHierarchy_t> mHierarchy; ///< The list to store this CK_CLASSID inheritance relationship.
	}
	
	/**
	 * The struct to describe an enum.
	 */
	public static class Enum_t {
		public Enum_t() {
			mEnumName = null;
			mEnumComment = null;
			mCanUnsigned = true;
			mUseFlags = false;
			mEntries = new Vector<EnumEntry_t>();
		}

		public String mEnumName; ///< The name of this enum. Can not be null.
		public String mEnumComment; ///< The comment of this enum. null if no comment.
		public boolean mCanUnsigned; ///< true if this enum can use unsigned integer as its underlaying type.
		public boolean mUseFlags; ///< true if this enum will have flags feature (supporting OR, AND, operators).
		public Vector<EnumEntry_t> mEntries; ///< The list to store entries of this enum.
	}

	/**
	 * The struct to describe a collection of enums.
	 */
	public static class EnumCollection_t {
		public EnumCollection_t() {
			mEnums = new Vector<Enum_t>();
		}

		public Vector<Enum_t> mEnums; ///< The list to store enums.
	}

}
