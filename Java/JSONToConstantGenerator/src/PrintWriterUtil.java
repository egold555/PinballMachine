import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

public class PrintWriterUtil {
	
	public static class HInt extends PrintWriterTemplate {

		public HInt(String fileName) {
			super(fileName);
		}

		@Override
		protected String getFilePath(String fileName) {
			return "C:\\Users\\eric\\Documents\\Projects\\PinballMachine\\Arduino\\Protocol" + fileName + ".h";
		}

		@Override
		protected String getConstantFormatting(String name, int id) {
			return "#define " + name + " " + id;
		}

		@Override
		protected void getBegining(String name, List<String> l) {
			l.add("//Generated: " + new Date().toString());
			l.add("");
		}

		@Override
		protected void getEnd(String name, List<String> l) {}
		
	}
	
	
	public static class JavaEnumInt extends PrintWriterTemplate {

		public JavaEnumInt(String fileName) {
			super(fileName);
		}

		@Override
		protected String getFilePath(String fileName) {
			return "C:\\Users\\eric\\Documents\\Projects\\PinballMachine\\Java\\Pinball\\src\\org\\golde\\pinball\\constants\\" + fileName + ".java";
		}

		@Override
		protected String getConstantFormatting(String name, int id) {
			return "	" + name + "(" + id + "),";
		}

		@Override
		protected void getBegining(String name, List<String> l) {
			l.add("package org.golde.pinball.constants;");
			l.add("");
			l.add("//Generated: " + new Date().toString());
			l.add("public enum " + name + " {");
			l.add("");
		}

		@Override
		protected void getEnd(String name, List<String> l) {
			l.add("	;");
			l.add("");
			l.add("	private final int id;");
			l.add("	" + name + "(int id) {");
			l.add("		this.id = id;");
			l.add("	}");
			l.add("");
			l.add("	public int getId() {");
			l.add("		return id;");
			l.add("	}");
			l.add("");
			l.add("}");
		}
		
	}
	
	public static abstract class PrintWriterTemplate {
		
		private PrintWriter pw;
		private final String fileName;
		
		public PrintWriterTemplate(String fileName) {
			this.fileName = fileName;
		}
		
		protected abstract String getFilePath(String fileName);
		protected abstract String getConstantFormatting(String name, int id);
		protected abstract void getBegining(String name, List<String> l);
		protected abstract void getEnd(String name, List<String> l);
		
		public final void begin() throws FileNotFoundException {
			File f = new File(getFilePath(fileName));
			if(f.exists()) {
				f.delete();
			}

			pw = new PrintWriter(f);

			List<String> temp = new ArrayList<String>();
			getBegining(fileName, temp);
			for(String s : temp) {
				pw.println(s);
			}
			
		}
		
		public final void writeConstant(String name, int id) {
			pw.println(getConstantFormatting(name, id));
		}
		
		public void end() {
			List<String> temp = new ArrayList<String>();
			getEnd(fileName, temp);
			for(String s : temp) {
				pw.println(s);
			}
			pw.flush();
			pw.close();
		}
	}

}
