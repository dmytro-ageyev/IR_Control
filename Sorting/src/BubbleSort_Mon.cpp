#include "BubbleSort_Mon.h"

void BubbleSort_Mon(int arr[], int size) {
  Serial.println("=== Початок сортування методом 'Бульбашки' ===\r\n");
  
  // Зовнішній цикл — кількість проходів
  for (int i = 0; i < size - 1; i++) {
    Serial.print("Прохід №");
    Serial.println(i + 1);

    bool swapped = false; // Прапорець, щоб перевіряти, чи були зміни

    // Внутрішній цикл — порівняння сусідніх елементів
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        // Обмін елементів місцями
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
        swapped = true;

        // Вивід поточного стану масиву після обміну
        Serial.print("  Обмін елементів ");
        Serial.print(j + 1);
        Serial.print(" і ");
        Serial.print(j + 2);
        Serial.println(": ");
        for (int k = 0; k < size; k++) {
          Serial.print(arr[k]);
          if (k < size - 1) Serial.print("\t");
        }
        Serial.println("\r\n");
      }
    }

    // Якщо жодного обміну не було — масив уже відсортований
    if (!swapped) {
      Serial.println("  Немає обмінів — масив уже відсортований.\r\n");
      break;
    }
    
    Serial.print("Бульбашка всплила: ");
    Serial.println(arr[size - i -1]); 
    Serial.println(); // Порожній рядок між проходами
  }

  Serial.println("=== Сортування завершено ===\r\n");
}
