package org.golde.pinball.game.original;

import lombok.EqualsAndHashCode;
import lombok.Getter;
import lombok.Setter;
import lombok.ToString;

@Getter
@Setter
@ToString
@EqualsAndHashCode
public class Player {
	
	boolean lightA;
	boolean lightB;
	boolean lightC;
	boolean lightD;
	long score;
	
}
