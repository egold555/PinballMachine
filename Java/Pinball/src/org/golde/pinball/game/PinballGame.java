package org.golde.pinball.game;

import org.golde.pinball.MessageTranslator;
import org.golde.pinball.MessageTranslator.MessageParserCallback;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Solinoids;
import org.golde.pinball.constants.SoundData;
import org.golde.pinball.constants.Sounds;

public abstract class PinballGame implements MessageParserCallback {

	protected static final int MAX_AMOUNT_OF_PLAYERS = 4;
	private MessageTranslator mt;
	private boolean soundPlaying = false;

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

	public final void writeScore(long score) {
		if(mt != null) {
			mt.writeScore(score);
		}
	}

	public final void writeText(String text) {
		if(mt != null) {
			mt.writeText(text);
		}
	}

	@Deprecated
	public final void writeScrollingText(String text) {
		if(mt != null) {
			mt.writeScrollingText(text);
		}
	}

	public final void playSound(Sounds sound) {
		if(mt != null) {
			mt.playSound(sound);
			soundPlaying = true;
		}
	}

	@Override
	public final void setMessageTranslator(MessageTranslator translator) {
		this.mt = translator;
	}

	@Override
	public final void onSoundData(SoundData soundData) {
		if(soundData == SoundData.FEEDBACK_STOPPED) {
			soundPlaying = false;
		}

	}
	
	public final void stopMusic() {
		if(mt != null) {
			mt.sendSoundData(SoundData.STOP);
		}
	}

	public boolean isSoundPlaying() {
		return soundPlaying;
	}
}
