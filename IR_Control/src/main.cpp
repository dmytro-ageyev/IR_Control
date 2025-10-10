#include <Arduino.h>
#include <Servo.h>
#include <IRremote.hpp>   // Версія 4.5.0
#define IR_RECEIVE_PIN 2

// === Константи та глобальні змінні ===
#define LED_PIN 13
#define SERVO_PIN 9

Servo myServo;

int servoAngle = 90;       // Початковий кут сервоприводу
bool ledState = false;     // Поточний стан LED
int currentMode = 0;       // 0 - Монітор, 1 - LED, 2 - Servo, 3 - Сервіс

// --- Коди кнопок пульта ---
#define BTN_STAR 22  // *
#define BTN_HASH 13  // #

// --- Прототипи функцій ---
void PrintMenu();
void HandleSerialInput();
void HandleIRCommand();
bool InitSystem();
void PrintAction(String msg);
void ControlLED(unsigned long code);
void ControlServo(unsigned long code);
String GetSystemTime();
void PrintServiceStatus();

// === Функція setup() ===
void setup() {
  Serial.begin(9600);
  delay(300);
  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ ));
  Serial.println(F("Using library version " VERSION_IRREMOTE));

  // --- ЛОГО та банер ---
  Serial.println();
  Serial.println(F("================================================================="));
  Serial.println(F("                    Robocode/OS v.1.0 – Pro.Embedded"));
  Serial.println(F("================================================================="));
  delay(200);

  Serial.println(F(" Платформа : Arduino Uno"));
  Serial.println(F(" Ядро      : AVR @16MHz"));
   Serial.println(F("----------------------------------------------------------------"));

  if (!InitSystem()) {
    Serial.println(F("\n[ !!!!  ] Критична помилка ініціалізації. Перезапустіть пристрій."));
    while (true); // Зупинка програми при помилці
  }

  Serial.println(F("[  OK  ] Ініціалізацію завершено успішно!"));
  Serial.println(F("----------------------------------------------------------------"));
  Serial.println(F(" Стан  : Система готова до роботи"));
  Serial.println(F("----------------------------------------------------------------\n"));

  PrintMenu();
}

// === Основна петля програми ===
void loop() {
  HandleSerialInput();
  HandleIRCommand();
}

// === Функція ініціалізації системи ===
bool InitSystem() {
  bool initOK = true;
  
  Serial.println(F("[ BOOT ] Ініціалізація системних модулів Robocode/OS..."));

  Serial.println(F("\t Ініціалізація Serial... "));
  delay(200);

  if (Serial) Serial.println(F("[  OK  ] Проініціалізовано Serial."));
  else { Serial.println(F("[FAILED] Сбій ініціалізації Serial !!!")); initOK = false; }

  Serial.println(F("\t Запуск приймача ІЧ-сигналів... "));
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK); // ENABLE_LED_FEEDBACK);
  delay(200);
  Serial.print(F("\t Готов до прийому ІЧ-сигналів на порту "));
  // printActiveIRProtocols(&Serial);
  // Serial.println();
  Serial.println(IR_RECEIVE_PIN);
  if (IrReceiver.isIdle()) Serial.println(F("[  OK  ] Запущено приймач ІЧ-сигналів."));
  else Serial.println(F("[FAILED] Сбій запуску приймача ІЧ-сигналів !!!"));

  Serial.println(F("\t Ініціалізація сервоприводу... "));
  myServo.attach(SERVO_PIN);
  delay(200);
  if (myServo.attached()) {
    myServo.write(servoAngle);
    Serial.print(F("\t Сервопривод готов до роботи на порту "));
    Serial.println(SERVO_PIN);
    Serial.print(F("\t Сервопривод розгорнуто на кут "));
    Serial.println(String(servoAngle) + "°");
    Serial.println(F("[  OK  ] Проініціалізовано сервопривод."));
  } else { Serial.println(F("[FAILED] Сбій ініціалізації сервоприводу !!!")); initOK = false; }

  Serial.println(F("\t Налаштування порту LED... "));
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
  delay(200);
  Serial.print  (F("\t Світлодіод готов до роботи на порту "));
  Serial.println(LED_PIN); 
  Serial.println(  "\t Світлодіод " + String(ledState ? "УВІМКНЕНО" : "ВИМКНЕНО"));
  Serial.println(F("[  OK  ] Налаштовано порт світлодіоду."));
  
  return initOK;
}

// === Отримати системний час у форматі [HH:MM:SS] ===
String GetSystemTime() {
  String res;
  
  unsigned long seconds = millis() / 1000;
  unsigned int h = seconds / 3600;
  unsigned int m = (seconds % 3600) / 60;
  unsigned int s = seconds % 60;
  char buf[10];
  sprintf(buf, "%02u:%02u:%02u", h, m, s);
  res = String(buf);
  
  return res;
}

// === Вивід меню ===
void PrintMenu() {
  
  Serial.println(F("\n=============== ГОЛОВНЕ МЕНЮ ==============="));
  Serial.print  (F("[ INFO ] Uptime : "));
  Serial.println(GetSystemTime()); 
  Serial.println(F("[ INFO ] Виберіть режим:\n"));
  Serial.print  (currentMode == 0 ? "  [1]   " : "   1    " );
  Serial.println(F(" Режим монітору кнопок пульта"));
  Serial.print  (currentMode == 1 ? "  [2]   " : "   2    " );
  Serial.println(F(" Керування світлодіодом"));
  Serial.print  (currentMode == 2 ? "  [3]   " : "   3    " );
  Serial.println(F(" Керування сервоприводом"));
  Serial.print  (currentMode == 3 ? "  [4]   " : "   4    " );
  Serial.println(F(" Сервісний режим (діагностика системи)"));
  Serial.println(F("============================================\n"));
}

// === Вивід дії у форматі системного логу ===
void PrintAction(String msg) {
  Serial.print(GetSystemTime());
  Serial.print(F(" > [ACTION] "));
  Serial.println(msg);
}

// === Обробка введення з терміналу ===
void HandleSerialInput() {
  if (Serial.available()>0) {
    char input = Serial.read();
    switch (input) {

      case '0':
        PrintMenu();
        break;
      case '1':
        currentMode = 0;
        PrintAction(F("Режим: Моніторинг ІЧ-кнопок."));
        break;
      case '2':
        currentMode = 1;
        PrintAction(F("Режим: Керування світлодіодом."));
        break;
      case '3':
        currentMode = 2;
        PrintAction(F("Режим: Керування сервоприводом."));
        break;
      case '4':
        currentMode = 3;
        PrintAction(F("Режим: Сервісний режим (діагностика)."));
        PrintServiceStatus();
        break;
      default:
        Serial.print(GetSystemTime());
        Serial.println(F(" > [ERROR ] Невідомий режим. Спробуйте ще раз."));
        break;
    }
  }
}

// === Обробка сигналів пульта ===
void HandleIRCommand() {
  unsigned long code = 0;
  if (IrReceiver.decode()) {

    /*
       Print a summary of received data
    */
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
      PrintAction(F("Received noise or an unknown (or not yet enabled) protocol"));
      // We have an unknown protocol here, print extended info
      IrReceiver.printIRResultRawFormatted(&Serial, true);

      IrReceiver.resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
    } else {
      code = IrReceiver.decodedIRData.command;
      Serial.print(GetSystemTime());
      Serial.print(F(" > [  IR  ] Отримано код: "));
      Serial.print(code);
      Serial.print(F(" >> "));
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.printIRSendUsage(&Serial);
      IrReceiver.resume(); // Early enable receiving of the next IR frame
    }
   // Serial.println();

    /*
       Finally, check the received data and perform actions according to the received command
    */
    if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
      Serial.print(GetSystemTime());
      Serial.println(F(" > [  IR  ] Repeat received."));

    } else {
      Serial.print(GetSystemTime());
      Serial.print(F(" > [  IR  ] Отримано код: "));
      Serial.print(code);
      Serial.print(F(" >> "));
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    
    }
  }

    switch (currentMode) {
      case 0: break; // Монітор кодів
      case 1: ControlLED(code); break;
      case 2: ControlServo(code); break;
      case 3: break;
    }
//    irrecv.resume(); // Готовність до наступного сигналу
}

// === Керування LED ===
void ControlLED(unsigned long code) {
  if (code == BTN_STAR) {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
    PrintAction(F("Світлодіод УВІМКНЕНО (*)."));
  } else if (code == BTN_HASH) {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
    PrintAction(F("Світлодіод ВИМКНЕНО (#)."));
  }
}

// === Керування сервоприводом ===
void ControlServo(unsigned long code) {
  if (code == BTN_STAR) {
    servoAngle -= 3;
    if (servoAngle < 0) servoAngle = 0;
    myServo.write(servoAngle);
    PrintAction("Сервопривід зменшено на 3° (" + String(servoAngle) + "°).");
  } else if (code == BTN_HASH) {
    servoAngle += 3;
    if (servoAngle > 180) servoAngle = 180;
    myServo.write(servoAngle);
    PrintAction("Сервопривід збільшено на 3° (" + String(servoAngle) + "°).");
  }
  // code = 0; // Скидання коду після обробки
}

void dumpProtocols() {

    Serial.println();
    Serial.print(F("IR PROTOCOLS  "));
    Serial.print(F("SEND     "));
    Serial.println(F("DECODE"));
    Serial.print(F("============= "));
    Serial.print(F("======== "));
    Serial.println(F("========"));
    Serial.print(F("RC5:          "));
#if defined(DECODE_RC5)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("RC6:          "));
#if defined(DECODE_RC6)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("NEC:          "));
#if defined(DECODE_NEC)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("SONY:         "));
#if defined(DECODE_SONY)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("PANASONIC:    "));
#if defined(DECODE_PANASONIC)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("JVC:          "));
#if defined(DECODE_JVC)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("SAMSUNG:      "));
#if defined(DECODE_SAMSUNG)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("LG:           "));
#if defined(DECODE_LG)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("DENON:        "));
#if defined(DECODE_DENON)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

#if !defined(EXCLUDE_EXOTIC_PROTOCOLS) // saves around 2000 bytes program memory

    Serial.print(F("BANG_OLUFSEN: "));
#if defined(DECODE_BEO)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("BOSEWAVE:     "));
#if defined(DECODE_BOSEWAVE)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("WHYNTER:      "));
#if defined(DECODE_WHYNTER)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif

    Serial.print(F("FAST:         "));
#if defined(DECODE_FAST)
    Serial.println(F("Enabled"));
#else
    Serial.println(F("Disabled"));
#endif
#endif
}

// === Сервісний режим ===
void PrintServiceStatus() {

  Serial.println(F("\n=============== SERVICEMODE ==============="));
  Serial.println(F("[ INFO ]        Статус системи"));

  String ledStatus = digitalRead(LED_PIN) ? "УВІМКНЕНО" : "ВИМКНЕНО";
  Serial.println(  " LED          : Порт -> " + String(LED_PIN));
  Serial.println(  " LED          : " + ledStatus);
  Serial.println(F(" Servo        : Підключено"));
  Serial.println(  " Servo        : Кут = " + String(servoAngle) + "°");
  Serial.println(  " IR Receiver  : " + String(IrReceiver.isIdle() ? "Готовий" : "Помилка"));
  Serial.println(  " IR Receiver  : Порт -> " + String(IR_RECEIVE_PIN));

  Serial.println(F(" Servo Lib    : 1.2.2"));
  Serial.println(F(" IRremote Lib : " VERSION_IRREMOTE));
  Serial.print  (F(" Arduino Core : AVR @ "));
  Serial.print  (F_CPU / 1000000);
  Serial.println(F(" MHz"));

  Serial.println(" Uptime       : " + GetSystemTime());

  Serial.println(F("==========================================\n"));
  dumpProtocols();
//  PrintMenu();
}
