
// Map 0-15 to the pin names of arduino
// [0,7] stores x pins, [8,15] stores y pins
byte btn_indices[16] = { A0, A1, A2, A3, A4, A5, A6, A7, 2, 3, 4, 5, 6, 7, 8, 9 };

// Return the pin number of any pressed button
// Returns -1 if no buttons are pressed 
int get_btn(){

  for(byte i = 0; i < 16; i++){
    if(digitalRead(btn_indices[i]) == LOW)
      return btn_indices[i];
  }

  // No buttons pressed
  return -1;
}

// Sets the state of every button IO pin to INPUT_PULLUP or OUTPUT
// 1 denotes INPUT_PULLUP, 0, denotes OUTPUT
void set_PINMODE(bool pinmode){

  // INPUT_PULLUP
  if(pinmode){
    for(byte i = 0; i < 16; i++){
      pinMode(btn_indices[i], INPUT_PULLUP);
    }
  }

  // OUTPUT
  else{
    for(byte i = 0; i < 16; i++){
      pinMode(btn_indices[i], OUTPUT);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  set_PINMODE(1);

//  for(int i = 0; i < 8; i++){
//    digitalWrite(btn_indices[i], LOW);
//    delay(50);
//    digitalWrite(btn_indices[i], HIGH);
//    delay(50);
//  }
}
