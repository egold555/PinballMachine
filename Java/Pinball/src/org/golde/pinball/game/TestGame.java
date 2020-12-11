package org.golde.pinball.game;

import java.util.Timer;
import java.util.TimerTask;

import org.golde.pinball.MessageTranslator;
import org.golde.pinball.constants.ProtocolIDs;

public class TestGame extends PinballGame {

	int currentLight = ProtocolIDs.PID_LT_BONUS_8000;
	int lastLight = ProtocolIDs.PID_LT_BALL_1;
	
	int currentSolinoid = ProtocolIDs.PID_SN_BALL_RETURN;

	@Override
	public void onButtonHit(int id) {
		System.out.println("Button hit: " + id);
		if(id == ProtocolIDs.PID_SW_LEFT_THUMPER_BUMPER) {
			fireSolinoid(ProtocolIDs.PID_SN_LEFT_THUMPER_BUMPER);
		}
		if(id == ProtocolIDs.PID_SW_RIGHT_THUMPER_BUMPER) {
			fireSolinoid(ProtocolIDs.PID_SN_RIGHT_THUMPER_BUMPER);
		}
		if(id == ProtocolIDs.PID_SW_LEFT_SLING_SHOT) {
			fireSolinoid(ProtocolIDs.PID_SN_LEFT_SLING_SHOT);
		}
		if(id == ProtocolIDs.PID_SW_RIGHT_SLING_SHOT) {
			fireSolinoid(ProtocolIDs.PID_SN_RIGHT_SLING_SHOT);
		}
		if(id == ProtocolIDs.PID_SW_BALL_RETURN) {
			fireSolinoid(ProtocolIDs.PID_SN_BALL_RETURN);
		}
	}

	@Override
	public void onButtonStateChange(int id, boolean state) {
		System.out.println("State changed: " + id + " " + state);
	}

	@Override
	public void start() {
		
		for(int i = ProtocolIDs.PID_LT_BONUS_8000; i < ProtocolIDs.PID_LT_BALL_1; i++) {
			setLight(i, false);
		}
		
		new Timer().scheduleAtFixedRate(new TimerTask() {

			@Override
			public void run() {

				setLight(currentLight, true);
				setLight(lastLight, false);

				lastLight = currentLight;
				currentLight++;

				if(currentLight > ProtocolIDs.PID_LT_BALL_1) {
					currentLight = ProtocolIDs.PID_LT_BONUS_8000;
				}



			}
		}, 0, 100);
		
//		new Timer().scheduleAtFixedRate(new TimerTask() {
//
//			@Override
//			public void run() {
//
//				fireSolinoid(currentSolinoid);
//				
//				currentSolinoid++;
//				if(currentSolinoid > ProtocolIDs.PID_SN_RIGHT_THUMPER_BUMPER) {
//					currentSolinoid = ProtocolIDs.PID_SN_BALL_RETURN;
//				}
//				
//
//			}
//		}, 0, 1000);
		
		
		System.out.println("Set");
	}

}
