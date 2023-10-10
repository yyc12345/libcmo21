import java.io.OutputStreamWriter;

public class IndentHelper {
	public IndentHelper(OutputStreamWriter writer, CommonHelper.LangType lang_type) {
		mIndent = 0;
		mLangType = lang_type;
		mWriter = writer;
	}

	private int mIndent;
	private CommonHelper.LangType mLangType;
	private OutputStreamWriter mWriter;

	private void rawIndent() throws Exception {
		for (int i = 0; i < mIndent; ++i) {
			switch (mLangType) {
			case CPP:
				mWriter.write("\t");
				break;
			case Python:
				mWriter.write("    ");
				break;
			}
		}
	}
	
	public void inc() {
		++mIndent;
	}

	public void dec() {
		--mIndent;
	}

	public void indent() throws Exception {
		mWriter.write("\n");
		rawIndent();
	}

	public void puts(String data) throws Exception {
		indent();
		mWriter.write(data);
	}

	public void printf(String fmt, Object... args) throws Exception {
		indent();
		mWriter.write(String.format(fmt, args));
	}

	/**
	 * Write standard Doxygen document comment.
	 * <p>
	 * Usually be called before writing content.
	 * 
	 * @param comment
	 * @throws Exception
	 */
	public void briefComment(String comment) throws Exception {
		if (comment == null)
			return;
		
		switch (mLangType) {
		case CPP:
			rawIndent();
			puts("/**");

			puts(comment);

			rawIndent();
			puts(" */");
			break;
		case Python:
			rawIndent();
			puts("\"\"\"!");
			
			puts(comment);

			rawIndent();
			puts("\"\"\"");
			break;
		}
	}

	/**
	 * Write suffix style Doxygen document comment.
	 * <p>
	 * Usually be called after writing content.
	 * 
	 * @param comment
	 * @throws Exception
	 */
	public void afterMemberComment(String comment) throws Exception {
		if (comment == null)
			return;

		switch (mLangType) {
		case CPP:
			mWriter.write(String.format("\t/**< %s */", CommonHelper.removeEol(comment)));
			break;
		case Python:
			mWriter.write(String.format("    ##< %s", CommonHelper.removeEol(comment)));
			break;
		}
		
	}

}
