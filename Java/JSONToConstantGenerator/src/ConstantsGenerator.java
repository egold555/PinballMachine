import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

public class ConstantsGenerator implements Runnable {

	@SuppressWarnings("unchecked")
	@Override
	public void run() {
	
		
		//Protocol Ids
		HashMap<String, Double> ids = FileUtil.loadFromFile("ProtocolIDs", HashMap.class);
		
		try {
			
			PrintWriter pwJava = PrintWriterUtil.Java.begin("ProtocolIDs");
			PrintWriter pwH = PrintWriterUtil.H.begin("ProtocolIDs");
			ids = (HashMap<String, Double>) sortByValue(ids);
			for(String key : ids.keySet()) {
				int value = ids.get(key).intValue();
				PrintWriterUtil.Java.writeIntConstant(pwJava, key, value);
				PrintWriterUtil.H.writeIntConstant(pwH, key, value);
			}
			
			PrintWriterUtil.Java.end(pwJava);
			PrintWriterUtil.H.end(pwH);
		}
		catch(FileNotFoundException e) {
			e.printStackTrace();
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
