package org.golde.pinball.game.original;

import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Solinoids;
import org.golde.pinball.constants.Sounds;
import org.golde.pinball.game.PinballGame;

public class GameOriginal extends PinballGame {

	private EnumGameState state = EnumGameState.BEFORE;
	
	private Player[] players = new Player[MAX_AMOUNT_OF_PLAYERS];
	private int currentPlayer;
	private int amountOfPlayers;
	private boolean hasScoredThisRound = false;
	
	@Override
	public void onButtonHit(Buttons btn) {
		
		if(state == EnumGameState.PLAYING) {
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
		}
		
		if(btn == Buttons.BALL_RETURN && state != EnumGameState.GAME_OVER) {
			endOfBall();
		}
		
		if(btn == Buttons.START) {
			startGame();
		}
		
		
	}

	

	@Override
	public void onButtonStateChange(Buttons btn, boolean state) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void start() {
		//clear out all the old player objects, and make new ones
		for(int i = 0; i < MAX_AMOUNT_OF_PLAYERS; i++) {
			players[i] = new Player();
		}
	}
	
	private void startGame() {
		
		//Initalize playing, ball 1 is hilighted.
		if(state == EnumGameState.BEFORE) {
			state = EnumGameState.PLAYING;
			setLight(Lights.BALL_1, true);
			writeScore(0);
		}
		else if(state == EnumGameState.GAME_OVER) {
			reset();
		}
		
		if(!hasScoredThisRound && players[0].score == 0) {
			amountOfPlayers++;
			if(amountOfPlayers > MAX_AMOUNT_OF_PLAYERS) {
				amountOfPlayers = 1;
			}
			playSound(Sounds.POINT);
			lightPlayerLights(amountOfPlayers);
		}
	}

	private void lightPlayerLights(int num) {
		setLight(Lights.PLAYER_1, num == 1);
		setLight(Lights.PLAYER_2, num == 2);
		setLight(Lights.PLAYER_3, num == 3);
		setLight(Lights.PLAYER_4, num == 4);
	}
	
	private void endOfBall() {
		if(!hasScoredThisRound) {
			fireSolinoid(Solinoids.BALL_RETURN);  //Ball fireer has failed, so we should not count this as a new ball, we should keep trying to push the ball out of the shooter
			return;
		}
	}

	private void reset() {
		// TODO Auto-generated method stub
		
	}

}
