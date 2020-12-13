package org.golde.pinball.game.original;

public enum EnumRankingTitle {

	AMATEUR(40000),
	SUPER(75000),
	CHAMP(100000),
	FANTASTIC(125000),
	WIZARD(150000),
	FIREBALL(150000),
	
	ERROR(-1);
	
	private final long score;
	EnumRankingTitle(long score) {
		this.score = score;
	}
	
	public static EnumRankingTitle get(long score2) {
		
		if(score2 < AMATEUR.score) {
			return AMATEUR;
		}
		else if(score2 < SUPER.score) {
			return SUPER;
		}
		else if(score2 < CHAMP.score) {
			return CHAMP;
		}
		else if(score2 < FANTASTIC.score) {
			return FANTASTIC;
		}
		else if(score2 < WIZARD.score) {
			return WIZARD;
		}
		else if(score2 > FIREBALL.score) {
			return FIREBALL;
		}
		else {
			return ERROR;
		}
	}
	
}
