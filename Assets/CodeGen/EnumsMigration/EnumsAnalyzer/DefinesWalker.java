
import org.antlr.v4.runtime.*;

/**
 * The generic walker for collecting defines as a enum.
 */
public class DefinesWalker extends CKDefinesParserBaseListener {
	public DefinesWalker(BufferedTokenStream tokenStream) {
		mCommentsFinder = new CommentsFinder(tokenStream);
		mResult = null;

		mCurrentEnum = null;
		mCurrentEntry = null;
	}
	
	public EnumsHelper.BEnum getEnum() {
		return mResult;
	}
	
	private CommentsFinder mCommentsFinder;
	private EnumsHelper.BEnum mResult;

	private EnumsHelper.BEnum mCurrentEnum;
	private EnumsHelper.BEnumEntry mCurrentEntry;
	
	@Override
	public void enterProg(CKDefinesParser.ProgContext ctx) {
		mCurrentEnum = new EnumsHelper.BEnum();
	}

	@Override
	public void exitProg(CKDefinesParser.ProgContext ctx) {
		// update enum
		mCurrentEnum.updateByEntries();
		// and return
		mResult = mCurrentEnum;
		mCurrentEnum = null;
	}

	@Override
	public void enterDefinePair(CKDefinesParser.DefinePairContext ctx) {
		mCurrentEntry = new EnumsHelper.BEnumEntry();
	}

	@Override
	public void exitDefinePair(CKDefinesParser.DefinePairContext ctx) {
		// set values
		mCurrentEntry.mEntryName = ctx.CKGENERIC_ID(0).getText();
		mCurrentEntry.mEntryComment = mCommentsFinder.getComment(ctx.getStart(), ctx.getStop());
		
		if (ctx.CKGENERIC_NUM() == null) {
			// define with id
			mCurrentEntry.mEntryValue = ctx.CKGENERIC_ID(1).getText();
			// it refers other memeber, so its sign is unknown
			mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Unknown;
			// it refers other memeber, so it may flag.
			mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.MayFlag;
		} else {
			// define with number
			String num = ctx.CKGENERIC_NUM().getText();
			mCurrentEntry.mEntryValue = num;

			// check the sign of this number
			if (CommonHelper.isNegativeNumber(num)) {
				mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Negative;
			} else {
				mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Positive;
			}
			// if the number is in hex form, it may belong to flag enum
			if (CommonHelper.isHexNumber(num)) {
				mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.MayFlag;
			} else {
				mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.NotFlag;
			}

		}
		// add entry
		mCurrentEnum.mEntries.add(mCurrentEntry);
		mCurrentEntry = null;
	}
	
}
