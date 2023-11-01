#include <DHT.h>
#include <AmperkaKB.h>

#define DHTPIN 5
#define LIGHT_SENSOR A0
#define RAIN_SENSOR A2

DHT dht(DHTPIN, DHT22);
AmperkaKB KB(13, 12, 11, 10, 9, 8, 7);

int printingNow = 1;
int limitT = 90;
int limitR = 500;
int limitL = 900;

void setup() {
  Serial.begin(9600);
  dht.begin();
  KB.begin(KB4x3);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(3, HIGH);
  delay(100);
  digitalWrite(3, LOW);
  delay(100);
  digitalWrite(4, HIGH);
  delay(100);
  digitalWrite(4, LOW);
  delay(100);
  digitalWrite(3, HIGH);
  delay(100);
  digitalWrite(3, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(3, HIGH);
  delay(100);
  digitalWrite(3, LOW);
  delay(100);
  digitalWrite(4, HIGH);
    delay(100);
  digitalWrite(4, LOW);
  delay(100);
}

void loop() {
  for(int i = 0; i < 20; i++) {
    delay(100);
    if(i == 10) {
      int alertT = 0;
      int alertR = 0;
      int alertL = 0;
      float temp = dht.readTemperature();
      float humi = dht.readHumidity();
      int leak = analogRead(RAIN_SENSOR);
      int light = analogRead(LIGHT_SENSOR);
      if(printingNow == 1) {
        Serial.print("Humidity: ");
        Serial.print(humi);
        Serial.print(" % | ");
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.print(" *C\n\n");
      };
      if(printingNow == 2) {
        Serial.print("Light: "); 
        Serial.print(light);
        Serial.print("\n\n");
      };
      if(printingNow == 3) {
        Serial.print("Leak: ");
        Serial.print(leak);
        Serial.print("\n\n");
      }
      if(humi > limitT) alertT = 1;
      if(leak < limitR) alertR = 1;
      if(light > limitL) alertL = 1;
      if(alertT || alertR || alertL) {
        tone(6, 100);
        if(alertT) digitalWrite(4, HIGH);
        if(alertR) digitalWrite(3, HIGH);
        if(alertL) digitalWrite(2, HIGH);
        delay(300);
        noTone(6);
        digitalWrite(4, LOW);
        digitalWrite(3, LOW);
        digitalWrite(2, LOW);
      };
    }
    KB.read();
    if (KB.justPressed()) {
      if(KB.getNum == 15) {
        if(printingNow == 1) {
          printingNow = 2;
          for(int k = 0; k < 3; k++) {
            digitalWrite(2, HIGH);
            delay(100);
            digitalWrite(2, LOW);
            delay(100);
          }
        }
        else if(printingNow == 2) {
          printingNow = 3;
          for(int k = 0; k < 3; k++) {
            digitalWrite(3, HIGH);
            delay(100);
            digitalWrite(3, LOW);
            delay(100);
          }
        }
        else {
          printingNow = 1;
          for(int k = 0; k < 3; k++) {
            digitalWrite(4, HIGH);
            delay(100);
            digitalWrite(4, LOW);
            delay(100);
          }
        };
      }
      if(KB.getNum == 14) {
        int newNum = 0;
        for(int j = 0; j < 100; j++) {
          delay(100);
          KB.read();
          if (KB.justPressed()) {
            if(KB.getNum == 14) break;
            if(newNum > 1023) break;
            if(KB.getNum != 14 && KB.getNum != 15) {
              newNum = newNum * 10 + KB.getNum;
            }
          };
        };
        if(printingNow == 1) {
          if(newNum > 0 && newNum <= 100) {
            limitT = newNum;
            Serial.print("Updated limitT to: ");
            Serial.print(limitT);
            Serial.print("\n\n");
            for(int k = 0; k < 3; k++) {
              digitalWrite(4, HIGH);
              delay(100);
              digitalWrite(4, LOW);
              delay(100);
            }
          }
        } else if(printingNow == 2) {
          if(newNum > 0 && newNum < 1024) {
            limitL = newNum;
            Serial.print("Updated limitL to: ");
            Serial.print(limitL);
            Serial.print("\n\n");
            for(int k = 0; k < 3; k++) {
              digitalWrite(2, HIGH);
              delay(100);
              digitalWrite(2, LOW);
              delay(100);
            }
          };
        } else if(printingNow == 3) {
          if(newNum > 0 && newNum < 1024) {
            limitR = newNum;
            Serial.print("Updated limitR to: ");
            Serial.print(limitR);
            Serial.print("\n\n");
            for(int k = 0; k < 3; k++) {
              digitalWrite(3, HIGH);
              delay(100);
              digitalWrite(3, LOW);
              delay(100);
            }
          };
        };
      }
    }
    // if (KB.justPressed()) {
    //   Serial.print("Key is pressed ");
    //   Serial.print(KB.getNum);
    //   Serial.print(" = \"");
    //   Serial.print(KB.getChar);
    //   Serial.println("\"");
    // }
    // if (KB.justReleased()) {
    //   Serial.print("Key is released ");
    //   Serial.print(KB.getNum);
    //   Serial.print(" = \"");
    //   Serial.print(KB.getChar);
    //   Serial.println("\"");
    // }
    // if (KB.isHold()) {
    //   Serial.print("Key on long press ");
    //   Serial.print(KB.getNum);
    //   Serial.print(" = \"");
    //   Serial.print(KB.getChar);
    //   Serial.println("\"");
    // }
  }
}
