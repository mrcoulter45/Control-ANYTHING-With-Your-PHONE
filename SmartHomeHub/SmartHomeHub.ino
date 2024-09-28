#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_MODULE
#include <Dabble.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(10, 9);               // nRF24L01 (CE,CSN)
RF24Network network(radio);      // Include the radio in the network
const uint16_t master_address = 0;
const uint16_t bed_lights_address = 1;
const uint16_t bb8_led_light_address = 2;
const uint16_t r2d2_led_light_address = 3;
RF24NetworkHeader *header_active_receiver;
RF24NetworkHeader header_bed_lights(bed_lights_address);
RF24NetworkHeader header_bb8_led_light(bb8_led_light_address);
RF24NetworkHeader header_r2d2_led_light(r2d2_led_light_address);

String terminal_input = "";
uint8_t cmd = 0;
const char help_msg[] PROGMEM = "Commands:\n\"bed\": set receiver to Bed Lights\n\"bb8\": set receiver to BB8 LED Light\n\"r2d2\": set receiver to R2D2 LED Light\n\"brighten\": brighten LEDS/quicken LED flash Speed\n\"dim\": dim LEDS/slow LED flash Speed\n\"off\": turn LEDS off\n\"on\": turn LEDS on\n\"r0\": red shade 0\n\"r1\": red shade 1\n\"r2\": red shade 2\n\"r3\": red shade 3\n\"r4\": red shade 4\n\"g0\": green shade 0\n\"g1\": green shade 1\n\"g2\": green shade 2\n\"g3\": green shade 3\n\"g4\": green shade 4\n\"b0\": blue shade 0\n\"b1\": blue shade 1\n\"b2\": blue shade 2\n\"b3\": blue shade 3\n\"b4\": blue shade 4\n\"white\": white\n\"flash\": flash\n\"strobe\": strobe\n\"fade\": fade\n\"smooth\": smooth\n\"r\": repeat last command\n\"help\": show this list of commands\n";

// given a PROGMEM string, use Terminal.print() to send it out to smart phone
void printProgStr(PGM_P str) {
  char c;
  String line = "";
  if(!str) return;
  while((c = pgm_read_byte(str++))) {
    if (c != '\n') {
      line += c;
    } else {
      Terminal.print(line);
      line = "";
    }
  }
}

void setup() {
  Serial.begin(9600);
  Dabble.begin(9600);
  delay(200);
  SPI.begin();
  radio.begin();
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  network.begin(90, master_address);  //(channel, node address)
  header_active_receiver = &header_bed_lights;
  Terminal.println("receiver set to bed");

  if (radio.isChipConnected()) {
    Terminal.println("chip is connected to the SPI bus");
  } else {
    Terminal.println("chip is not connected to the SPI bus");
  }
}

void loop() {
  Dabble.processInput();
  while (Terminal.available()) {
    terminal_input = Terminal.readString();
    Serial.println(terminal_input);
    if (terminal_input.equalsIgnoreCase("bed")) {
      header_active_receiver = &header_bed_lights;
      Terminal.println("receiver set to bed");
      break;
    } else if (terminal_input.equalsIgnoreCase("bb8")) {
      header_active_receiver = &header_bb8_led_light;
      Terminal.println("receiver set to bb8");
      break;
    } else if (terminal_input.equalsIgnoreCase("r2d2")) {
      header_active_receiver = &header_r2d2_led_light;
      Terminal.println("receiver set to r2d2");
      break;
    }

    else if (terminal_input.equalsIgnoreCase("brighten")) {
      cmd = 5;
    } else if (terminal_input.equalsIgnoreCase("dim")) {
      cmd = 4;
    } else if (terminal_input.equalsIgnoreCase("off")) {
      cmd = 6;
    } else if (terminal_input.equalsIgnoreCase("on")) {
      cmd = 7;
    } else if (terminal_input.equalsIgnoreCase("r0")) {
      cmd = 9;
    } else if (terminal_input.equalsIgnoreCase("r1")) {
      cmd = 13;
    } else if (terminal_input.equalsIgnoreCase("r2")) {
      cmd = 21;
    } else if (terminal_input.equalsIgnoreCase("r3")) {
      cmd = 25;
    } else if (terminal_input.equalsIgnoreCase("r4")) {
      cmd = 17;
    } else if (terminal_input.equalsIgnoreCase("g0")) {
      cmd = 8;
    } else if (terminal_input.equalsIgnoreCase("g1")) {
      cmd = 12;
    } else if (terminal_input.equalsIgnoreCase("g2")) {
      cmd = 20;
    } else if (terminal_input.equalsIgnoreCase("g3")) {
      cmd = 24;
    } else if (terminal_input.equalsIgnoreCase("g4")) {
      cmd = 16;
    } else if (terminal_input.equalsIgnoreCase("b0")) {
      cmd = 10;
    } else if (terminal_input.equalsIgnoreCase("b1")) {
      cmd = 14;
    } else if (terminal_input.equalsIgnoreCase("b2")) {
      cmd = 22;
    } else if (terminal_input.equalsIgnoreCase("b3")) {
      cmd = 26;
    } else if (terminal_input.equalsIgnoreCase("b4")) {
      cmd = 18;
    } else if (terminal_input.equalsIgnoreCase("white")) {
      cmd = 11;
    } else if (terminal_input.equalsIgnoreCase("flash")) {
      cmd = 15;
    } else if (terminal_input.equalsIgnoreCase("strobe")) {
      cmd = 23;
    } else if (terminal_input.equalsIgnoreCase("fade")) {
      cmd = 27;
    } else if (terminal_input.equalsIgnoreCase("smooth")) {
      cmd = 19;
    } else if (terminal_input.equalsIgnoreCase("r")) {
      cmd = cmd;
    } else if (terminal_input.equalsIgnoreCase("help")) {
      printProgStr(help_msg);
      break;
    } else { // invalid cmd
      Terminal.println("invalid cmd");
      break;
    }


    network.update();
    bool network_write_ok = network.write(*header_active_receiver, &cmd, sizeof(cmd));
    if (network_write_ok) {
      Terminal.println("cmd success");
    } else {
      Terminal.println("cmd failed, receiver did not receive cmd");
    }
  }
}
