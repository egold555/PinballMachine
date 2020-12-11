import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.Date;

public class PrintWriterUtil {

	public static class Java {
		public static PrintWriter begin(String name) throws FileNotFoundException {
			File f = new File("C:\\Users\\eric\\Documents\\Projects\\PinballMachine\\Java\\Pinball\\src\\org\\golde\\pinball\\constants\\" + name + ".java");
			if(f.exists()) {
				f.delete();
			}

			PrintWriter pw = new PrintWriter(f);


			pw.println("package org.golde.pinball.constants;");
			pw.println("");
			pw.println("//Generated: " + new Date().toString());
			pw.println("public class " + name + " {");
			pw.println("");

			return pw;
		}

		public static PrintWriter writeIntConstant(PrintWriter pw, String name, int value) {
			pw.println("	public static final int " + name + " = " + value + ";");
			return pw;
		}

		public static void end(PrintWriter pw) {
			pw.println("}");
			endPW(pw);
		}
	}


	public static class H {
		public static PrintWriter begin(String name) throws FileNotFoundException {
			File f = new File("C:\\Users\\eric\\Documents\\Projects\\PinballMachine\\Arduino\\" + name + ".h");
			if(f.exists()) {
				f.delete();
			}

			PrintWriter pw = new PrintWriter(f);
			pw.println("//Generated: " + new Date().toString());
			pw.println("");

			return pw;
		}

		public static PrintWriter writeIntConstant(PrintWriter pw, String name, int value) {
			pw.println("#define " + name + " " + value);
			return pw;
		}

		public static void end(PrintWriter pw) {
			endPW(pw);
		}
	}

	private static void endPW(PrintWriter pw) {

		pw.flush();
		pw.close();
	}

}
