// Programm for Teensy 3.2

const int in_button1 = 7;         // Button       
const int out_LED1 = 3;           // Green LED
const int out_LED2 = 10;          // Red LED

 int i_buttonState1 = 0;          // State of button
 int i_igniteCoilState = 0;       // State of ignite coil
 int i_charge_CapState = 0;       // State of charge capazitor
 
 int charge_Cap = 9;              // cable bruin
 int ignite_Coil = 8;             // cable green
 int charge_Value = 14;           // cable grey

 int power = 0;            
 int increase = 1;

 String Start; 

void setup(){
  
  Start = Serial.readString();
  Serial.println(" Start: Press Key und Enter ");
      while (Serial.available()==0) {
      } 
  
  
  pinMode(in_button1, INPUT);
  pinMode(out_LED1, OUTPUT);
  pinMode(out_LED2, OUTPUT);
  pinMode(charge_Cap, INPUT_PULLDOWN);
  pinMode(ignite_Coil, INPUT_PULLDOWN);

  pinMode(charge_Value, OUTPUT);

delay (5000);
}

int Intensity = 0;

void loop(){

 analogWrite(charge_Value, power);            // count from zero
      Serial.print("charge_Value = ");
      Serial.println(power);
  
 i_buttonState1 = digitalRead(in_button1);
 i_igniteCoilState = digitalRead(ignite_Coil);
 i_charge_CapState = digitalRead(charge_Cap);

  if (i_igniteCoilState == HIGH ){
      analogWrite(charge_Value, power);
      power = 20;      
      Serial.print("ignite = ");
      Serial.println(power);

   }
 
 if (i_buttonState1 == HIGH) {
     digitalWrite(out_LED1, LOW);  // Green LED
     digitalWrite(out_LED2, HIGH);
     power = 20;
     Serial.print("Button pressed = ");
     Serial.println(power);
     
     delay (100);
     }
     
 else {
    digitalWrite(out_LED1, HIGH);
    digitalWrite(out_LED2, LOW);
    delay (100);
    }
  
  if (charge_Cap == HIGH || i_buttonState1 == HIGH)
     power = 20;
     power = power + increase;
    if (power == 0 || power >= 256) {
       power = 255;
       digitalWrite(out_LED2, LOW);       }
       Serial.print("");
       Serial.println(power);
       delay (1000);  
    }

