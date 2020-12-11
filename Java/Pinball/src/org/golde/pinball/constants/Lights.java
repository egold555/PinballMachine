package org.golde.pinball.constants;

//Generated: Fri Dec 11 11:36:31 PST 2020
public enum Lights {

	BONUS_8000(25),
	BONUS_9000(26),
	EXTRA_BALL_RIGHT(27),
	A(28),
	BONUS_6000(29),
	T1(30),
	BONUS_1000(31),
	B(32),
	BONUS_5000(33),
	DOUBLE_BONUS(34),
	EXTRA_BALL_LEFT(35),
	C(36),
	BONUS_7000(37),
	BONUS_10000(38),
	BONUS_2000(39),
	D(40),
	BONUS_3000(41),
	T2(42),
	SAME_PLAYER_SHOOTS(43),
	T3(44),
	BONUS_4000(45),
	TRIPPLE_BONUS(46),
	BALL_2(47),
	BALL_3(48),
	BALL_4(49),
	BALL_5(50),
	PLAYER_1(51),
	PLAYER_2(52),
	PLAYER_3(53),
	PLAYER_4(54),
	BALL_1(55),
	;

	private final int id;
	Lights(int id) {
		this.id = id;
	}

	public int getId() {
		return id;
	}

}
