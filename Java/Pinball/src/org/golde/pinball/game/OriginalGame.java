package org.golde.pinball.game;

import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Solinoids;

public class OriginalGame extends PinballGame {

	@Override
	public void onButtonHit(Buttons btn) {
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
		// TODO Auto-generated method stub
		
	}

	@Override
	public void start() {
		// TODO Auto-generated method stub
		
	}

}
