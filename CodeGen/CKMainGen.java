
public class CKMainGen {
	public static void main(String[] args) throws Exception {
		CKEnumRunner.run("src/CKENUMS.txt", "src/VXENUMS.txt", "dest/CKEnums.gen.hpp", "dest/AccessibleValue.gen.hpp");
		System.out.println("DONE!");
	}
}
