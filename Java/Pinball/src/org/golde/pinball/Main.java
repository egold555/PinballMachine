package org.golde.pinball;

import org.golde.pinball.game.TestGame;
import org.golde.pinball.game.PinballGame;

public class Main {

	public static void main(String[] args) {
		SerialPortManager spm = new SerialPortManager();
		MessageTranslator translator = new MessageTranslator(spm);
		PinballGame game = new TestGame();
		translator.setCallback(game);
		spm.open(translator);
	}
	
}
