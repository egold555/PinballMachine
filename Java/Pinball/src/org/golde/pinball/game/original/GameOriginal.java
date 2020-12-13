package org.golde.pinball.game.original;

import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Solinoids;
import org.golde.pinball.constants.Sounds;
import org.golde.pinball.game.PinballGame;

public class GameOriginal extends PinballGame {

	private EnumGameState state = EnumGameState.BEFORE;

	private Player[] players = new Player[MAX_AMOUNT_OF_PLAYERS];
	private int currentPlayer; //0-3
	private int amountOfPlayers; //1-4

	//reset every round
	private boolean hasScoredThisRound = false;

	private boolean targetLeft = false, targetRight = false, targetCenter = false;
	private boolean extraBallLeft = false, extraBallRight = false;
	private boolean doubleBonus = false, trippleBonus = false;
	private byte bonus = 0;
	private byte balls = 1, extraBalls;
	private final byte MAX_BALLS = 5;

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

			if(btn == Buttons.LEFT_THUMPER_BUMPER || btn == Buttons.RIGHT_THUMPER_BUMPER) {
				updatePlayerScore(100);
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


			if(btn == Buttons.LEFT_TARGET && !targetLeft) {
				targetLeft = true;
				advanceBonus();
				advanceBonus();
				setLight(Lights.TARGET_LEFT, true);
			}

			if(btn == Buttons.CENTER_TARGET && !targetCenter) {
				targetCenter = true;
				advanceBonus();
				advanceBonus();
				setLight(Lights.TARGET_RIGHT, true);
			}

			if(btn == Buttons.RIGHT_TARGET && !targetRight) {
				targetRight = true;
				advanceBonus();
				advanceBonus();
				setLight(Lights.TARGET_CENTER, true);
				updatePlayerScore(500); //this one is 1000, and by default very one is 500.
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
			
			if(btn == Buttons.LEFT_EXTRA_BALL_LANE || btn == Buttons.RIGHT_EXTRA_BALL_LANE) {
				updatePlayerScore(500);
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
			setBallLights();
			writeScore(0);
			updateABCDLightsforCurrentPlayer();
			playSound(Sounds.STARTUP);
			fireSolinoid(Solinoids.BALL_RETURN);
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

		long bonusAmount = bonus * 1000;
		playSound(Sounds.NEW_BALL);

		//calculate the bonuses
		if(doubleBonus) {
			bonusAmount = bonus * 1000 * 2;
		}
		else if(trippleBonus) {
			bonusAmount = bonusAmount * 1000 * 3;
		}

		bonus = 0;

		extraBallLeft = false;
		extraBallRight = false;
		setLight(Lights.EXTRA_BALL_LEFT, false);
		setLight(Lights.EXTRA_BALL_RIGHT, false);

		doubleBonus = false;
		trippleBonus = false;
		setLight(Lights.DOUBLE_BONUS, false);
		setLight(Lights.TRIPPLE_BONUS, false);
		setLight(Lights.SAME_PLAYER_SHOOTS, false);

		setLight(Lights.BALL_1, false);
		setLight(Lights.BALL_2, false);
		setLight(Lights.BALL_3, false);
		setLight(Lights.BALL_4, false);
		setLight(Lights.BALL_5, false);

		setLight(Lights.TARGET_LEFT, false);
		setLight(Lights.TARGET_CENTER, false);
		setLight(Lights.TARGET_RIGHT, false);

		setLight(Lights.BONUS_1000, false);
		setLight(Lights.BONUS_2000, false);
		setLight(Lights.BONUS_3000, false);
		setLight(Lights.BONUS_4000, false);
		setLight(Lights.BONUS_5000, false);
		setLight(Lights.BONUS_6000, false);
		setLight(Lights.BONUS_7000, false);
		setLight(Lights.BONUS_8000, false);
		setLight(Lights.BONUS_9000, false);
		setLight(Lights.BONUS_10000, false);

		//TODO: wait for music to play out
		while(isSoundPlaying()) {
			veryBadDelay(1);
		}

		while(bonusAmount < 0) {
			veryBadDelay(500);
			bonusAmount -=1000;
			updatePlayerScore(1000);
		}

		veryBadDelay(1000);

		if(extraBalls > 0) {
			extraBalls--;
			writeText("XtraBall");
			System.out.println("Extra ball");
		}
		else {
			switchPlayersOrAdvanceBall();
		}

		setBallLights();

		if (balls > MAX_BALLS)
		{
			endGame();
			return;
		}
		hasScoredThisRound = false;
		fireSolinoid(Solinoids.BALL_RETURN);
		writeScore(players[currentPlayer].score);
		//tilted = false;

	}
	
	private void setBallLights() {
		System.out.println("Ball light: " + balls);
		switch (balls)
		{
		case 1:
			setLight(Lights.BALL_1, true);
			break;
		case 2:
			setLight(Lights.BALL_2, true);
			break;
		case 3:
			setLight(Lights.BALL_3, true);
			break;
		case 4:
			setLight(Lights.BALL_4, true);
			break;
		case 5:
			setLight(Lights.BALL_5, true);
			break;
		}
	}

	private void endGame() {
		boolean endNow = false;
		state = EnumGameState.GAME_OVER;
		while(isSoundPlaying()) {
			veryBadDelay(1);
		}
		playSound(Sounds.ENDING_SONG);
		for(Lights l : Lights.values()) {
			setLight(l, false);
		}
		setLight(Lights.A, true);
		setLight(Lights.B, true);
		setLight(Lights.C, true);
		setLight(Lights.D, true);

		setLight(Lights.TARGET_LEFT, true);
		setLight(Lights.TARGET_CENTER, true);
		setLight(Lights.TARGET_RIGHT, true);

		while(isSoundPlaying() && !endNow) {
			int animationDelay = 2000;

			for(int i = 0; i < amountOfPlayers; i++) {
				Player p = players[i];
				lightPlayerLights(i + 1);
				writeScore(p.score);


				//endNow = delayWithLights(animationdelay);
				veryBadDelay(animationDelay);

				if(endNow) {
					break;
				}

				EnumRankingTitle rankingTitle = EnumRankingTitle.get(p.score);
				writeText(rankingTitle.name());
				//endNow = delayWithLights(animationdelay);
				veryBadDelay(animationDelay);

				if(endNow) {
					break;
				}
			}

		}

		//stop music
		stopMusic();
		
		lightPlayerLights(0);
		reset();

	}



	private void switchPlayersOrAdvanceBall() {
		++currentPlayer;
		if (currentPlayer >= amountOfPlayers)
		{
			balls++;
			setBallLights();
			currentPlayer = 0;
		}

		setLight(Lights.A, players[currentPlayer].lightA);
		setLight(Lights.B, players[currentPlayer].lightB);
		setLight(Lights.C, players[currentPlayer].lightC);
		setLight(Lights.D, players[currentPlayer].lightD);

		lightPlayerLights(currentPlayer + 1);
	}



	//TODO: THIS IS BAD, ERIC WHAT THE FUCK MAN
	private void veryBadDelay(int amount) {
		try {
			Thread.sleep(amount);
		} 
		catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	private void updatePlayerScore(int add) {
		players[currentPlayer].score += add;
		playSound(Sounds.POINT);
		writeScore(players[currentPlayer].score);
		hasScoredThisRound = true;
	}

	private void advanceBonus() {
		System.out.println("Targets: " + targetLeft + " " + targetRight + " " + targetCenter);
		if (targetLeft && targetRight)
		{
			if (targetCenter)
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

	//game over, reset it for the next time
	private void reset() {
		stopMusic();
		//turn all lights off
		for(Lights l : Lights.values()) {
			setLight(l, false);
		}

		for(Player p : players) {
			p.lightA = true;
			p.lightB = true;
			p.lightC = true;
			p.lightD = true;
			p.score = 0;
		}

		amountOfPlayers = 0;
		currentPlayer = 0;
		hasScoredThisRound = false;

		targetLeft = targetRight = targetCenter = false;
		extraBallLeft = extraBallRight = false;
		doubleBonus = trippleBonus = false;
		bonus = 0;
		extraBalls = 0;



		state = EnumGameState.BEFORE;
		playSound(Sounds.STARTUP);

	}

}
