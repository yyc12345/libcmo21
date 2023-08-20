import java.io.OutputStreamWriter;

public class IndentHelper {
	public IndentHelper(OutputStreamWriter writer) {
		mIndent = 0;
		mWriter = writer;
	}

	private int mIndent;
	private OutputStreamWriter mWriter;

	public void inc() {
		++mIndent;
	}

	public void dec() {
		--mIndent;
	}

	public void indent() throws Exception {
		mWriter.write("\n");
		for (int i = 0; i < mIndent; ++i) {
			mWriter.write("\t");
		}
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
	 * @param comment
	 * @throws Exception
	 */
	public void briefComment(String comment) throws Exception {
		if (comment == null)
			return;
		puts("/**");
		puts(comment);
		puts(" */");
	}

	/**
	 * Write suffix style Doxygen document comment.
	 * <p>
	 * Usually be called after writing content.
	 * @param comment
	 * @throws Exception
	 */
	public void afterMemberComment(String comment) throws Exception {
		if (comment == null)
			return;
		mWriter.write(String.format("\t/**< %s */", CommonHelper.removeEol(comment)));
	}

}
