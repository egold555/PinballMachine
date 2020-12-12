package org.golde.pinball.constants;

//Generated: Fri Dec 11 18:12:54 PST 2020
public enum Buttons {

	BALL_RETURN(1),
	TILT(2),
	RIGHT_SPINNER(3),
	RIGHT_EXTRA_BALL_RETURN(4),
	A(5),
	START(6),
	LEFT_TARGET(7),
	LEFT_SLING_SHOT(8),
	B(9),
	LEFT_THUMPER_BUMPER(10),
	LEFT_BUMPER(11),
	LEFT_EXTRA_BALL_LANE(12),
	C(13),
	RIGHT_THUMPER_BUMPER(14),
	LEFT_SPINNER(15),
	LEFT_ADVANCED_LANE(16),
	D(17),
	RIGHT_TARGET(18),
	RIGHT_SLING_SHOT(19),
	CENTER_TARGET(20),
	RIGHT_BUMPER(21),
	RIGHT_ADVANCED_LANE(22),
	FLIPPER_LEFT(23),
	FLIPPER_RIGHT(24),
	;

	private final Object id;
	Buttons(Object id) {
		this.id = id;
	}

	public Object getId() {
		return id;
	}

	public static Buttons get(Object o) {
		for(Buttons i : values()) {
			if(i.id.equals(o)) {
				return i;
			}
		}
		System.err.println("Failed to get Buttons with id of: " + o);
		return null;
	}
}
