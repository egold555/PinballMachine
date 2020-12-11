package org.golde.pinball.constants;

//Generated: Fri Dec 11 11:36:31 PST 2020
public enum Solinoids {

	BALL_RETURN(56),
	LEFT_SLING_SHOT(57),
	RIGHT_SLING_SHOT(58),
	LEFT_THUMPER_BUMPER(59),
	RIGHT_THUMPER_BUMPER(60),
	;

	private final int id;
	Solinoids(int id) {
		this.id = id;
	}

	public int getId() {
		return id;
	}

}
