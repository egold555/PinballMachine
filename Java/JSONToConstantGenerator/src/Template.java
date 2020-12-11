
public enum Template {

	TEST(1);
	
	private final int id;
	Template(int id) {
		this.id = id;
	}
	
	public int getId() {
		return id;
	}
	
}
