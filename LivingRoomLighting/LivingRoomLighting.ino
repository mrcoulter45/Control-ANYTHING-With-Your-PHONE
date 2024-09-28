#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

// NEC IR protocol: http://www.diegm.uniud.it/bernardini/Laboratorio_Didattico/2016-2017/2017-Telecomando/ir-protocols.html

// red:   F609FF00
// green: F708FF00
// blue:  F50AFF00
// white: F40BFF00
#define IR_SEND_PIN 3

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t bb8_led_light_address = 2;

void setup()
{
  Serial.begin(9600);
  pinMode(IR_SEND_PIN, OUTPUT);
  SPI.begin();
  radio.begin();
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  network.begin(90, bb8_led_light_address);  //(channel, node address)
  Serial.println("setup complete");
}
uint8_t addr = 0x0;
uint8_t cmd = 0x0;
uint8_t repeats = 0;
uint8_t cmds[16] = {9,8,10,11,13,12,14,21,20,22,25,24,26,17,16,18};
void loop()
{
  // Serial.println("sending cmds: ");
  for (uint8_t i = 0; i < 16; i += 1) {
    Serial.println(cmds[i], DEC);
    
    (addr, cmds[i]);
    delay(1000);
  }
  // ======
  // network.update();
  // //===== Receiving =====//
  // while (network.available()) {     // Is there any incoming data?
  //   Serial.println("data received...");
  //   // blinkDataLed();
  //   RF24NetworkHeader header;
  //   network.read(header, &cmd, sizeof(cmd)); // Read the incoming data
  //   Serial.println(cmd);
  //   Serial.println(cmd, DEC);
  //   ir_send(addr, cmd);
  // }
}
void ir_start() {
  digitalWrite(IR_SEND_PIN, 0);
  delay(9);
  digitalWrite(IR_SEND_PIN, 1);
  delay(4);
  delayMicroseconds(499);
}
void ir_end() {
  digitalWrite(IR_SEND_PIN, 0);
  delayMicroseconds(561);
  digitalWrite(IR_SEND_PIN, 1);
  delayMicroseconds(561);
}
void ir_send_logic_1() {
  digitalWrite(IR_SEND_PIN, 0);
  delayMicroseconds(561);
  digitalWrite(IR_SEND_PIN, 1);
  delayMicroseconds(1610);
}
void ir_send_logic_0() {
  digitalWrite(IR_SEND_PIN, 0);
  delayMicroseconds(563);
  digitalWrite(IR_SEND_PIN, 1);
  delayMicroseconds(530);
}
void ir_send_byte(uint8_t byte) {
  for (int i = 0; i < 8; i += 1) {
    if ((byte >> i) & 0x01) {
      ir_send_logic_1();
    } else {
      ir_send_logic_0();
    }
  }
}
void ir_send(uint8_t addr, uint8_t cmd) {
  ir_start();
  ir_send_byte(addr);
  ir_send_byte(~addr);
  ir_send_byte(cmd);
  ir_send_byte(~cmd);
  ir_end();
}
