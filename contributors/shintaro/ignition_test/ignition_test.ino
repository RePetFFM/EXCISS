void setup() {
	pinMode(11, INPUT_PULLUP); // charge
	pinMode(10, INPUT_PULLUP); // ignition

	pinMode(7, OUTPUT); // charge
	pinMode(8, OUTPUT); // ignition
	Serial.begin(9600);
}

uint8_t docharge = 0;

void loop() {
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
		digitalWrite(7,HIGH);
		delay(10);
		digitalWrite(7,LOW);
	}
	
	chargCap();
}

void chargCap() {
	static uint16_t targetvoltage = 500;

	if(docharge==1) {
		if(targetvoltage>analogRead(A2)) {
			Serial.print("charg ");
			Serial.println(analogRead(A2));
			digitalWrite(8,HIGH);
			delay(20);
			digitalWrite(8,LOW);
			delay(100);
		} else {
			Serial.print("charged ");
			Serial.println(analogRead(A2));
			digitalWrite(8,HIGH);
		}	
	}
	
}