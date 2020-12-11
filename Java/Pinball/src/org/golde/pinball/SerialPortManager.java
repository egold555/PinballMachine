package org.golde.pinball;

import java.util.Arrays;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

public class SerialPortManager {

	private final String COM_PORT = "COM5";

	private static final int BAUD_RATE = 57600, DATA_BITS = 8, STOP_BITS = SerialPort.ONE_STOP_BIT, PARITY = SerialPort.NO_PARITY;

	private SerialPort sp;

	private StringBuilder stringBuilder = new StringBuilder();

	public void listAllSerialPorts() {
		for(SerialPort sp : SerialPort.getCommPorts()) {
			System.out.println(sp.getSystemPortName());
		}
	}

	public void open(IncomingMessageCallback incomingMessageCallback) {

		try {

			sp = SerialPort.getCommPort(COM_PORT);

			sp.setComPortParameters(BAUD_RATE, DATA_BITS, STOP_BITS, PARITY);
			sp.setComPortTimeouts(SerialPort.TIMEOUT_NONBLOCKING, SerialPort.TIMEOUT_NONBLOCKING, SerialPort.TIMEOUT_NONBLOCKING);
			//sp.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
			sp.addDataListener(new SerialPortDataListener() {

				@Override
				public void serialEvent(SerialPortEvent event) {
					//System.out.println(event.getEventType());
					if(event.getEventType() == SerialPort.LISTENING_EVENT_DATA_RECEIVED) {
						try {
							byte[] data = event.getReceivedData();
							for (int i = 0; i < data.length; ++i) {
								if (data[i] == '\n') {
									incomingMessageCallback.onMessageRecieved(stringBuilder.toString().replace("\n", "").replace("\r", ""));
									stringBuilder.setLength(0); // clear the stringBuilder
								}
								else {
									stringBuilder.append((char) data[i]);  // Just ASCII, so char convert from byte directly.
								}
							}
						} catch (Exception e) {
							System.err.println(e.toString());
						}
					}
				}

				@Override
				public int getListeningEvents() {
					return SerialPort.LISTENING_EVENT_DATA_RECEIVED;
				}
			});

			sp.openPort();
		}
		catch(Exception e) {

		}

	}		


	public void write(String msg) {
		if(sp == null) {
			System.err.println("Serial port was null so I denied you sending data");
			return;
		}
		msg += "\n";
		byte[] buffer = msg.getBytes();
		System.out.println("About to write data: " + Arrays.toString(buffer));
		int response = sp.writeBytes(buffer, buffer.length);
		
		if(response == -1) {
			System.err.println("Failed to write to COM port: " + msg);
		}
		else {
			System.out.println("Wrote " + response + "/" + buffer.length +  " bytes to the serial port");
		}
	}

	public interface IncomingMessageCallback {
		public void onMessageRecieved(String msg);
	}

}
