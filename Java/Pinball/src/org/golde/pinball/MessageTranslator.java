package org.golde.pinball;

import java.util.Arrays;

import org.golde.pinball.SerialPortManager.IncomingMessageCallback;
import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Messages;
import org.golde.pinball.constants.Solinoids;
import org.golde.pinball.constants.Sounds;

public class MessageTranslator implements IncomingMessageCallback {

	private MessageParserCallback callback = null;
	private final SerialPortManager spm;

	public MessageTranslator(SerialPortManager spm) {
		this.spm = spm;
	}

	public void setCallback(MessageParserCallback callback) {
		this.callback = callback;
		callback.setMessageTranslator(this);
	}

	public interface MessageParserCallback {

		public void onButtonHit(Buttons btn);
		public void onButtonStateChange(Buttons btn, boolean state);
		public void start();
		public default void setMessageTranslator(MessageTranslator translator) {};

	}

	@Override
	public void onMessageRecieved(String msg) {
		String[] split = msg.split("\\-");
		if(split[0].equals(Messages.BUTTON_PRESSED.getId()) || split[0].equals(Messages.BUTTON_TOGGLED.getId())) {
			parseButton(split);
		}
		else if(split[0].equals(Messages.LOG.getId())) {
			System.out.println("[Arduino Info] " + split[1]);
			if(split[1].equals("Ready")) {
				if(callback != null) {
					callback.start();
				}
			}
			
		}
		else if(split[0].equals(Messages.ERROR.getId())) {
			System.err.println("[Arduino Error] " + split[1]);
		}
		else {
			System.err.println("[Com Warning] Unrecognised message recieved: " + msg);
		}
	}

	//BP-ID
	//BT-ID-01
	private void parseButton(String[] split) {
		try {
			if(split[0].equals(Messages.BUTTON_PRESSED.getId())) {
				if(callback != null) {
					int id = Integer.valueOf(split[1]);
					callback.onButtonHit(Buttons.get(id));
				}

			}
			else if(split[0].equals(Messages.BUTTON_TOGGLED.getId())) {
				int id = Integer.valueOf(split[1]);
				boolean state = (Integer.valueOf(split[2]) == 1 ? true : false);
				if(callback != null) {
					callback.onButtonStateChange(Buttons.get(id), state);
				}

			}
			else {
				System.err.println("[Com Warning] Got a strange button request: " + Arrays.toString(split));
			}
		}
		catch(Exception e) {
			e.printStackTrace();
			System.err.println("[Com Error] Malformed button: " + Arrays.toString(split));
		}
	}

	public void fireSolinoid(Solinoids id) {
		spm.write(Messages.SOLINOID.getId() + "-" + id.getId());
	}

	public void setLight(Lights id, boolean value) {
		spm.write(Messages.LIGHT.getId() + "-" + id.getId() + "-" + (value ? 1 : 0));
	}
	
	public void writeScore(int score) {
		//right align
		String scoreText = Integer.toString(score);
		scoreText = scoreText.substring(0, Math.min(scoreText.length(), 8));
		scoreText = String.format("%8s", scoreText);
		writeText(scoreText);
	}
	
	public void writeText(String text) {
		spm.write(Messages.DISPLAY_TEXT.getId() + "-" + text);
	}
	
	@Deprecated
	public void writeScrollingText(String text) {
		
	}
	
	public void playSound(Sounds sound) {
		spm.write(Messages.SOUND.getId() + "-" + sound.getId());
	}

}