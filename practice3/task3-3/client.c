#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

#define report_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main() {
    int sock;
    struct sockaddr_in server_addr;
    int numbers[20];
    int sum;

    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) report_error("socket");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        report_error("connect");
    }
    printf("[CLIENT] Подключён к серверу\n");

    // Заполнение массива числами от 1 до 20 (для примера)
    printf("[CLIENT] Генерация 20 чисел: ");
    for (int i = 0; i < 20; i++) {
        numbers[i] = i + 1;
        printf("%d ", numbers[i]);
    }
    printf("\n");

    // Отправка массива серверу
    if (send(sock, numbers, sizeof(numbers), 0) != sizeof(numbers)) {
        report_error("send");
    }

    // Получение суммы
    if (recv(sock, &sum, sizeof(sum), 0) != sizeof(sum)) {
        report_error("recv");
    }
    printf("[CLIENT] Получена сумма от сервера: %d\n", sum);

    close(sock);
    return 0;
}