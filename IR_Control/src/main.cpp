/**
 * @file IR_Remote_LED_Servo_Menu_Functions.ino
 * @brief Керування світлодіодом і сервоприводом через ІЧ-пульт з меню у Serial Monitor.
 *
 * Програма підтримує три режими:
 *   1️⃣ Моніторинг кнопок (вивід кодів у Serial Monitor)
 *   2️⃣ Керування вбудованим світлодіодом (кнопка '*' – УВІМКНУТИ, '#' – ВИМКНУТИ)
 *   3️⃣ Керування сервоприводом (кнопка '*' – зменшити кут на 3°, '#' – збільшити)
 *
 * Вибір режиму здійснюється через Serial Monitor.
 * Програма виводить у порт коди кнопок, зміну стану LED та кута сервоприводу.
 *
 * --- Підключення ---
 *  ІЧ-приймач (VS1838B або TSOP4838):
 *    - OUT → D2
 *    - VCC → 5V
 *    - GND → GND
 *
 *  Сервопривід:
 *    - Сигнал → D9
 *    - Живлення → 5V
 *    - Земля → GND
 *
 * --- Необхідні бібліотеки ---
 *  - <IRremote.h> (прийом сигналів пульта)
 *  - <Servo.h>    (керування сервоприводом)
 *
 * @author  Дмитро Агеєв
 * @date    09.10.2025
 * @license MIT
 */

#include <Arduino.h>
#include <IRremote.hpp>
#include <Servo.h>

// ----------------------------------------------------------
//                    Константи та змінні
// ----------------------------------------------------------

const int RECV_PIN = 2;   ///< Пін приймача ІЧ-пульта
const int LED_PIN = 13;   ///< Вбудований світлодіод
const int SERVO_PIN = 9;  ///< Пін сервоприводу
const int ANGLE_STEP = 3; ///< Крок зміни кута сервоприводу

//IRrecv irrecv(RECV_PIN);  ///< Об’єкт приймача ІЧ-сигналів
decode_results results;   ///< Збереження прийнятого коду
Servo myServo;            ///< Об’єкт сервоприводу

int menuMode = 0;         ///< Поточний режим (0 – моніторинг, 1 – LED, 2 – серво)
int servoAngle = 90;      ///< Поточний кут сервоприводу

// ----------------------------------------------------------
//                   ПРОТОТИПИ ФУНКЦІЙ
// ----------------------------------------------------------
void PrintMenu();
void HandleSerialInput();
void HandleIRInput();
void HandleLEDControl(uint16_t command);
void HandleServoControl(uint16_t command);
void PrintAngleChange(int angle);

// ----------------------------------------------------------
//                         SETUP()
// ----------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("====================================================");
  Serial.println("📡 Система керування через ІЧ-пульт і Serial Monitor");
  Serial.println("====================================================");
  Serial.println();
  Serial.println(">  Ініціалізація пристроїв: ");
  
  //irrecv.enableIRIn();
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("[+]  Ініціалізація вбудованого світлодіоду ");
  Serial.println("[+]  Ініціалізація IR-приймача ");
  if (myServo.attach(SERVO_PIN))
  {
    Serial.println("[+]  Ініціалізація сервоприводу ");
    myServo.write(servoAngle);
  }
  else Serial.println("[-]  Ініціалізація сервоприводу ");
  Serial.println();
  PrintMenu();
}

// ----------------------------------------------------------
//                          LOOP()
// ----------------------------------------------------------
void loop() {
  HandleSerialInput(); // Обробка вводу з терміналу
  HandleIRInput();     // Обробка команд із пульта
}

// ----------------------------------------------------------
//                    ОБРОБКА ВВОДУ З SERIAL
// ----------------------------------------------------------
/**
 * @brief Обробляє введення користувача з терміналу (Serial Monitor).
 *
 * Користувач може ввести цифру 0, 1 або 2 для перемикання між режимами:
 * - 0 — Моніторинг кнопок пульта
 * - 1 — Керування світлодіодом
 * - 2 — Керування сервоприводом
 */
void HandleSerialInput() {
  if (Serial.available()) {
    char choice = Serial.read();
    switch (choice) {
      case '0': menuMode = 0; break;
      case '1': menuMode = 1; break;
      case '2': menuMode = 2; break;
      default:
        Serial.println("❌ Невідомий вибір. Введіть 0, 1 або 2.");
        return;
    }
    PrintMenu();
  }
}

// ----------------------------------------------------------
//                 ОБРОБКА СИГНАЛІВ З ПУЛЬТА
// ----------------------------------------------------------
/**
 * @brief Обробляє команди з ІЧ-пульта в залежності від поточного режиму.
 *
 * Усі прийняті коди виводяться у Serial Monitor у шістнадцятковому форматі.
 * Відповідна функція обробки викликається залежно від активного режиму меню.
 */
void HandleIRInput() {
  if (IrReceiver.decode()) {
    uint16_t command = IrReceiver.decodedIRData.command;;
    
    // Вивід коду кнопки
    Serial.print("Код кнопки: 0x");
    Serial.println(command);

    switch (menuMode) {
      case 0:
        Serial.println("Режим моніторингу: кнопка прийнята.\n");
        break;
      case 1:
        HandleLEDControl(command);
        break;
      case 2:
        HandleServoControl(command);
        break;
    }

    IrReceiver.resume(); // Готуватися до прийому наступного сигналу
  }
}

// ----------------------------------------------------------
//                    ВИВІД МЕНЮ В ТЕРМІНАЛ
// ----------------------------------------------------------
/**
 * @brief Виводить головне меню програми у Serial Monitor.
 */
void PrintMenu() {
  Serial.println("\n=== ГОЛОВНЕ МЕНЮ ===");
  Serial.println("0 - Режим моніторингу кнопок");
  Serial.println("1 - Керування світлодіодом");
  Serial.println("2 - Керування сервоприводом");
  Serial.print("Поточний режим: ");
  switch (menuMode) {
    case 0: Serial.println("Моніторинг кнопок"); break;
    case 1: Serial.println("LED-керування"); break;
    case 2: Serial.println("Серво-керування"); break;
  }
  Serial.println("====================\n");
}

// ----------------------------------------------------------
//              КЕРУВАННЯ СВІТЛОДІОДОМ З ПУЛЬТА
// ----------------------------------------------------------
/**
 * @brief Обробляє натискання кнопок пульта у режимі керування LED.
 *
 * Кнопка '*' — вмикає світлодіод.
 * Кнопка '#' — вимикає світлодіод.
 */
void HandleLEDControl(uint16_t command) {
  switch (command) {
    case 0xFFFF: break; // Повтор — пропускаємо
    case 0xFFE0:   // Код кнопки "*"
      digitalWrite(LED_PIN, HIGH);
      Serial.println("💡 Світлодіод УВІМКНЕНО\n");
      break;
    case 0xFF02:   // Код кнопки "#"
      digitalWrite(LED_PIN, LOW);
      Serial.println("💡 Світлодіод ВИМКНЕНО\n");
      break;
    default:
      Serial.println("Невідома кнопка у режимі LED.");
  }
}

// ----------------------------------------------------------
//               КЕРУВАННЯ СЕРВОПРИВОДОМ
// ----------------------------------------------------------
/**
 * @brief Обробляє натискання кнопок пульта у режимі керування сервоприводом.
 *
 * Кнопка '*' — зменшує кут на 3°.
 * Кнопка '#' — збільшує кут на 3°.
 */
void HandleServoControl(uint16_t command) {
  switch (command) {
    case 0xFFFF: break;
    case 0xFFE0:   // "*"
      servoAngle -= ANGLE_STEP;
      if (servoAngle < 0) servoAngle = 0;
      myServo.write(servoAngle);
      PrintAngleChange(servoAngle);
      break;
    case 0xFF02:   // "#"
      servoAngle += ANGLE_STEP;
      if (servoAngle > 180) servoAngle = 180;
      myServo.write(servoAngle);
      PrintAngleChange(servoAngle);
      break;
    default:
      Serial.println("Невідома кнопка у режимі серво.");
  }
}

// ----------------------------------------------------------
//              ВИВІД КУТА СЕРВО У MONITOR
// ----------------------------------------------------------
/**
 * @brief Виводить поточний кут сервоприводу у графічному вигляді.
 * @param angle Поточний кут сервоприводу.
 */
void PrintAngleChange(int angle) {
  Serial.print("Поточний кут сервоприводу: ");
  Serial.print(angle);
  Serial.print("°  [");

  int filled = map(angle, 0, 180, 0, 20);
  for (int i = 0; i < 20; i++) {
    if (i < filled) Serial.print("#");
    else Serial.print("-");
  }
  Serial.println("]\n");
}