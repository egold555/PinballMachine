package org.golde.pinball.game.midi;

import java.io.File;
import java.io.IOException;
import java.util.concurrent.ThreadLocalRandom;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.Sequence;
import javax.sound.midi.Sequencer;
import javax.sound.midi.ShortMessage;

import org.golde.pinball.constants.Buttons;
import org.golde.pinball.constants.Lights;
import org.golde.pinball.constants.Solinoids;
import org.golde.pinball.game.PinballGame;

public class GameMidi extends PinballGame implements Receiver {

	private Receiver old;

	@Override
	public void onButtonHit(Buttons btn) {
		// TODO Auto-generated method stub

	}

	@Override
	public void onButtonStateChange(Buttons btn, boolean state) {
		// TODO Auto-generated method stub

	}

	@Override
	public void start() {
		for(Lights l : Lights.values()) {
			setLight(l, false);
		}

		try {
			Sequencer sequencer = MidiSystem.getSequencer(); // Get the default Sequencer
			if (sequencer==null) {
				System.err.println("Sequencer device not supported");
				return;
			} 
			sequencer.open(); // Open device
			// Create sequence, the File must contain MIDI file data.
			Sequence sequence = MidiSystem.getSequence(new File("res/games/midi/full.mid"));
			sequencer.setSequence(sequence); // load it into sequencer

			old = sequencer.getReceiver();
			sequencer.getTransmitter().setReceiver(this);

			sequencer.start();  // start the playback

		} catch (MidiUnavailableException | InvalidMidiDataException | IOException ex) {
			ex.printStackTrace();
		}

	}

	public static <T extends Enum<?>> T randomEnum(Class<T> clazz){
		int x = ThreadLocalRandom.current().nextInt(clazz.getEnumConstants().length);
		return clazz.getEnumConstants()[x];
	}

	@Override
	public void close() {
		old.close();
	}

	@Override
	public void send(MidiMessage message, long timeStamp) {
		old.send(message, timeStamp);
		if(message instanceof ShortMessage) {
			ShortMessage sm = (ShortMessage)message;
			writeText("" + sm.getData1() + " " + sm.getChannel() + " " + sm.getChannel());
			if (sm.getCommand() == ShortMessage.NOTE_ON) {
				
				if(sm.getChannel() == 9) {
					fireSolinoid(randomEnum(Solinoids.class));
				}
				
				
				
			}
			else if (sm.getCommand() == ShortMessage.NOTE_OFF) {
				//System.out.printf("NOTE OFF\n");
				
			}
		}
	}

}
