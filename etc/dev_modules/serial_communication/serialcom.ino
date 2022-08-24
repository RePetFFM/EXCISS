

void setup() {
	Serial.begin(9600);

	Serial.println("ready");
}


void loop() {
	serialParser();
}

void serialParser() {
	uint8_t c;	
	while(Serial.available()) {
		c = Serial.read();
		serialCMDParser(c);
	}
}


void serialCMDParser(char c) {
	static char buf[32];
	static uint8_t bufcnt = 0;
	static uint8_t bufcntcmd = 0;
	buf[bufcnt++] = c;
	if(c!=0x0a && c!=0x0d) bufcntcmd++;

	if((c == 0x0a) || (bufcnt >= 30)) { // (c == 0x0d) ||
		switch(buf[0]) {
			case 'w': // write
				serialParserWrite(&buf[1],bufcntcmd);
			break;
			case 'r': // read
				serialParserRead(&buf[1],bufcntcmd);
			break;
			case 'e': // execute
				serialParserExecute(&buf[1],bufcntcmd);
			break;
		}

		bufcnt = 0;
		bufcntcmd = 0;
	}
}


void serialParserRead(char * buf,uint8_t cnt) {
	cnt-=4;
	switch (buf[0]) {
		case 'd':
			if(buf[1]=='t') {
				Serial.println("dt");	
			}
		break;

		case 'h':
			Serial.println("h");
		break;
		
	}
}

void serialParserWrite(char * buf,uint8_t cnt) {
	long tmpLong = 0L;
	cnt-=4;
	switch (buf[0]) {
		case 'f': // set front LED PWM
			{
				buf[0] = ' ';
				tmpLong = atoi((const char*)&buf[1]);
				Serial.println(tmpLong);
			}
		break;

		case 'b': // set back LED PWM
			{
				buf[0] = ' ';
				tmpLong = atoi((const char*)&buf[1]);
				Serial.println(tmpLong);
			}
		break;

		case 'd':
			if(buf[1]=='t') {
				String str((const char*)&buf[2]);
				Serial.println(str);
			}
		break;
	}
}




void serialParserExecute(char * buf,uint8_t cnt) {
	cnt-=4;
	switch (buf[0]) {
		case 's':
			if(buf[1]=='e') {
				Serial.println("shake enabled");
			}

			if(buf[1]=='d') {
				Serial.println("shake disabled");
			}
		break;

		case 'r':
			Serial.println("raspberry power down requested");
		break;
	}
}


char readSingleHex(char c) {
	char cret = 0x00;
	if(c>='A' && c<='F') {
		cret = c-'A'+10;
	}
	if(c>='a' && c<='f') {
		cret = c-'a'+10;
	}
	if(c>='0' && c<='9') {
		cret = c-'0';
	}
	return cret;
}