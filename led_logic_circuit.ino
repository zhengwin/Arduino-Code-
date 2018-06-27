#include <Arduino.h>
const int ledPin = 9;
const int ledPin2 = 10;
const int input = 2;
int ledPinState = HIGH;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(input, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(input) == HIGH) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin2, LOW);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);
    } else {
      delay(100);
      digitalWrite(ledPin2, HIGH);

      delay(200);
      digitalWrite(ledPin2, LOW);

      delay(200);
      digitalWrite(ledPin2, HIGH);

      delay(100);
      if (ledPinState == HIGH) {
          digitalWrite(ledPin, LOW);
          ledPinState = LOW;
        } else {
          digitalWrite(ledPin, HIGH);
          ledPinState = HIGH;
          }
      }
}
