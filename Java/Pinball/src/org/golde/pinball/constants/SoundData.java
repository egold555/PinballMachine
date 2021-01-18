package org.golde.pinball.constants;

//Generated: Mon Dec 14 16:23:05 PST 2020
public enum SoundData {

	STOP(10),
	FEEDBACK_STOPPED(11),
	;

	private final Object id;
	SoundData(Object id) {
		this.id = id;
	}

	public Object getId() {
		return id;
	}

	public static SoundData get(Object o) {
		for(SoundData i : values()) {
			if(i.id.equals(o)) {
				return i;
			}
		}
		System.err.println("Failed to get SoundData with id of: " + o);
		return null;
	}
}
