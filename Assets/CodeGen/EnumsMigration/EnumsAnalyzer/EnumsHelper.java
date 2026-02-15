import java.util.Vector;

public class EnumsHelper {

	/**
	 * The kind of enum entry value.
	 * This kind indicates whether this enum entry belong to a flag enum.
	 */
	public enum BEnumEntryFlagKind {
		/**
		 * This enum entry can not belong to a flag enum.
		 * Because its value is ordinary.
		 */
		NotFlag,
		/**
		 * This enum entry may belong to a flag enum.
		 * Because its value is in HEX format, and refering other members.
		 */
		MayFlag,
		/**
		 * This enum entry must belong to a flag enum.
		 * Because its value use bitwise operation.
		 */
		MustFlag,
	}

	/**
	 * The kind of enum entry value.
	 * This kind indicates the sign of this enum entry value.
	 */
	public enum BEnumEntrySignKind {
		/** The value of this enum entry is positive number or zero. */
		Positive,
		/** The value of this enum entry is negative number. */
		Negative,
		/**
		 * The value of this enum entry is unknown.
		 * This is may be caused by that it refer other memeber.
		 */
		Unknown,
	}

	/**
	 * The struct to describe the entry of an enum.
	 */
	public static class BEnumEntry {
		public BEnumEntry() {
			mEntryName = null;
			mEntryValue = null;
			mEntryFlagKind = null;
			mEntrySignKind = null;
			mEntryComment = null;
		}

		/** The name of this entry. Can not be null. */
		public String mEntryName;
		/** The value of this entry. null if this entry do not have explicit value. */
		public String mEntryValue;
		/** The flag kind of this entry value. */
		public BEnumEntryFlagKind mEntryFlagKind;
		/** The sign kind of this entry value. */
		public BEnumEntrySignKind mEntrySignKind;
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
		
		/**
		 * The list to store this CK_CLASSID inheritance relationship.
		 * The first item is the oldest parent in inheritance.
		 * The last item is self.
		 */
		public Vector<BHierarchyEnumEntry> mHierarchy;
	}
	
	/**
	 * The struct to describe an enum.
	 */
	public static class BEnum {
		public BEnum() {
			mEnumName = null;
			mEnumComment = null;
			mIsUnsigned = true;
			mIsFlag = false;
			mEntries = new Vector<BEnumEntry>();
		}

		/** The name of this enum. Can not be null. */
		public String mEnumName;
		/** The comment of this enum. null if no comment. */
		public String mEnumComment;
		/** True if this enum should use unsigned integer as its underlying type, otherwise false. */
		public boolean mIsUnsigned;
		/** True if this enum shoule have flags feature (supporting OR, AND, operators), otherwise false. */
		public boolean mIsFlag;
		/** The list to store entries of this enum. */
		public Vector<BEnumEntry> mEntries;

		/**
		 * Update some properties located in this class according to existing entries.
		 */
		public void updateByEntries() {
			// If there is at least one negative entry, the enum should be signed,
			// Otherwise, it is unsigned.
			// For unknown entries, ignore them.
			boolean has_negative = false;
			for (BEnumEntry entry : this.mEntries) {
				if (entry.mEntrySignKind == BEnumEntrySignKind.Negative) {
					has_negative = true;
				}
			}
			this.mIsUnsigned = !has_negative;

			// For flag kind, if there is "Must Flag" entry, the enum should be a flag enum.
			// Then, if "May Flag" entry is more than "Not Flag", the enum would be a flag enum.
			// Otherwise, it is not flag.
			boolean has_must_flag = false;
			int cnt_may_flag = 0, cnt_not_flag = 0;
			for (BEnumEntry entry : this.mEntries) {
				switch (entry.mEntryFlagKind) {
					case NotFlag -> ++cnt_not_flag;
					case MayFlag -> ++cnt_may_flag;
					case MustFlag -> has_must_flag = true;
				}
			}
			if (has_must_flag) {
				this.mIsFlag = true;
			} else if (cnt_may_flag > cnt_not_flag) {
				this.mIsFlag = true;
			} else {
				this.mIsFlag = false;
			}
		}
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
