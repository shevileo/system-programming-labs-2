#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 4096

#define report_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main() {
    int fd;
    char filepath[256];
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    FILE *output;

    // Открываем FIFO для чтения
    fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) report_error("open fifo");

    // Читаем путь к файлу
    bytes_read = read(fd, filepath, sizeof(filepath) - 1);
    if (bytes_read <= 0) report_error("read filepath");
    filepath[bytes_read] = '\0';

    // Проверяем, не ошибка ли это
    if (strcmp(filepath, "ERROR: file not found") == 0) {
        fprintf(stderr, "[READER] Ошибка: файл не найден у отправителя\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("[READER] Получен путь: %s\n", filepath);

    // Открываем copy.txt для записи
    output = fopen("copy.txt", "wb");
    if (!output) {
        report_error("fopen copy.txt");
    }

    // Читаем содержимое файла из FIFO и сохраняем в copy.txt
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        if (fwrite(buffer, 1, bytes_read, output) != (size_t)bytes_read) {
            report_error("fwrite");
        }
    }

    fclose(output);
    close(fd);
    printf("[READER] Файл сохранён как copy.txt\n");

    // Удаляем FIFO
    unlink(FIFO_NAME);

    return 0;
}