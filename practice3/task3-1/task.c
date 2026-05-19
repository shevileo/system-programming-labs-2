#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define report_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main() {
    int parent_to_child[2];  // Канал: родитель -> дочерний
    int child_to_parent[2];  // Канал: дочерний -> родитель
    pid_t pid;
    int number, square;

    // Создаём два канала
    if (pipe(parent_to_child) == -1) report_error("pipe parent_to_child");
    if (pipe(child_to_parent) == -1) report_error("pipe child_to_parent");

    pid = fork();

    if (pid == -1) {
        report_error("fork");
    }
    else if (pid == 0) {
        // Дочерний процесс
        close(parent_to_child[1]);  // Закрываем запись в parent_to_child
        close(child_to_parent[0]);  // Закрываем чтение из child_to_parent

        // Читаем число от родителя
        if (read(parent_to_child[0], &number, sizeof(number)) != sizeof(number)) {
            report_error("child read");
        }
        printf("[CHILD] Получено число: %d\n", number);

        // Вычисляем квадрат
        square = number * number;
        printf("[CHILD] Квадрат числа: %d\n", square);

        // Отправляем квадрат родителю
        if (write(child_to_parent[1], &square, sizeof(square)) != sizeof(square)) {
            report_error("child write");
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(EXIT_SUCCESS);
    }
    else {
        // Родительский процесс
        close(parent_to_child[0]);  // Закрываем чтение из parent_to_child
        close(child_to_parent[1]);  // Закрываем запись в child_to_parent

        // Вводим число с клавиатуры
        printf("[PARENT] Введите целое число: ");
        if (scanf("%d", &number) != 1) {
            fprintf(stderr, "Ошибка ввода\n");
            exit(EXIT_FAILURE);
        }

        // Отправляем число дочернему процессу
        if (write(parent_to_child[1], &number, sizeof(number)) != sizeof(number)) {
            report_error("parent write");
        }
        printf("[PARENT] Отправлено число: %d\n", number);

        // Читаем квадрат от дочернего процесса
        if (read(child_to_parent[0], &square, sizeof(square)) != sizeof(square)) {
            report_error("parent read");
        }
        printf("[PARENT] Получен квадрат: %d\n", square);

        close(parent_to_child[1]);
        close(child_to_parent[0]);
        wait(NULL);
    }

    return 0;
}