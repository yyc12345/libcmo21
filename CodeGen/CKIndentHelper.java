import java.io.OutputStreamWriter;

public class CKIndentHelper {
	public CKIndentHelper(OutputStreamWriter writer) {
		mIndent = 0;
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
	
	public void briefComment(String fmt) throws Exception {
		if (fmt == null) return;
		printf("/**< {} */", fmt.replaceAll("[\\r\\n]+", ""));
	}
	
	public void afterMemberComment(String fmt) throws Exception {
		if (fmt == null) return;
		mWriter.write("\t");
		mWriter.write(fmt);
	}

}
