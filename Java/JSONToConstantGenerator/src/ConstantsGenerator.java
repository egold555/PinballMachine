import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.reflect.TypeToken;

public class ConstantsGenerator implements Runnable {

	private static final String FILE_NAME = "Protocol";
	
	@SuppressWarnings("unchecked")
	@Override
	public void run() {
	
		
//		//Protocol Ids
//		HashMap<String, Double> ids = FileUtil.loadFromFile("ProtocolIDs", HashMap.class);
//		
//		
//		try {
//			
//			PrintWriter pwJava = PrintWriterUtil.Java.begin("ProtocolIDs");
//			PrintWriter pwH = PrintWriterUtil.H.begin("ProtocolIDs");
//			ids = (HashMap<String, Double>) sortByValue(ids);
//			for(String key : ids.keySet()) {
//				int value = ids.get(key).intValue();
//				PrintWriterUtil.Java.writeIntConstant(pwJava, key, value);
//				PrintWriterUtil.H.writeIntConstant(pwH, key, value);
//			}
//			
//			PrintWriterUtil.Java.end(pwJava);
//			PrintWriterUtil.H.end(pwH);
//		}
//		catch(FileNotFoundException e) {
//			e.printStackTrace();
//		}
		
		try {
			intConstants("Switches", "PID_SW_");
			intConstants("Lights", "PID_LT_");
			intConstants("Solinoids", "PID_SN_");
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	private void intConstants(String name, String prefix) throws FileNotFoundException {
		
		JsonObject obj = FileUtil.loadFromFile(FILE_NAME, JsonObject.class);
		Map<String, Integer> mapObj = Main.GSON.fromJson(obj.get(name), new TypeToken<HashMap<String, Integer>>() {}.getType());
		mapObj = sortByValue(mapObj);
		
		PrintWriterUtil.PrintWriterTemplate pwJava = new PrintWriterUtil.JavaEnumInt(name);
		pwJava.begin();
		
		PrintWriterUtil.PrintWriterTemplate pwH = new PrintWriterUtil.HInt(name);
		pwH.begin();
		
		for(String key : mapObj.keySet()) {
			int value = mapObj.get(key);
			pwJava.writeConstant(key, value);
			pwH.writeConstant(prefix + key, value);
		}
		
		pwJava.end();
		pwH.end();

	}
	
	public static <K, V extends Comparable<? super V>> Map<K, V> sortByValue(Map<K, V> map) {
        List<Entry<K, V>> list = new ArrayList<>(map.entrySet());
        list.sort(Entry.comparingByValue());

        Map<K, V> result = new LinkedHashMap<>();
        for (Entry<K, V> entry : list) {
            result.put(entry.getKey(), entry.getValue());
        }

        return result;
    }
	
}
