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

	public EnumsHelper.BEnumCollection getEnums() {
		return mResult;
	}

	private String getEnumComment(Token enumHead) {
		return CommonHelper
				.cutComments(CommonHelper.getPreChannelTokens(mTokenStream, enumHead, CKGenericLexer.COMMENTS));
	}

	private BufferedTokenStream mTokenStream;
	private CommentsFinder mCommentsFinder;
	private EnumsHelper.BEnumCollection mResult;

	private EnumsHelper.BEnumCollection mCurrentProg;
	private EnumsHelper.BEnum mCurrentEnum;
	private EnumsHelper.BEnumEntry mCurrentEntry;

	@Override
	public void enterProg(CKEnumsParser.ProgContext ctx) {
		mCurrentProg = new EnumsHelper.BEnumCollection();
	}

	@Override
	public void exitProg(CKEnumsParser.ProgContext ctx) {
		mResult = mCurrentProg;
		mCurrentProg = null;
	}

	@Override
	public void enterEnumBody(CKEnumsParser.EnumBodyContext ctx) {
		mCurrentEnum = new EnumsHelper.BEnum();
	}

	@Override
	public void exitEnumBody(CKEnumsParser.EnumBodyContext ctx) {
		// get enum comment
		mCurrentEnum.mEnumComment = getEnumComment(ctx.getStart());
		// get the last name (for typedef case)
		List<TerminalNode> allNames = ctx.CKGENERIC_ID();
		mCurrentEnum.mEnumName = allNames.get(allNames.size() - 1).getText();

		// update self and add into list
		mCurrentEnum.updateByEntries();
		mCurrentProg.mEnums.add(mCurrentEnum);
		mCurrentEnum = null;
	}

	@Override
	public void enterEntryPair(CKEnumsParser.EntryPairContext ctx) {
		mCurrentEntry = new EnumsHelper.BEnumEntry();
	}

	@Override
	public void exitEntryPair(CKEnumsParser.EntryPairContext ctx) {
		// get entry comment
		mCurrentEntry.mEntryComment = mCommentsFinder.getComment(ctx.getStart(), ctx.getStop());
		// get entry name
		mCurrentEntry.mEntryName = ctx.CKGENERIC_ID().getText();

		// if its value is null, we manually fill 2 kinds
		if (mCurrentEntry.mEntryValue == null) {
			// the sign kind is unknown because it relys on other value (+1)
			mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Unknown;
			// because it just adds one from previous member, it should not belong to a flag enum
			mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.NotFlag;
		}

		mCurrentEnum.mEntries.add(mCurrentEntry);
		mCurrentEntry = null;
	}

	@Override
	public void exitEntryDirectValue(CKEnumsParser.EntryDirectValueContext ctx) {
		// get all numbers
		List<TerminalNode> nums = ctx.CKGENERIC_NUM();

		switch (nums.size()) {
			case 1: {
				// value is immediate number
				TerminalNode node = nums.get(0);
				String num = node.getText();
				mCurrentEntry.mEntryValue = num;

				// check whether this enum can be unsigned
				if (CommonHelper.isNegativeNumber(num)) {
					mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Negative;
				} else {
					mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Positive;
				}
				// if the number is in hex form, this entry may belong to flag enum
				if (CommonHelper.isHexNumber(num)) {
					mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.MayFlag;
				} else {
					mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.NotFlag;
				}

				break;
			}
			case 2: {
				// value is bitwise operation
				TerminalNode num = nums.get(0), offset = nums.get(1);
				mCurrentEntry.mEntryValue = String.format("%s << %s", num.getText(), offset.getText());

				// << operator appears.
				// it shoud be unsigned.
				mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Positive;
				// and it must belong to flag enum
				mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.MustFlag;

				break;
			}
			default:
				throw new IllegalArgumentException("Unexpected value: " + nums.size());
		}

	}

	@Override
	public void exitEntryRelativeValue(CKEnumsParser.EntryRelativeValueContext ctx) {
		// get all identifiers and join them
		mCurrentEntry.mEntryValue = ctx.CKGENERIC_ID().stream().map(value -> value.getText())
				.collect(Collectors.joining(" | "));

		if (ctx.CKGENERIC_ID().size() > 1) {
			// If there is more than one ID, it means | operator appears.
			// It should be unsigned.
			mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Positive;
			// And it must belong to flag enum.
			mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.MustFlag;
		} else {
			// Otherwise it just refer other member.
			// The sign of its value is unclear.
			mCurrentEntry.mEntrySignKind = EnumsHelper.BEnumEntrySignKind.Unknown;
			// And it may belong to flag enum because it refers other memeber.
			mCurrentEntry.mEntryFlagKind = EnumsHelper.BEnumEntryFlagKind.MayFlag;
		}
	}

}