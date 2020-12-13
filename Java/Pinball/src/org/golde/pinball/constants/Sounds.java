package org.golde.pinball.constants;

//Generated: Sat Dec 12 17:03:10 PST 2020
public enum Sounds {

	STARTUP(10),
	POINT(11),
	NEW_BALL(12),
	EXTRA_BALL(13),
	TILT(14),
	ENDING_SONG(15),
	NAME_ENTER(16),
	;

	private final Object id;
	Sounds(Object id) {
		this.id = id;
	}

	public Object getId() {
		return id;
	}

	public static Sounds get(Object o) {
		for(Sounds i : values()) {
			if(i.id.equals(o)) {
				return i;
			}
		}
		System.err.println("Failed to get Sounds with id of: " + o);
		return null;
	}
}
