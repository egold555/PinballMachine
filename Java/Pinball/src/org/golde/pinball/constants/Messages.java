package org.golde.pinball.constants;

//Generated: Mon Dec 14 16:23:05 PST 2020
public enum Messages {

	BUTTON_PRESSED("BP"),
	BUTTON_TOGGLED("BT"),
	DISPLAY_TEXT_SCROLL("DS"),
	DISPLAY_TEXT("DT"),
	ERROR("ER"),
	LOG("LG"),
	LIGHT("LT"),
	SOUND_DATA("SD"),
	SOLINOID("SN"),
	SOUND_PLAY("SP"),
	;

	private final Object id;
	Messages(Object id) {
		this.id = id;
	}

	public Object getId() {
		return id;
	}

	public static Messages get(Object o) {
		for(Messages i : values()) {
			if(i.id.equals(o)) {
				return i;
			}
		}
		System.err.println("Failed to get Messages with id of: " + o);
		return null;
	}
}
