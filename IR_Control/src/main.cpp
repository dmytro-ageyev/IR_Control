#include <Arduino.h>
#include <IRremote.h>
#include <Servo.h>

// === Константи та глобальні змінні ===
#define LED_PIN 13
#define SERVO_PIN 9
#define IR_PIN 2

IRrecv irrecv(IR_PIN);
decode_results results;
Servo myServo;

int servoAngle = 90;       // Початковий кут сервоприводу
bool ledState = false;     // Поточний стан LED
int currentMode = 0;       // 0 - Монітор, 1 - LED, 2 - Servo, 3 - Сервіс

// --- Коди кнопок пульта (приклад для твоїх кнопок) ---
#define BTN_STAR 0xFFA25D  // *
#define BTN_HASH 0xFFB04F  // #

// --- Прототипи функцій ---
void PrintMenu();
void HandleSerialInput();
void HandleIRCommand();
void PrintProgressBar(int percent);
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

  // --- Лого та банер ---
  Serial.println();
  Serial.println("=================================================");
  Serial.println("         Robocode/OS v1.0 - Pro — Система Керування");
  Serial.println("=================================================\n");
  delay(700);

  if (!InitSystem()) {
    Serial.println("\n[!!!] Критична помилка ініціалізації. Перезапустіть пристрій.");
    while (true); // Зупинка програми
  }

  Serial.println("\n[ OK ] Ініціалізацію завершено успішно!");
  Serial.println("-------------------------------------------------");
  Serial.println(" Платформа : Arduino Uno");
  Serial.println(" Ядро      : AVR @16MHz");
  Serial.println(" Стан      : Система готова до роботи");
  Serial.println("-------------------------------------------------\n");

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
  int progress = 0;

  Serial.println("[BOOT] Ініціалізація системних модулів Robocode/OS...\n");

  Serial.print(" -> Ініціалізація Serial... ");
  delay(300);
  if (Serial) Serial.println("[OK]");
  else { Serial.println("[FAILED]"); initOK = false; }
  progress += 20;
  PrintProgressBar(progress);

  Serial.print(" -> Запуск приймача ІЧ-сигналів... ");
  irrecv.enableIRIn();
  delay(300);
  if (irrecv.isIdle()) Serial.println("[OK]");
  else Serial.println("[WARNING]");
  progress += 20;
  PrintProgressBar(progress);

  Serial.print(" -> Ініціалізація сервоприводу... ");
  myServo.attach(SERVO_PIN);
  delay(300);
  if (myServo.attached()) {
    myServo.write(servoAngle);
    Serial.println("[OK]");
  } else { Serial.println("[FAILED]"); initOK = false; }
  progress += 30;
  PrintProgressBar(progress);

  Serial.print(" -> Налаштування піну LED... ");
  pinMode(LED_PIN, OUTPUT);
  delay(200);
  Serial.println("[OK]");
  progress += 20;
  PrintProgressBar(progress);

  Serial.print(" -> Завершення системного запуску... [OK]");
  delay(200);
  PrintProgressBar(100);

  return initOK;
}

// === Прогрес-бар ===
void PrintProgressBar(int percent) {
  Serial.print("[");
  int totalBars = 25;
  int filledBars = map(percent, 0, 100, 0, totalBars);

  for (int i = 0; i < totalBars; i++) {
    if (i < filledBars) Serial.print("#");
    else Serial.print("-");
  }
  Serial.print("] ");
  Serial.print(percent);
  Serial.println("%");
}

// === Отримати системний час у форматі [HH:MM:SS] ===
String GetSystemTime() {
  unsigned long seconds = millis() / 1000;
  unsigned int hours = seconds / 3600;
  unsigned int minutes = (seconds % 3600) / 60;
  unsigned int secs = seconds % 60;

  char buffer[10];
  sprintf(buffer, "[%02u:%02u:%02u]", hours, minutes, secs);
  return String(buffer);
}

// === Вивід меню ===
void PrintMenu() {
  Serial.println("\n=============== ГОЛОВНЕ МЕНЮ ===============");
  Serial.println(" " + GetSystemTime() + " [INFO] Виберіть режим:");
  Serial.println(" [0] Режим монітору кнопок пульта");
  Serial.println(" [1] Керування світлодіодом");
  Serial.println(" [2] Керування сервоприводом");
  Serial.println(" [3] Сервісний режим (діагностика системи)");
  Serial.println("============================================");
  Serial.println(" Введіть номер режиму у терміналі:");
  Serial.println("============================================\n");
}

// === Вивід дії у форматі системного логу ===
void PrintAction(String msg) {
  Serial.print(GetSystemTime());
  Serial.print(" [ACTION] ");
  Serial.println(msg);
}

// === Обробка введення з терміналу ===
void HandleSerialInput() {
  if (Serial.available()) {
    char input = Serial.read();
    switch (input) {
      case '0':
        currentMode = 0;
        PrintAction("Режим: Моніторинг ІЧ-кнопок.");
        break;
      case '1':
        currentMode = 1;
        PrintAction("Режим: Керування світлодіодом.");
        break;
      case '2':
        currentMode = 2;
        PrintAction("Режим: Керування сервоприводом.");
        break;
      case '3':
        currentMode = 3;
        PrintAction("Режим: Сервісний режим (діагностика).");
        PrintServiceStatus();
        break;
      default:
        Serial.println(GetSystemTime() + " [ERROR] Невідомий режим. Спробуйте ще раз.");
        break;
    }
  }
}

// === Обробка сигналів пульта ===
void HandleIRCommand() {
  if (irrecv.decode(&results)) {
    unsigned long code = results.value;
    Serial.print(GetSystemTime());
    Serial.print(" [IR] Отримано код: 0x");
    Serial.println(code, HEX);

    switch (currentMode) {
      case 0: break; // Монітор кодів
      case 1: ControlLED(code); break;
      case 2: ControlServo(code); break;
      case 3: PrintServiceStatus(); break;
    }

    irrecv.resume(); // готовність до наступного сигналу
  }
}

// === Керування LED ===
void ControlLED(unsigned long code) {
  if (code == BTN_STAR) {
    digitalWrite(LED_PIN, HIGH);
    ledState = true;
    PrintAction("Світлодіод УВІМКНЕНО (*).");
  } else if (code == BTN_HASH) {
    digitalWrite(LED_PIN, LOW);
    ledState = false;
    PrintAction("Світлодіод ВИМКНЕНО (#).");
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
}

// === Сервісний режим ===
void PrintServiceStatus() {
  Serial.println("\n=============== SERVICEMODE ===============");
  Serial.println(GetSystemTime() + " [INFO] Статус системи:");

  String ledStatus = ledState ? "УВІМКНЕНО" : "ВИМКНЕНО";
  Serial.println(" LED          : " + ledStatus);
  Serial.println(" Servo        : Підключено, кут = " + String(servoAngle) + "°");
  Serial.println(" IR Receiver  : " + String(irrecv.isIdle() ? "Готовий" : "Помилка"));

  Serial.println(" Servo Lib    : 1.1.8");
  Serial.println(" IRremote Lib : 2.9.0");
  Serial.println(" Arduino Core : " + String((F_CPU / 1000000)) + " MHz AVR");

  unsigned long seconds = millis() / 1000;
  unsigned int h = seconds / 3600;
  unsigned int m = (seconds % 3600) / 60;
  unsigned int s = seconds % 60;
  char buf[10];
  sprintf(buf, "%02u:%02u:%02u", h, m, s);
  Serial.println(" Uptime       : " + String(buf));

  Serial.println("==========================================\n");
}
