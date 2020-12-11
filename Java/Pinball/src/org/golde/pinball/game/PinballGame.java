package org.golde.pinball.game;

import org.golde.pinball.MessageTranslator;
import org.golde.pinball.MessageTranslator.MessageParserCallback;

public abstract class PinballGame implements MessageParserCallback{

	private final MessageTranslator mt;
	
	public PinballGame(MessageTranslator mt) {
		this.mt = mt;
	}
	
	public final void setLight(int id, boolean value) {
		mt.setLight(id, value);
	}

	public final void fireSolinoid(int id) {
		mt.fireSolinoid(id);
	}
}
