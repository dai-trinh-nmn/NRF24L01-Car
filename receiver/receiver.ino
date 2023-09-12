/* ARDUINO CAR USING NRF24L01 AND JOYSTICK - RECEIVER
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
 ****************************************************
 *  L298N   ||    ARDUINO
 *   ENA    ->      6
 *   ENB    ->      5
 *   IN1    ->      7
 *   IN2    ->      8
 *   IN3    ->      3
 *   IN4    ->      4
 */
 
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

const int enA = 6;
const int IN1 = 7;
const int IN2 = 8;
const int enB = 5;
const int IN3 = 3;
const int IN4 = 4;

const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};
RF24 radio(9, 10);

int motorA_Speed = 0;
int motorB_Speed = 0;

struct Received_value {
  int xAxis;
  int yAxis;
  boolean KEY;
};

Received_value received_data;

void setup() {
  Serial.begin(9600);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1, pipes[1]);
  radio.openWritingPipe(pipes[0]);
  radio.startListening();

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}

void loop() {
  if (radio.available()) {
    radio.read(&received_data, sizeof(Received_value));
    delay(10);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(enA, 0);
    analogWrite(enB, 0);
  }

  int VRX = received_data.xAxis;
  int VRY = received_data.yAxis;
  boolean SW = received_data.KEY;
  
  //xAxis | Forward & Backward
  if (VRX <= 470) {
    //Backward - Motor A
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    //Backward - Motor B
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    motorA_Speed = map(VRX, 470, 0, 0, 255);
    motorB_Speed = map(VRX, 470, 0, 0, 255);
  } else if (VRX >= 550) {
    //Forward - Motor A
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    //Forward - Motor B
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    motorA_Speed = map(VRX, 550, 1023, 0, 255);
    motorB_Speed = map(VRX, 550, 1023, 0, 255);
  } else {
    motorA_Speed = 0;
    motorB_Speed = 0;
  }
  if (VRY <= 470) {
    int yMapped = map(VRY, 470, 0, 0, 255);
    //Turn Left | Increase Motor A Speed, Decrease Motor B Speed
    motorA_Speed = motorA_Speed + yMapped;
    motorB_Speed = motorB_Speed - yMapped;
    //Limit Speed (0 - 255)
    if (motorA_Speed > 255) {
      motorA_Speed = 255;
    }
    if (motorB_Speed < 0) {
      motorB_Speed = abs(motorB_Speed);
    }
    //Forward - Motor A
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    //Backward - Motor B
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  }
  if (VRY >= 550) {
    int yMapped = map(VRY, 550, 1023, 0, 255);
    //Turn Right | Increase Motor B Speed, Decrease Motor B Speed
    motorA_Speed = motorA_Speed - yMapped;
    motorB_Speed = motorB_Speed + yMapped;
    if (motorA_Speed < 0) {
      motorA_Speed = abs(motorA_Speed);
    }
    if (motorB_Speed > 255) {
      motorB_Speed = 255;
    }
    //Backward - Motor A
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    //Forward - Motor B
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }
  if (motorA_Speed < 100) {
    motorA_Speed = 0;
  }
  if (motorB_Speed < 100) {
    motorB_Speed = 0;
  }
  analogWrite(enA, motorA_Speed);
  analogWrite(enB, motorB_Speed);
}
