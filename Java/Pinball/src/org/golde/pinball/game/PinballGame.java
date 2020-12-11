package org.golde.pinball.game;

import org.golde.pinball.MessageTranslator;
import org.golde.pinball.MessageTranslator.MessageParserCallback;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Solinoids;

public abstract class PinballGame implements MessageParserCallback {

	private MessageTranslator mt;
	
	public final void setLight(Lights light, boolean value) {
		if(mt != null) {
			mt.setLight(light, value);
		}
	}

	public final void fireSolinoid(Solinoids solinoid) {
		if(mt != null) {
			mt.fireSolinoid(solinoid);
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
