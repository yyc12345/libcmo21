import java.util.List;
import java.util.stream.Collectors;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

/**
 * The generic walker for collecting multiple enums.
 */
public class EnumsWalker extends CKEnumsParserBaseListener {
	public EnumsWalker(BufferedTokenStream tokenStream) {
		mTokenStream = tokenStream;
		mCommentsFinder = new CommentsFinder(tokenStream);
		mResult = null;

		mCurrentProg = null;
		mCurrentEnum = null;
		mCurrentEntry = null;
	}

	public EnumsHelper.EnumCollection_t getEnums() {
		return mResult;
	}

	private String getEnumComment(Token enumHead) {
		return CommonHelper
				.cutComment(CommonHelper.getPreChannelToken(mTokenStream, enumHead, CKGeneralLexer.COMMENTS));
	}

	private BufferedTokenStream mTokenStream;
	private CommentsFinder mCommentsFinder;
	private EnumsHelper.EnumCollection_t mResult;

	private EnumsHelper.EnumCollection_t mCurrentProg;
	private EnumsHelper.Enum_t mCurrentEnum;
	private EnumsHelper.EnumEntry_t mCurrentEntry;

	@Override
	public void enterProg(CKEnumsParser.ProgContext ctx) {
		mCurrentProg = new EnumsHelper.EnumCollection_t();
	}

	@Override
	public void exitProg(CKEnumsParser.ProgContext ctx) {
		mResult = mCurrentProg;
		mCurrentProg = null;
	}

	@Override
	public void enterEnumBody(CKEnumsParser.EnumBodyContext ctx) {
		mCurrentEnum = new EnumsHelper.Enum_t();
	}

	@Override
	public void exitEnumBody(CKEnumsParser.EnumBodyContext ctx) {
		// get enum comment
		mCurrentEnum.mEnumComment = getEnumComment(ctx.getStart());
		// get the last name (for typedef case)
		List<TerminalNode> allNames = ctx.CKGENERAL_ID();
		mCurrentEnum.mEnumName = allNames.get(allNames.size() - 1).getText();

		mCurrentProg.mEnums.add(mCurrentEnum);
		mCurrentEnum = null;
	}

	@Override
	public void enterEntryPair(CKEnumsParser.EntryPairContext ctx) {
		mCurrentEntry = new EnumsHelper.EnumEntry_t();
	}

	@Override
	public void exitEntryPair(CKEnumsParser.EntryPairContext ctx) {
		// get entry comment
		mCurrentEntry.mEntryComment = mCommentsFinder.getComment(ctx.getStart(), ctx.getStop());
		// get entry name
		mCurrentEntry.mEntryName = ctx.CKGENERAL_ID().getText();

		mCurrentEnum.mEntries.add(mCurrentEntry);
		mCurrentEntry = null;
	}

	@Override
	public void exitEntryDirectValue(CKEnumsParser.EntryDirectValueContext ctx) {
		// get all numbers
		List<TerminalNode> nums = ctx.CKGENERAL_NUM();

		switch (nums.size()) {
		case 1: {
			// set value
			TerminalNode node = nums.get(0);
			mCurrentEntry.mEntryValue = node.getText();

			// check whether this enum can be unsigned
			if (CommonHelper.isNegativeNumber(node.getText())) {
				mCurrentEnum.mCanUnsigned = false;
			}
			// if the number is in hex form, this enum MIGHT have flags feature
			if (CommonHelper.isHexNumber(node.getText())) {
				mCurrentEnum.mUseFlags = true;
			}

			break;
		}
		case 2: {
			// set value
			TerminalNode num = nums.get(0), offset = nums.get(1);
			mCurrentEntry.mEntryValue = String.format("{} << {}", num.getText(), offset.getText());

			// << operator appears. this enum must have flags feature
			mCurrentEnum.mUseFlags = true;

			break;
		}
		default:
			throw new IllegalArgumentException("Unexpected value: " + nums.size());
		}

	}

	@Override
	public void exitEntryRelativeValue(CKEnumsParser.EntryRelativeValueContext ctx) {
		// get all identifiers and join them
		mCurrentEntry.mEntryValue = ctx.CKGENERAL_ID().stream().map(value -> value.getText())
				.collect(Collectors.joining(" | "));

		// | operator appears. this enum must have flags feature
		mCurrentEnum.mUseFlags = true;

	}

}