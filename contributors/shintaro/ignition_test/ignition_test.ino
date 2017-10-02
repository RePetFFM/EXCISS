#define CHARGEMONITOR_VOLTAGE_MAX_CHARGE 600

uint16_t targetvoltage = 300;

uint8_t ingition_requested = false;

void setup() {
	pinMode(11, INPUT_PULLUP); // charge
	pinMode(10, INPUT_PULLUP); // ignition

	pinMode(6, OUTPUT); // ignition
	pinMode(7, OUTPUT); // activity
	pinMode(8, OUTPUT); // charge
	Serial.begin(9600);

	for(uint8_t i=0; i<20; i++) {
		digitalWrite(7,HIGH);
		delay(50);
		digitalWrite(7,LOW);
		delay(50);
	}
}

uint8_t docharge = 0;

void loop() {
	SERIAL__Parser();
	uint8_t pinStates = 0;
	pinStates = 0;
	if(digitalRead(10)==HIGH) {
		pinStates = pinStates|(1<<0);
	}
	if(digitalRead(11)==HIGH) {
		pinStates = pinStates|(1<<1);
	}

	if(pinStates==1) {
		Serial.println("Charge");
		docharge=1;
		Serial.println(docharge);
		delay(500);
	}

	if(pinStates==2) {
		Serial.println("Ignition");
		docharge=0;
		digitalWrite(8,LOW);
		digitalWrite(6,HIGH);
		delay(10);
		digitalWrite(6,LOW);
		delay(2000);
	}
	
	chargCap();

	static uint32_t nextLEDEvent;
	static uint8_t ledOn = false;

	if(millis()>nextLEDEvent && ledOn) {
		digitalWrite(7,HIGH);
		nextLEDEvent = millis()+500UL;
		ledOn = false;
	}

	if(millis()>nextLEDEvent && !ledOn) {
		digitalWrite(7,LOW);
		nextLEDEvent = millis()+500UL;
		ledOn = true;
	}	
}

void SERIAL__Parser() {
	static uint8_t cmd[32];         // command buffer
	static uint8_t cmdcount = 0;    // position in the buffer of the received byte
	uint8_t c;                      // received byte
	while(Serial.available()) {
		c = Serial.read();
		if(c > ' ') cmd[cmdcount++] = c;
		if((c == 8) && (cmdcount > 0)) cmdcount--;                // deals with backspaces, if a person on the other side types 
		if((c == 0x0d) || (c == 0x0a) || (cmdcount > 32)) {       // end of command, gets interpreted now
			cmd[cmdcount] = 0;    // clear the last byte in cmd buffer
			if(cmdcount > 2) {    // prevent empty cmd buffer parsing
				switch(cmd[0]) {
					case 'e': // execute
					SERIAL__ParserExecute(&cmd[1],cmdcount);
					break;           
				}    
			}
			cmdcount = 0;
		} 
	}
}

void IGNITION_request_charging(long tmpLong) {
	targetvoltage = (uint16_t)tmpLong;
	docharge = 1;
}




void SERIAL__ParserExecute(char * buf,uint8_t cnt) {
	long tmpLong;
	cnt-=4;
	switch (buf[0]) {
		
		case 'i':
			if(buf[1]=='c') { // wct set arc capcitor charg level
				buf[0] = ' ';
				buf[1] = ' ';
				tmpLong = atoi((const char*)&buf[2]);
				Serial.print("IG CHARG REQUEST ACK ");
				Serial.println(tmpLong);
				IGNITION_request_charging(tmpLong);
			}
			
			if(buf[1]=='a') { 
				Serial.println("ABORT");
			}

			if(buf[1]=='i') {
				IGNITION_request_charging(targetvoltage);
				ingition_requested = true;
			}
			
		break;

	}
}

void chargCap() {
	
	uint16_t current_cap_voltage_raw_analog = analogRead(A2);
	uint16_t current_cap_voltage = map(current_cap_voltage_raw_analog, 0, 1023, 0, CHARGEMONITOR_VOLTAGE_MAX_CHARGE);
	
	Serial.println(current_cap_voltage);

	if(docharge==1) {
		if(targetvoltage>current_cap_voltage) {
			Serial.print("target v: ");
			Serial.print(targetvoltage);
			Serial.print("  charg voltage level: ");
			Serial.println(current_cap_voltage);
			digitalWrite(8,HIGH);
		} else {
			if(ingition_requested) {
				ingition_requested = false;
				digitalWrite(6,HIGH);
				delay(10);
				digitalWrite(6,LOW);
				Serial.println("REMOTE IGNITION");
			}
			docharge = 0;
			Serial.print("charged ");
			Serial.println(current_cap_voltage);
			digitalWrite(8,LOW);
		}	
	}
	
}