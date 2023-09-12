/* ARDUINO CAR USING NRF24L01 AND JOYSTICK - TRANSMITTER
 * CODE POWERED BY TRINH DUC DAI
 * (https://www.facebook.com/dai.trinh.pro)
 * WIRING
 * NRF24L01   ||    ARDUINO
 *  GND  (1)  ->      GND
 *  VCC  (2)  ->      VCC
 *  CE   (3)  ->      D9
 *  CSN  (4)  ->      D10
 *  CLK  (5)  ->      D13
 *  MOSI (6)  ->      D11
 *  MISO (7)  ->      D12
 */

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

const int X_PIN = A1;
const int Y_PIN = A0;
const int SW = 2;

const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};
RF24 radio(9, 10);

struct JS_value {
    int VRX;
    int VRY;
    boolean KEY;
};

JS_value sent_data;

void setup() {
  // put your setup code here, to run once:
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);
  pinMode(SW, INPUT_PULLUP);

  Serial.begin(9600);

  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1, pipes[0]);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(X_PIN));
  sent_data.VRX = analogRead(X_PIN);
  Serial.println(analogRead(Y_PIN));
  sent_data.VRY = analogRead(Y_PIN);
  sent_data.KEY = digitalRead(SW);
  radio.write(&sent_data, sizeof(JS_value));
}
