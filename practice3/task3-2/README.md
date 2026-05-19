\# Задание 3-2 (Вариант 5): Передача файла через FIFO



\## Описание

Программа создаёт именованный канал (FIFO). Writer передаёт через FIFO путь к файлу, затем содержимое этого файла. Reader сохраняет полученное содержимое в `copy.txt`.



\## Компиляция

```bash

gcc -Wall -Wextra -o writer writer.c

gcc -Wall -Wextra -o reader reader.c

