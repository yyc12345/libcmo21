import java.io.OutputStreamWriter;
import java.util.List;
import java.util.Vector;
import java.util.stream.Collectors;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

public class CKEnumRunner {
	public static class EnumEntry_t {
		public EnumEntry_t() {
			mEntryName = null;
			mEntryValue = null;
			mEntryComment = null;
		}

		public String mEntryName;
		public String mEntryValue; // setting to null mean this entry do not have specified value.
		public String mEntryComment;
	}

	public static class Enum_t {
		public Enum_t() {
			mEnumName = null;
			mEnumComment = null;
			mCanUnsigned = true;
			mEntries = new Vector<EnumEntry_t>();
		}

		public String mEnumName;
		public String mEnumComment;
		public boolean mCanUnsigned;
		public Vector<EnumEntry_t> mEntries;
	}

	public static class EnumWalker extends CKEnumParserBaseListener {
		public EnumWalker(BufferedTokenStream tokenStream) {
			mTokenStream = tokenStream;
			mCommentsHelper = new CKEnumCommentsHelper(tokenStream);
			mResult = null;

			mCurrentProg = null;
			mCurrentEnum = null;
			mCurrentEntry = null;
		}

		public List<Enum_t> getResult() {
			return mResult;
		}

		private BufferedTokenStream mTokenStream;
		private CKEnumCommentsHelper mCommentsHelper;
		private Vector<Enum_t> mResult;

		private Vector<Enum_t> mCurrentProg;
		private Enum_t mCurrentEnum;
		private EnumEntry_t mCurrentEntry;

		@Override
		public void enterProg(CKEnumParser.ProgContext ctx) {
			mCurrentProg = new Vector<Enum_t>();
		}

		@Override
		public void exitProg(CKEnumParser.ProgContext ctx) {
			mResult = mCurrentProg;
			mCurrentProg = null;
		}

		@Override
		public void enterEnumBody(CKEnumParser.EnumBodyContext ctx) {
			mCurrentEnum = new Enum_t();
		}

		@Override
		public void exitEnumBody(CKEnumParser.EnumBodyContext ctx) {
			// get enum comment
			mCurrentEnum.mEnumComment = CKCommonHelper.cutComment(
					CKCommonHelper.getPreChannelToken(mTokenStream, ctx.getStart(), CKGeneralLexer.COMMENTS));
			// get the last name (for typedef case)
			List<TerminalNode> allNames = ctx.CKGENERAL_ID();
			mCurrentEnum.mEnumName = allNames.get(allNames.size() - 1).getText();

			mCurrentProg.add(mCurrentEnum);
			mCurrentEnum = null;
		}

		@Override
		public void enterEntryPair(CKEnumParser.EntryPairContext ctx) {
			mCurrentEntry = new EnumEntry_t();
		}

		@Override
		public void exitEntryPair(CKEnumParser.EntryPairContext ctx) {
			// get entry comment
			mCurrentEntry.mEntryComment = mCommentsHelper.getComment(ctx.getStart(), ctx.getStop());
			// get entry name
			mCurrentEntry.mEntryName = ctx.CKGENERAL_ID().getText();

			mCurrentEnum.mEntries.add(mCurrentEntry);
			mCurrentEntry = null;
		}

		@Override
		public void exitEntryDirectValue(CKEnumParser.EntryDirectValueContext ctx) {
			// get all numbers
			List<TerminalNode> nums = ctx.CKGENERAL_NUM();

			switch (nums.size()) {
			case 1: {
				TerminalNode node = nums.get(0);
				// check whether target is minus number
				if (CKCommonHelper.isNegtiveNumber(node.getText())) {
					mCurrentEnum.mCanUnsigned = false;
				}
				// set value
				mCurrentEntry.mEntryValue = node.getText();

				break;
			}
			case 2: {
				TerminalNode num = nums.get(0), offset = nums.get(1);
				// set value
				mCurrentEntry.mEntryValue = String.format("{} << {}", num.getText(), offset.getText());

				break;
			}
			default:
				throw new IllegalArgumentException("Unexpected value: " + nums.size());
			}
		}

		@Override
		public void exitEntryRelativeValue(CKEnumParser.EntryRelativeValueContext ctx) {
			// get all identifiers and join them
			mCurrentEntry.mEntryValue = String.join(" | ",
					ctx.CKGENERAL_ID().stream().map(value -> value.getText()).collect(Collectors.toList()));
		}

	}

	public static class EnumProgWriter {
		public static void writeEnums(OutputStreamWriter writer, CKCommonHelper.CKParts parts, List<Enum_t> prog)
				throws Exception {
			CKIndentHelper indent = new CKIndentHelper(writer);
			indent.puts("#pragma once");
			indent.puts("#include <cstdint>");
			indent.printf("namespace LibCmo::{} {{", CKCommonHelper.getCKPartsNamespace(parts));
			indent.inc();

			// write enums
			for (Enum_t enum_t : prog) {
				// write enum comment
				indent.briefComment(enum_t.mEnumComment);

				// write enum start
				indent.printf("enum class {} : {} {{", enum_t.mEnumName,
						CKCommonHelper.getEnumUnderlayingType(enum_t.mCanUnsigned));
				indent.inc();

				// write enum entries
				for (EnumEntry_t enumEntry_t : enum_t.mEntries) {
					// write entry self
					if (enumEntry_t.mEntryValue == null) {
						indent.printf("{},", enumEntry_t.mEntryName);
					} else {
						indent.printf("{} = {},", enumEntry_t.mEntryName, enumEntry_t.mEntryValue);
					}
					
					// write entry comment after member
					indent.afterMemberComment(enumEntry_t.mEntryComment);
				}

				// write enum tail
				indent.dec();
				indent.puts("}");
			}

			indent.dec();
			indent.puts("}");
		}

		public static void writeComments(OutputStreamWriter writer, List<Enum_t> prog) throws Exception {
			CKIndentHelper indent = new CKIndentHelper(writer);

		}
	}

	public static void Run(String infilename, String outfilename) {

	}
}
