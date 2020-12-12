import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.google.gson.JsonObject;
import com.google.gson.reflect.TypeToken;

public class ConstantsGenerator implements Runnable {

	private static final String FILE_NAME = "Protocol";

	@Override
	public void run() {

		try {
			intConstants("Buttons", "PID_SW_", false);
			intConstants("Lights", "PID_LT_", false);
			intConstants("Solinoids", "PID_SN_", false);
			intConstants("Messages", null, true);
			intConstants("Sounds", "PID_SD_", false);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	@SuppressWarnings("unchecked")
	private void intConstants(String name, String prefix, boolean ignoreCPP) throws FileNotFoundException {

		JsonObject obj = FileUtil.loadFromFile(FILE_NAME, JsonObject.class);
		Map<String, Object> mapObj = Main.GSON.fromJson(obj.get(name), new TypeToken<HashMap<String, Object>>() {}.getType());
		Map<String, Comparable<Object>> stupidHack = new HashMap<String, Comparable<Object>>();

		for(String key : mapObj.keySet()) {
			Object value = mapObj.get(key);
			stupidHack.put(key, (Comparable<Object>)value);
		}

		stupidHack = sortByValue(stupidHack);

		PrintWriterUtil.PrintWriterTemplate pwJava = new PrintWriterUtil.JavaEnumInt(name);
		pwJava.begin();

		PrintWriterUtil.PrintWriterTemplate pwH = new PrintWriterUtil.HInt(name);

		if(!ignoreCPP) {
			pwH.begin();
		}

		for(String key : stupidHack.keySet()) {
			Object value = stupidHack.get(key);
			pwJava.writeConstant(key, value);
			if(!ignoreCPP) {
				pwH.writeConstant(prefix + key, value);
			}
		}

		pwJava.end();

		if(!ignoreCPP) {
			pwH.end();
		}

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
