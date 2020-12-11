package org.golde.pinball.game;

import org.golde.pinball.MessageTranslator;
import org.golde.pinball.MessageTranslator.MessageParserCallback;

public abstract class PinballGame implements MessageParserCallback {

	private MessageTranslator mt;
	
	public final void setLight(int id, boolean value) {
		if(mt != null) {
			mt.setLight(id, value);
		}
	}

	public final void fireSolinoid(int id) {
		if(mt != null) {
			mt.fireSolinoid(id);
		}
	}
	
	@Deprecated
	public final void writeScore(int score) {
		
	}
	
	@Deprecated
	public final void writeText(String text) {
		
	}
	
	@Deprecated
	public final void playSFX(int sfx) {
		
	}
	
	@Override
	public final void setMessageTranslator(MessageTranslator translator) {
		this.mt = translator;
	}
}
