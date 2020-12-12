package org.golde.pinball.game;

import java.util.Timer;
import java.util.TimerTask;

import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Solinoids;
import org.golde.pinball.constants.Sounds;

public class TestGame extends PinballGame {

	int score = 0;
	
	@Override
	public void onButtonHit(Buttons btn) {
		System.out.println("Button hit: " + btn);
		
		score++;
		writeScore(score);
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
		writeScore(score);
	}

}
