// Решите загадку: Сколько чисел от 1 до 1000 содержат как минимум одну цифру 3?
// Напишите ответ здесь:
#include <iostream>

using namespace std;


int main() {
    int kol = 0;
    for (int i = 1; i <= 1000; i++) {
        int number = i;
        while (number > 0) {
            if (number % 10 == 3) {
                kol++;
                break;
            }
            number /= 10;
        }
    }
    cout << kol << endl;
    return 0;
}
// Закомитьте изменения и отправьте их в свой репозиторий.
