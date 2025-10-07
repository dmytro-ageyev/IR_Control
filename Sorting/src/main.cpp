#include <Arduino.h> // Бібліотека Arduino для базових функцій
#include "BubbleSort_Mon.h"

// Межі випадкових чисел (унікальні назви, щоб уникнути конфлікту)
const int RND_MIN = 0; 
const int RND_MAX = 100;
const int MY_ARRAY_SIZE = 10;

int MyArr[MY_ARRAY_SIZE]; // Масив для збереження випадкових чисел

// Прототипи функцій
void FillArray(int arr[], int size);              
void WaitAnyKey(String msg);                      
void BubbleSort(int arr[], int size);             
void PrintArray(int arr[], int size, String msg); 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  

  WaitAnyKey("Натисніть будь-яку клавішу, щоб заповнити масив випадковими числами...");
  FillArray(MyArr, MY_ARRAY_SIZE); 

  WaitAnyKey("Натисніть будь-яку клавішу, щоб переглянути вміст масиву...");
  PrintArray(MyArr, MY_ARRAY_SIZE, "Несортований масив:");

  WaitAnyKey("Натисніть будь-яку клавішу, щоб відсортувати масив методом 'Бульбашки'...");

 /* Поставити "зірочку" -> /

 BubbleSort(MyArr, MY_ARRAY_SIZE); 
 /*/
 BubbleSort_Mon(MyArr, MY_ARRAY_SIZE); 
/**/


  PrintArray(MyArr, MY_ARRAY_SIZE, "Масив після сортування (за зростанням):");
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
void FillArray(int arr[], int size) {
  randomSeed(analogRead(A0));
  for (int i = 0; i < size; i++) {
    arr[i] = random(RND_MIN, RND_MAX + 1);
  }
  Serial.println("Масив заповнено випадковими числами.\r\n");
}

void WaitAnyKey(String msg)
{
  Serial.println(msg);
  while (!Serial.available()) delay(10);
  Serial.read();
}

void PrintArray(int arr[], int size, String msg) {
  Serial.println(msg);
  for (int i = 0; i < size; i++) {
    Serial.print(arr[i]);
    if (i < size - 1) Serial.print("\t");
  }
  Serial.println();
}


// Функція BubbleSort виконує сортування масиву за зростанням 
// методом «бульбашки» (Bubble Sort).
// Принцип роботи: порівнюються сусідні елементи масиву, 
// і якщо поточний більший за наступний — вони міняються місцями.
// Таким чином, після кожного проходу найбільший елемент «спливає» в кінець масиву, 
// тому назва «бульбашка».


void BubbleSort(int arr[], int size) {

  // Виведення інформаційного повідомлення в монітор порту,
  // щоб користувач знав, що розпочався процес сортування
  Serial.println("Виконується сортування масиву методом 'Бульбашки'...\r\n");

  /* Поставити "зірочку" -> /

  // Зовнішній цикл: визначає кількість проходів по масиву.
  // Після кожного проходу один елемент (найбільший із тих, що залишилися)
  // переміщається у свій «правильний» кінець масиву.
  for (int i... ;... ; i++) {

    // Внутрішній цикл: порівнює сусідні елементи.
    // З кожною ітерацією зовнішнього циклу межа порівнянь зменшується на 1,
    // тому перевіряємо лише до (size - i - 1).
    for (int j... ; ...; j++) {

      // Якщо поточний елемент більший за наступний —
      // виконується обмін (щоб менший елемент "піднявся" вище)
      if (...) {

        // Тимчасова змінна для збереження одного з елементів під час обміну
        int temp = ...;

        // Міняємо місцями елементи arr[j] і arr[j + 1]
        arr[j] = ...;
        arr[j + 1] = ...;
      }
    }
  }
/* */
  // Після завершення всіх проходів — масив відсортовано за зростанням.
  // Виводимо повідомлення для користувача в монітор порту.
  Serial.println("Масив відсортовано.\r\n");
}
