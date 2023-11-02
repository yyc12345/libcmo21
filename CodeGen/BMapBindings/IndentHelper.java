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

	private void indent() throws Exception {
		for (int i = 0; i < mIndent; ++i) {
			mWriter.write("    ");
		}
	}
	
	private void lineBreak() throws Exception {
		mWriter.write(System.lineSeparator());
	}

	public void puts(String data) throws Exception {
		indent();
		mWriter.write(data);
		lineBreak();
	}

	public void printf(String fmt, Object... args) throws Exception {
		indent();
		mWriter.write(String.format(fmt, args));
		lineBreak();
	}

}
