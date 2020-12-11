

import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class FileUtil {

	public static void saveToFile(Object list, String filename) {

		FileWriter writer = null;

		try {
			writer = new FileWriter("res/" + filename + ".json");
			Main.GSON.toJson(list, writer);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		finally {
			if(writer != null) {
				try {
					writer.close();
				} 
				catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

	}

	public static <T> T loadFromFile(String filename, Class<T> clazz) {

		FileReader reader = null;

		try {
			reader = new FileReader("res/" + filename + ".json");
			return Main.GSON.fromJson(reader, clazz);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		finally {
			if(reader != null) {
				try {
					reader.close();
				} 
				catch (IOException e) {
					e.printStackTrace();
				}
			}

		}


		return null;

	}

}
