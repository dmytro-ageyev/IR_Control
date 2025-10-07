/**
 * @file ServoControl_SerialFunctions.ino
 * @brief Керування сервоприводом через введення кута з монітора порту (Serial Monitor).
 *
 * Програма дозволяє користувачу вводити кут повороту сервоприводу (0–180°)
 * через монітор порту Arduino IDE. Зчитане значення передається сервоприводу,
 * який повертається на вказаний кут.
 *
 * Всі операції введення/виведення через Serial винесено в окремі функції:
 * - ShowInstructions() — показує користувачу інструкцію;
 * - ReadAngleFromSerial() — зчитує введений кут;
 * - PrintAngleFeedback() — виводить результат виконаної дії;
 * - PrintErrorMessage() — повідомляє про помилку введення.
 *
 * Підключення сервоприводу:
 * - Сигнальний провід → D5
 * - Живлення (червоний) → 5V
 * - GND (чорний або коричневий) → GND
 *
 * @author
 *  Дмитро Агеєв
 * @date
 *  07.10.2025
 */

#include <Arduino.h>
#include <Servo.h>

// === Константи ===
/**
 * @brief Пін для підключення сигнального проводу сервоприводу.
 */
const int SERVO_PIN = 5;

/**
 * @brief Межі дозволених значень кута сервоприводу.
 */
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

// === Глобальні змінні ===
Servo myServo;          ///< Об’єкт для керування сервоприводом
int currentAngle = 90;  ///< Поточний кут повороту сервоприводу

// === Прототипи функцій ===
void ShowInstructions();                          // Виводить інструкції у Serial Monitor
int ReadAngleFromSerial();                        // Зчитує кут, введений користувачем
void PrintAngleFeedback(int angle);               // Виводить повідомлення про успішний поворот
void PrintErrorMessage(int angle);                // Повідомляє про помилку
void ClearSerialBuffer();                         // Очищає буфер Serial
void MoveServoToAngle(int angle);                 // Повертає серво на вказаний кут


// === Функція setup() ===
/**
 * @brief Ініціалізує серійний порт, сервопривід і виводить інструкції для користувача.
 */
void setup()
{
  Serial.begin(9600);       // Запуск серійного з’єднання
  myServo.attach(SERVO_PIN); // Прив’язка серво до піна
  myServo.write(currentAngle); // Початковий кут (90°)
  ShowInstructions();        // Виведення інструкцій
}

// === Функція loop() ===
/**
 * @brief Основний цикл: зчитує введення користувача і повертає серво на заданий кут.
 */
void loop()
{
  int angle = ReadAngleFromSerial(); // Зчитування кута з монітора

  if (angle >= 0) // Якщо користувач ввів дані
  {
    if (angle >= MIN_ANGLE && angle <= MAX_ANGLE)
    {
      MoveServoToAngle(angle);
      PrintAngleFeedback(angle);
    }
    else
    {
      PrintErrorMessage(angle);
    }
  }
}

/**
 * @brief Виводить інструкцію для користувача у Serial Monitor.
 */
void ShowInstructions()
{
  Serial.println("=== Керування сервоприводом через Serial Monitor ===");
  Serial.println("Введіть кут у межах від 0 до 180 градусів і натисніть Enter.");
  Serial.println("Приклад: 45");
  Serial.println("-------------------------------------------\n");
}

/**
 * @brief Зчитує введений користувачем кут із монітора порту.
 *
 * Функція чекає, поки у буфері Serial з’являться дані,
 * потім зчитує ціле число (int) і очищає буфер.
 *
 * @return Введений користувачем кут (0–180), або -1, якщо даних немає.
 */
int ReadAngleFromSerial()
{
  if (Serial.available() > 0)
  {
    int value = Serial.parseInt(); // Зчитування числа
    ClearSerialBuffer();           // Очищення буфера після читання
    return value;
  }
  return -1; // Немає даних
}

/**
 * @brief Виводить повідомлення про успішний поворот серво.
 *
 * @param angle Новий кут сервоприводу.
 */
void PrintAngleFeedback(int angle)
{
  Serial.print("✅ Серво повернуто на кут: ");
  Serial.print(angle);
  Serial.println("°\n");
}

/**
 * @brief Виводить повідомлення про помилку при некоректному введенні кута.
 *
 * @param angle Значення, яке ввів користувач.
 */
void PrintErrorMessage(int angle)
{
  Serial.print("⚠️  Помилка: значення ");
  Serial.print(angle);
  Serial.print(" виходить за межі ");
  Serial.print(MIN_ANGLE);
  Serial.print("–");
  Serial.print(MAX_ANGLE);
  Serial.println("°.\n");
}

/**
 * @brief Очищає буфер Serial після зчитування введення.
 */
void ClearSerialBuffer()
{
  while (Serial.available() > 0)
  {
    Serial.read();
  }
}

/**
 * @brief Повертає сервопривід на вказаний кут і зберігає його.
 *
 * @param angle Кут повороту (0–180 градусів).
 */
void MoveServoToAngle(int angle)
{
  currentAngle = angle; // Збереження нового кута
  myServo.write(currentAngle);
}
