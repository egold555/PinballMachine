package org.golde.pinball.constants;

//Generated: Sat Dec 12 11:51:13 PST 2020
public enum Solinoids {

	BALL_RETURN(56),
	LEFT_SLING_SHOT(57),
	RIGHT_SLING_SHOT(58),
	LEFT_THUMPER_BUMPER(59),
	RIGHT_THUMPER_BUMPER(60),
	;

	private final Object id;
	Solinoids(Object id) {
		this.id = id;
	}

	public Object getId() {
		return id;
	}

	public static Solinoids get(Object o) {
		for(Solinoids i : values()) {
			if(i.id.equals(o)) {
				return i;
			}
		}
		System.err.println("Failed to get Solinoids with id of: " + o);
		return null;
	}
}
