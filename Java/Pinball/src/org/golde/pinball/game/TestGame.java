package org.golde.pinball.game;

import java.util.Timer;
import java.util.TimerTask;

import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Solinoids;

public class TestGame extends PinballGame {

	@Override
	public void onButtonHit(Buttons btn) {
		System.out.println("Button hit: " + btn);
		if(btn == Buttons.LEFT_THUMPER_BUMPER) {
			fireSolinoid(Solinoids.LEFT_THUMPER_BUMPER);
		}
		if(btn == Buttons.RIGHT_THUMPER_BUMPER) {
			fireSolinoid(Solinoids.RIGHT_THUMPER_BUMPER);
		}
		if(btn == Buttons.LEFT_SLING_SHOT) {
			fireSolinoid(Solinoids.LEFT_SLING_SHOT);
		}
		if(btn == Buttons.RIGHT_SLING_SHOT) {
			fireSolinoid(Solinoids.RIGHT_SLING_SHOT);
		}
		if(btn == Buttons.BALL_RETURN) {
			fireSolinoid(Solinoids.BALL_RETURN);
		}
	}

	@Override
	public void onButtonStateChange(Buttons btn, boolean state) {
		System.out.println("State changed: " + btn + " " + state);
	}

	@Override
	public void start() {

		new Timer().scheduleAtFixedRate(new TimerTask() {

			@Override
			public void run() {



			}
		}, 0, 100);
	}

}
