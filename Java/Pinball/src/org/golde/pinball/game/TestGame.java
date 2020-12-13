package org.golde.pinball.game;

import java.util.Timer;
import java.util.TimerTask;

import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Solinoids;
import org.golde.pinball.constants.Sounds;

public class TestGame extends PinballGame {
	
	@Override
	public void onButtonHit(Buttons btn) {
		System.out.println("Button hit: " + btn);
		writeText(btn.name());
		
		setLight(Lights.TARGET_LEFT, btn == Buttons.LEFT_TARGET);
		setLight(Lights.TARGET_CENTER, btn == Buttons.CENTER_TARGET);
		setLight(Lights.TARGET_RIGHT, btn == Buttons.RIGHT_TARGET);
	}

	@Override
	public void onButtonStateChange(Buttons btn, boolean state) {
		System.out.println("State changed: " + btn + " " + state);
		
	}

	@Override
	public void start() {

		for(Lights light : Lights.values()) {
			setLight(light, false);
		}
		playSound(Sounds.STARTUP);
	}

}
