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
		protected String getConstantFormatting(String name, Object id) {
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
		protected String getConstantFormatting(String name, Object id) {
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
			l.add("	private final Object id;");
			l.add("	" + name + "(Object id) {");
			l.add("		this.id = id;");
			l.add("	}");
			l.add("");
			l.add("	public Object getId() {");
			l.add("		return id;");
			l.add("	}");
			l.add("");
			l.add("	public static " + name + " get(Object o) {");
			l.add("		for(" + name + " i : values()) {");
			l.add("			if(i.id.equals(o)) {");
			l.add("				return i;");
			l.add("			}");
			l.add("		}");
			l.add("		System.err.println(\"Failed to get " + name + " with id of: \" + o);");
			l.add("		return null;");
			l.add("	}");
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
		protected abstract String getConstantFormatting(String name, Object id);
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
		
		public final void writeConstant(String name, Object id) {
			if(id instanceof String) {
				id = "\"" + id + "\"";
			}
			if(id instanceof Double) {
				id = ((Double)id).intValue();
			}
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
