package org.golde.pinball;

import org.golde.pinball.game.original.GameOriginal;
import org.golde.pinball.game.PinballGame;
import org.golde.pinball.game.debug.TestGame;
import org.golde.pinball.game.midi.GameMidi;

public class Main {

	public static void main(String[] args) {
		SerialPortManager spm = new SerialPortManager();
		MessageTranslator translator = new MessageTranslator(spm);
		PinballGame game = new GameMidi();
		translator.setCallback(game);
		spm.open(translator);
	}
	
}
