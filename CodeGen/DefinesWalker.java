
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

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
	
	public EnumsHelper.Enum_t getEnum() {
		return mResult;
	}
	
	private CommentsFinder mCommentsFinder;
	private EnumsHelper.Enum_t mResult;

	private EnumsHelper.Enum_t mCurrentEnum;
	private EnumsHelper.EnumEntry_t mCurrentEntry;
	
	@Override
	public void enterProg(CKDefinesParser.ProgContext ctx) {
		mCurrentEnum = new EnumsHelper.Enum_t();
	}

	@Override
	public void exitProg(CKDefinesParser.ProgContext ctx) {
		mResult = mCurrentEnum;
		mCurrentEnum = null;
	}

	@Override
	public void enterDefinePair(CKDefinesParser.DefinePairContext ctx) {
		mCurrentEntry = new EnumsHelper.EnumEntry_t();
	}

	@Override
	public void exitDefinePair(CKDefinesParser.DefinePairContext ctx) {
		// set values
		mCurrentEntry.mEntryName = ctx.CKGENERAL_ID(0).getText();
		mCurrentEntry.mEntryComment = mCommentsFinder.getComment(ctx.getStart(), ctx.getStop());
		
		if (ctx.CKGENERAL_NUM() == null) {
			// define with id
			mCurrentEntry.mEntryValue = ctx.CKGENERAL_ID(1).getText();
			
		} else {
			// define with number
			String num = ctx.CKGENERAL_NUM().getText();
			mCurrentEntry.mEntryValue = num;

			// check whether this enum can be unsigned
			if (CommonHelper.isNegativeNumber(num)) {
				mCurrentEnum.mCanUnsigned = false;
			}
			// if the number is in hex form, this enum MIGHT have flags feature
			if (CommonHelper.isHexNumber(num)) {
				mCurrentEnum.mUseFlags = true;
			}

		}
		// add entry
		mCurrentEnum.mEntries.add(mCurrentEntry);
		mCurrentEntry = null;
	}
	
}
