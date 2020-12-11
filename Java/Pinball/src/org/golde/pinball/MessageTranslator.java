package org.golde.pinball;

import java.util.Arrays;

import org.golde.pinball.SerialPortManager.IncomingMessageCallback;

public class MessageTranslator implements IncomingMessageCallback {

	private MessageParserCallback callback = null;
	private final SerialPortManager spm;

	public MessageTranslator(SerialPortManager spm) {
		this.spm = spm;
	}

	public void setCallback(MessageParserCallback callback) {
		this.callback = callback;
	}

	public interface MessageParserCallback {

		public void onButtonHit(int id);
		public void onButtonStateChange(int id, boolean state);
		public void start();

	}

	@Override
	public void onMessageRecieved(String msg) {
		String[] split = msg.split("\\-");
		if(split[0].equals("B")) {
			parseButton(split);
		}
		else if(split[0].equals("PC")) {
			System.out.println("Message from arduino: " + split[1]);
			if(split[1].equals("Ready")) {
				if(callback != null) {
					callback.start();
				}
			}
			
		}
		else if(split[0].equals("E")) {
			System.err.println("[OLD - Error] " + split[1]);
		}
		else {
			System.err.println("Unrecognised message: " + msg);
		}
	}

	//B-P-ID
	//B-T-ID-01
	private void parseButton(String[] split) {
		try {
			if(split[1].equals("P")) {
				if(callback != null) {
					callback.onButtonHit(Integer.valueOf(split[2]));
				}

			}
			else if(split[1].equals("T")) {
				int id = Integer.valueOf(split[2]);
				boolean state = (Integer.valueOf(split[3]) == 1 ? true : false);
				if(callback != null) {
					callback.onButtonStateChange(id, state);
				}

			}
			else {
				System.err.println("Got a strange button request: " + Arrays.toString(split));
			}
		}
		catch(Exception e) {
			e.printStackTrace();
			System.err.println("Malformed button: " + Arrays.toString(split));
		}
	}

	public void fireSolinoid(int id) {
		spm.write("S-" + id);
	}

	public void setLight(int id, boolean value) {
		spm.write("L-" + id + "-" + (value ? 1 : 0));
	}

}