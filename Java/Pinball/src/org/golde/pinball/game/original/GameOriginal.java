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

	//reset every round
	private boolean hasScoredThisRound = false;

	private boolean target1 = true, target2 = true, target3 = true;
	private boolean extraBallLeft = false, extraBallRight = false;
	private boolean doubleBonus = false, trippleBonus = false;
	private int bonus = 0;

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



			//ABCD Advance bonus
			if(btn == Buttons.A && players[currentPlayer].lightA) {
				players[currentPlayer].lightA = false;
				updateABCDLightsforCurrentPlayer();
				advanceBonus();
			}

			if(btn == Buttons.B && players[currentPlayer].lightB) {
				players[currentPlayer].lightB = false;
				updateABCDLightsforCurrentPlayer();
				advanceBonus();
			}

			if(btn == Buttons.C && players[currentPlayer].lightC) {
				players[currentPlayer].lightC = false;
				updateABCDLightsforCurrentPlayer();
				advanceBonus();
			}

			if(btn == Buttons.D && players[currentPlayer].lightD) {
				players[currentPlayer].lightD = false;
				updateABCDLightsforCurrentPlayer();
				advanceBonus();
			}

			//Advance bonus if ABCD is all off
			if (!players[currentPlayer].lightA && !players[currentPlayer].lightB && !players[currentPlayer].lightC && !players[currentPlayer].lightD)
			{
				players[currentPlayer].lightA = true;
				players[currentPlayer].lightB = true;
				players[currentPlayer].lightC = true;
				players[currentPlayer].lightD = true;
				updateABCDLightsforCurrentPlayer();
				players[currentPlayer].score += 25000;
				extraBallLeft = true;
				extraBallRight = true;
				setLight(Lights.EXTRA_BALL_LEFT, true);
				setLight(Lights.EXTRA_BALL_RIGHT, true);
				playSound(Sounds.STARTUP);
			}

			//ABCD Points
			if(btn == Buttons.A || btn == Buttons.B || btn == Buttons.C || btn == Buttons.D) {
				updatePlayerScore(100);
			}

			if(btn == Buttons.LEFT_BUMPER || btn == Buttons.RIGHT_BUMPER) {
				updatePlayerScore(50);
			}


			if(btn == Buttons.LEFT_TARGET && target1) {
				target1 = false;
				advanceBonus();
				advanceBonus();
			}

			if(btn == Buttons.CENTER_TARGET && target2) {
				target2 = false;
				updatePlayerScore(500); //Not sure why but this is in the old code. (Accounting fo 500 of above function)
				advanceBonus();
				advanceBonus();
			}

			if(btn == Buttons.RIGHT_TARGET && target3) {
				target3 = false;
				advanceBonus();
				advanceBonus();
			}

			if(btn == Buttons.LEFT_TARGET || btn == Buttons.RIGHT_TARGET || btn == Buttons.CENTER_TARGET) {
				updatePlayerScore(500);
			}

			//spinners
			if(btn == Buttons.LEFT_SPINNER || btn == Buttons.RIGHT_SPINNER) {
				updatePlayerScore(100);
			}

			if(btn == Buttons.LEFT_EXTRA_BALL_LANE && extraBallLeft) {
				extraBallLeft = false;
				setLight(Lights.EXTRA_BALL_LEFT, false); //TODO: Make it blink
			}

			if(btn == Buttons.RIGHT_EXTRA_BALL_LANE && extraBallRight) {
				extraBallRight = false;
				setLight(Lights.EXTRA_BALL_RIGHT, false); //TODO: Make it blink
			}

			if(btn == Buttons.LEFT_ADVANCED_LANE || btn == Buttons.RIGHT_ADVANCED_LANE) {
				updatePlayerScore(500);
				advanceBonus();
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

		//turn off all the lightslt
		for(Lights l : Lights.values()) {
			setLight(l, false);
		}

	}

	private void startGame() {

		//Initalize playing, ball 1 is hilighted.
		if(state == EnumGameState.BEFORE) {
			state = EnumGameState.PLAYING;
			setLight(Lights.BALL_1, true);
			writeScore(0);
			updateABCDLightsforCurrentPlayer();
		}

		//if we finished a game, start button is the restart button
		else if(state == EnumGameState.GAME_OVER) {
			reset();
		}

		//if we havent stored this round, we can press the start button again to increment how many players we
		//are wanting to play with
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

	private void updateABCDLightsforCurrentPlayer() {
		setLight(Lights.A, players[currentPlayer].lightA);
		setLight(Lights.B, players[currentPlayer].lightB);
		setLight(Lights.C, players[currentPlayer].lightC);
		setLight(Lights.D, players[currentPlayer].lightD);
	}
	private void endOfBall() {
		if(!hasScoredThisRound) {
			fireSolinoid(Solinoids.BALL_RETURN);  //Ball fireer has failed, so we should not count this as a new ball, we should keep trying to push the ball out of the shooter
			return;
		}
	}

	private void updatePlayerScore(int add) {
		players[currentPlayer].score += add;
		playSound(Sounds.POINT);
		writeScore(players[currentPlayer].score);
	}

	private void advanceBonus() {
		if (!target1 && !target2)
		{
			if (!target3)
			{
				doubleBonus = false;
				trippleBonus = true;
				setLight(Lights.DOUBLE_BONUS, false);
				setLight(Lights.TRIPPLE_BONUS, true);
			}
			else
			{
				doubleBonus = true;
				setLight(Lights.DOUBLE_BONUS, true);
			}
		}

		bonus++;
		if (bonus > 10)
		{
			bonus = 10;
		}

		if (bonus == 1)
		{
			setLight(Lights.BONUS_1000, true);
		}
		if (bonus == 2)
		{
			setLight(Lights.BONUS_2000, true);
		}
		if (bonus == 3)
		{
			setLight(Lights.BONUS_3000, true);
		}
		if (bonus == 4)
		{
			setLight(Lights.BONUS_4000, true);
		}
		if (bonus == 5)
		{
			setLight(Lights.BONUS_5000, true);
		}
		if (bonus == 6)
		{
			setLight(Lights.BONUS_6000, true);
		}
		if (bonus == 7)
		{
			setLight(Lights.BONUS_7000, true);
		}
		if (bonus == 8)
		{
			setLight(Lights.BONUS_8000, true);
		}
		if (bonus == 9)
		{
			setLight(Lights.BONUS_9000, true);
		}
		if (bonus == 10)
		{
			setLight(Lights.BONUS_10000, true);
		}

	}

	private void reset() {
		// TODO Auto-generated method stub

	}

}
