import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

public class Main {

	static final Gson GSON = new GsonBuilder().setPrettyPrinting().create();
	
	public static void main(String[] args) {
		new ConstantsGenerator().run();
	}

}
