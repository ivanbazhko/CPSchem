// группа 150501, Божко И.И.
// Микропроцессорное устройство контроля параметров супермаркета
// Подключение библиотек
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <AmperkaKB.h>

#define DHTPIN 5 // вывод для подключения датчика DHT22
#define LIGHT_SENSOR A0 // вывод для подключения датчика LM393
#define RAIN_SENSOR A2 // вывод для подключения датчика MH-RD
#define PIN_RELAY 6 // вывод для подключения реле

// присваиваем имя dht для датчика DHT22
DHT dht(DHTPIN, DHT22); 
// присваиваем имя KB для клавиатуры
AmperkaKB KB(13, 12, 11, 10, 9, 8, 7); 
// присваиваем имя LCD для дисплея
LiquidCrystal_I2C LCD(0x27,16,2); 

// переменная для выбора режима работы
int printingNow = 1;
// минимальное допустимое значение температуры
int limitT = 15; 
// максимальное допустимое значение уровня утечки
int limitR = 50; 
// минимальное допустимое значение освещённости
int limitL = 200; 
// включение/выключение звукового сигнала
int siren = 1;

// функция мигания светодиодом
void blinkLed(int a, int delayed, int enddelay) {
  digitalWrite(a, HIGH); // включение светодиода
  delay(delayed); // задержка
  digitalWrite(a, LOW); // выключение светодиода
  if(enddelay) delay(delayed); // задержка
}

// функция инициализации переменных
void setup() {
  // инициализация вывода для подключения реле
  pinMode(PIN_RELAY, OUTPUT);
  dht.begin(); // инициализация датчика DHT22
  KB.begin(KB4x3); // инициализация клавиатуры
  LCD.init(); // инициализация LCD дисплея
  LCD.backlight(); // включение подсветки дисплея
  // инициализация выводов для подключения светодиодов
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  // мигание светодиодами для индикации включения
  blinkLed(4, 100, 1);
  blinkLed(2, 100, 1);
  blinkLed(3, 100, 1);
  blinkLed(2, 100, 1);
  blinkLed(4, 100, 1);
  blinkLed(2, 100, 1);
  blinkLed(3, 100, 1);
  Serial.begin(9600);
}

// основной цикл программы
void loop() {
  // вспомогательный цикл для регулировки частоты снятия показаний
  for(int i = 0; i < 20; i++) {
    delay(100); // задержка 100мс
    // чтение показаний с датчиков
    if(i == 10) {
      // переменные для отслеживания превышения допустимых значений
      int alertT = 0;
      int alertR = 0;
      int alertL = 0;
      // чтение показаний с датчиков
      float temp = dht.readTemperature();
      float fleak = analogRead(RAIN_SENSOR);
      int leak = round(100.0 - (fleak * (100.0 / 1023.0)));
      float flight = analogRead(LIGHT_SENSOR);
      int light = round(1000 - (flight * 1000.0 / 1023.0));
      if(isnan(temp)) {
        for(int k = 0; k < 10; k++) { // индикация ошибки
          blinkLed(4, 100, 1); // мигание зелёным светодиодом
        };
      };
      Serial.print("-------------------------------------------------\n");
      Serial.print("Temp: ");
      Serial.print(temp);
      Serial.print(" Light: ");
      Serial.print(flight);
      Serial.print(" Leak: ");
      Serial.print(fleak);
      Serial.print("\n");
      Serial.print("Temp: ");
      Serial.print(temp);
      Serial.print(" Light: ");
      Serial.print(light);
      Serial.print(" Leak: ");
      Serial.print(leak);
      Serial.print("\n");
      Serial.print("Temp limit: ");
      Serial.print(limitT);
      Serial.print("\n");
      Serial.print("Light limit: ");
      Serial.print(limitL);
      Serial.print("\n");
      Serial.print("Leak limit: ");
      Serial.print(limitR);
      Serial.print("\n");
      Serial.print("Printing: ");
      Serial.print(printingNow);
      Serial.print("\n");
      Serial.print("-------------------------------------------------\n");
      // вывод значения температуры на экран
      if(printingNow == 1) {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("TEMPERATURE:");
        LCD.setCursor(14, 0);
        LCD.print(limitT);
        LCD.setCursor(0, 1);
        LCD.print(temp);
        LCD.setCursor(6, 1);
        LCD.print((char)223);
        LCD.print("C");
      };
      // вывод значения освещённости на экран
      if(printingNow == 2) {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("LIGHT:");
        LCD.setCursor(13, 0);
        LCD.print(limitL);
        LCD.setCursor(0, 1);
        LCD.print(light);
        LCD.setCursor(6, 1);
        LCD.print("lx");
      };
      // вывод значения уровня утечки на экран
      if(printingNow == 3) {
        LCD.clear();
        LCD.setCursor(0, 0);
        LCD.print("LEAK:");
        LCD.setCursor(13, 0);
        LCD.print(limitR);
        LCD.setCursor(0, 1);
        LCD.print(leak);
      }
      // проверка значения температуры
      if(temp < limitT) {
        alertT = 1;
        Serial.print("Temp alert\n");
      };
      // проверка значения уровня утечки
      if(leak > limitR) {
        alertR = 1;
        Serial.print("Leak alert\n");
      };
      // проверка значения освещённости
      if(light < limitL) {
        alertL = 1;
        Serial.print("Light alert\n");
      };
      // включение органов индикации при отклонении от нормы
      if(alertT || alertR || alertL) {
        // включение динамика
        if(siren == 1) {
          digitalWrite(PIN_RELAY, HIGH);
          Serial.print("Signal\n");
        };
        // включение зелёного светодиода
        if(alertT) {
          digitalWrite(4, HIGH); 
          Serial.print("Green LED\n");
        };
        // включение белого светодиода
        if(alertR) {
          digitalWrite(3, HIGH);
          Serial.print("White LED\n");
        }; 
        // включение оранжевого светодиода
        if(alertL) {
          digitalWrite(2, HIGH); 
          Serial.print("Orange LED\n");
        };
        delay(300); // задержка 300мс
        digitalWrite(PIN_RELAY, LOW); // выключение динамика
        // выключение зелёного светодиода
        digitalWrite(4, LOW); 
        // выключение белого светодиода
        digitalWrite(3, LOW); 
        // выключение оранжевого светодиода
        digitalWrite(2, LOW); 
      };
    }
    // чтение состояния клавиатуры
    KB.read();
    if (KB.isHold()) {
      Serial.print("KB Hold: ");
      Serial.print(KB.getNum);
      Serial.print("\n");
      if(KB.getNum == 0) { 
        if(siren == 1) {
          siren = 0;
          LCD.clear();
          LCD.setCursor(0, 0);
          LCD.print("SOUND OFF");
          delay(1000);
        }
        else {
          siren = 1;
          LCD.clear();
          LCD.setCursor(0, 0);
          LCD.print("SOUND ON");
          delay(1000);
        }
      }
    }
    if (KB.justPressed()) { // нажата кнопка
      Serial.print("KB Pressed: ");
      Serial.print(KB.getNum);
      Serial.print("\n");
      if(KB.getNum == 15) { // нажата кнопка #
        // переключение в режим вывода освещённости
        if(printingNow == 1) { 
          printingNow = 2;
          for(int k = 0; k < 3; k++) { // индикация переключения
            blinkLed(2, 100, 1); // мигание оранжевым светодиодом
          };
        }
        // переключение в режим вывода уровня утечки
        else if(printingNow == 2) { 
          printingNow = 3;
          for(int k = 0; k < 3; k++) { // индикация переключения
            blinkLed(3, 100, 1); // мигание белым светодиодом
          };
        }
        // переключение в режим вывода температуры
        else { 
          printingNow = 1;
          for(int k = 0; k < 3; k++) { // индикация переключения
            blinkLed(4, 100, 1); // мигание зелёным светодиодом
          };
        };
      }
      if(KB.getNum == 14) { // нажата кнопка *
      // новое значение допустимого значения
        int newNum = 0; 
        // 10с на ввод нового значения
        for(int j = 0; j < 100; j++) { 
          delay(100); // задержка 100мс
          KB.read(); // чтение значения с клавиатуры
          if (KB.justPressed()) { // нажата кнопка
            // нажата кнопка * - подтверждение ввода
            if(KB.getNum == 14) break; 
            // добавление новой цифры
            if(KB.getNum != 14 && KB.getNum != 15) { 
              newNum = newNum * 10 + KB.getNum;
            }
          };
        };
        // изменение допустимых значений температуры
        if(printingNow == 1) { 
          // проверка правильности ввода
          if(newNum >= 0 && newNum <= 80) {
            limitT = newNum; // установка нового значения
            LCD.clear();
            LCD.setCursor(0, 0);
            LCD.print("NEW TEMP:");
            LCD.setCursor(0, 1);
            LCD.print(newNum);
            for(int k = 0; k < 3; k++) { // индикация изменения
              // мигание зелёным светодиодом
              blinkLed(4, 100, 1); 
            };
          } else {
            LCD.clear();
            LCD.setCursor(0, 0);
            LCD.print("INVALID");
            LCD.setCursor(0, 1);
            LCD.print("INPUT");
            delay(1000);
          };
        // изменение допустимых значений освещённости
        } else if(printingNow == 2) {
          // проверка правильности ввода
          if(newNum > 0 && newNum < 5000) {
            limitL = newNum; // установка нового значения
            LCD.clear();
            LCD.setCursor(0, 0);
            LCD.print("NEW LIGHT:");
            LCD.setCursor(0, 1);
            LCD.print(newNum);
            for(int k = 0; k < 3; k++) { // индикация изменения
              // мигание оранжевым светодиодом
              blinkLed(2, 100, 1); 
            };
          } else {
            LCD.clear();
            LCD.setCursor(0, 0);
            LCD.print("INVALID");
            LCD.setCursor(0, 1);
            LCD.print("INPUT");
            delay(1000);
          };
        // изменение допустимых значений уровня утечки
        } else if(printingNow == 3) {
          // проверка правильности ввода
          if(newNum > 0 && newNum < 100) { 
            limitR = newNum; // установка нового значения
            LCD.clear();
            LCD.setCursor(0, 0);
            LCD.print("NEW LEAK:");
            LCD.setCursor(0, 1);
            LCD.print(newNum);
            for(int k = 0; k < 3; k++) { // индикация изменения
              // мигание белым светодиодом
              blinkLed(3, 100, 1); 
            };
          } else {
            LCD.clear();
            LCD.setCursor(0, 0);
            LCD.print("INVALID");
            LCD.setCursor(0, 1);
            LCD.print("INPUT");
            delay(1000);
          };
        };
      }
    }
  }
}
