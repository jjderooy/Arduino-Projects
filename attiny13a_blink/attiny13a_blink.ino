// Ok to make this work use connect an arduino nano or something to the AT Tiny13a. I was able to get away without using the 10uf
// cap but it is for stability so maybe I got lucky.

// Upload the "Arduino as ISP" sketch to the nano.

// Open this sketch and select "Arduino as ISP (MicroCore)" as the programmer under Tools

// Select "Burn Bootloader" (You only have to do this once for a chip, then it can be reprogrammed without reburning)

// Now upload this sketch by pressing the upload button.


void setup() {
  // put your setup code here, to run once:
  pinMode(4, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4, HIGH);
  delay(1000);
  digitalWrite(4, LOW);
  delay(1000);
}
