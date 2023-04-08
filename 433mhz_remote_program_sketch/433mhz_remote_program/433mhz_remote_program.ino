#include <RCSwitch.h>

RCSwitch tx = RCSwitch();

void setup() {
  
  Serial.begin(9600);
  
  // digital I/O pin 8
  tx.enableTransmit(8);

  // these options from receive logs of remotes
  tx.setProtocol(1);
  // "native" pulse length of our remotes is 340ms, we can go as short as 150ms,
  // which we want to, to reduce jamming
  tx.setPulseLength(150); 

  // not sure now many repetitions we really need
  tx.setRepeatTransmit(1000);


}

void loop() {
  // put your main code here, to run repeatedly:

  // codes are 24 bits

  // first 12 bits identifies the ref, 
  // next 12 bits identifies the button

  // head ref clear button
  //tx.send("000000000001000000000001");
  // head ref red button
  //tx.send("000000000001000000000010");
  // head ref white button
  //tx.send("000000000001000000000100");
  // left ref red button
  //tx.send("000000000010000000000010");
  // left ref white button
  //tx.send("000000000010000000000100");
  // right ref red button
  //tx.send("000000000100000000000010");
  // right ref white button
  tx.send("000000000100000000000100");

}
