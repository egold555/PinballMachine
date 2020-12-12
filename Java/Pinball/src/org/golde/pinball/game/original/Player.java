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
	
	boolean lightA = true;
	boolean lightB = true;
	boolean lightC = true;
	boolean lightD = true;
	long score = 0;
	
}
