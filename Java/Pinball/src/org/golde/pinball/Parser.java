package org.golde.pinball;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

public class Parser {

	private final String COM_PORT;

	private static final int BAUD_RATE = 57600, DATA_BITS = 8, STOP_BITS = SerialPort.ONE_STOP_BIT, PARITY = SerialPort.NO_PARITY;

	private StringBuilder stringBuilder = new StringBuilder();
	
	public Parser(String comPort) {
		this.COM_PORT = comPort;
	}

	public void start() {
		
		try {
			for(SerialPort sp : SerialPort.getCommPorts()) {
				System.out.println(sp.getSystemPortName());
			}

			SerialPort sp = SerialPort.getCommPort(COM_PORT);

			sp.setComPortParameters(BAUD_RATE, DATA_BITS, STOP_BITS, PARITY);
			//sp.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);

			sp.addDataListener(new SerialPortDataListener() {

				@Override
				public void serialEvent(SerialPortEvent event) {
					//System.out.println(event.getEventType());
					if(event.getEventType() == SerialPort.LISTENING_EVENT_DATA_RECEIVED) {
						try {
							byte[] data = event.getReceivedData();
							for (int i = 0; i < data.length; ++i) {
								if (data[i] == '\n' ) {
									incomingMessage(stringBuilder.toString());
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

	private void incomingMessage(String msg) {

	}

}