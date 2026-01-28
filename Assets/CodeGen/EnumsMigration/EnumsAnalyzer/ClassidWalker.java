
import java.util.Stack;

import org.antlr.v4.runtime.*;

/**
 * The specialized walker for collecting CK_CLASSID and its inherit
 * relationship.
 */
public class ClassidWalker extends CKDefinesParserBaseListener {

	public ClassidWalker(BufferedTokenStream tokenStream) {
		mTokenStream = tokenStream;
		mResult = null;

		mLevelStack = null;
		mCurrentEnum = null;
		mCurrentEntry = null;
	}

	public EnumsHelper.BEnum getEnum() {
		return mResult;
	}

	private int getClassidLevel(Token defineHead) {
		Token ws = CommonHelper.getPreChannelToken(mTokenStream, defineHead, CKGenericLexer.WHITESPACE);
		if (ws == null)
			return 0;

		// reverse finding how many tab used.
		int counter = 0;
		char[] wstxt = ws.getText().toCharArray();
		for (int i = wstxt.length - 1; i >= 0; i--) {
			if (wstxt[i] == '\t') {
				counter++;
			} else {
				break;
			}
		}
		return counter;
	}

	private void safePop() {
		if (mLevelStack.size() != 0)
			mLevelStack.pop();
	}

	private void safePopTimes(int times) {
		for (int i = 0; i < times; i++) {
			if (mLevelStack.size() != 0)
				mLevelStack.pop();
		}
	}

	private BufferedTokenStream mTokenStream;
	private EnumsHelper.BEnum mResult;

	private int mLevel;
	private Stack<EnumsHelper.BHierarchyEnumEntry> mLevelStack;
	private EnumsHelper.BEnum mCurrentEnum;
	private EnumsHelper.BHierarchyEnumEntry mCurrentEntry;

	@Override
	public void enterProg(CKDefinesParser.ProgContext ctx) {
		mLevel = 0;
		mLevelStack = new Stack<EnumsHelper.BHierarchyEnumEntry>();
		mCurrentEnum = new EnumsHelper.BEnum();
	}

	@Override
	public void exitProg(CKDefinesParser.ProgContext ctx) {
		mLevel = 0;
		mLevelStack = null;

		// classid is signed int and do not have flags feature.
		mCurrentEnum.mCanUnsigned = false;
		mCurrentEnum.mUseFlags = false;
		mResult = mCurrentEnum;
		mCurrentEnum = null;
	}

	@Override
	public void enterDefinePair(CKDefinesParser.DefinePairContext ctx) {
		mCurrentEntry = new EnumsHelper.BHierarchyEnumEntry();
	}

	@Override
	public void exitDefinePair(CKDefinesParser.DefinePairContext ctx) {
		// fill entry info
		mCurrentEntry.mEntryName = ctx.CKGENERIC_ID(0).getText();
		mCurrentEntry.mEntryValue = ctx.CKGENERIC_NUM().getText();

		// fill entry level info
		int this_level = getClassidLevel(ctx.getStart());
		if (this_level > mLevel) {
			// level up
			mLevel++;
			
			mLevelStack.push(mCurrentEntry);
			mCurrentEntry.mHierarchy.addAll(mLevelStack);
		} else if (this_level == mLevel) {
			safePop();
			mLevelStack.push(mCurrentEntry);
			mCurrentEntry.mHierarchy.addAll(mLevelStack);
		} else if (this_level < mLevel) {
			// level down
			safePopTimes(mLevel - this_level + 1);
			mLevel = this_level;
			
			mLevelStack.push(mCurrentEntry);
			mCurrentEntry.mHierarchy.addAll(mLevelStack);
		}
		
		// move to list
		mCurrentEnum.mEntries.add(mCurrentEntry);
		mCurrentEntry = null;
	}

}
