
#define POWERLED_FRONT 5
#define POWERLED_BACK 6

void setup() {
	pinMode(POWERLED_FRONT, OUTPUT);
	pinMode(POWERLED_BACK, OUTPUT);
}


void loop() {
	analogWrite(POWERLED_FRONT, 120);
	analogWrite(POWERLED_BACK, 120);
}