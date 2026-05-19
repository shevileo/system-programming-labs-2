#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#define LOG_FILE "/tmp/daemon_size.log"
#define DATA_FILE "data.txt"
#define CHECK_INTERVAL 5  // секунды

static volatile int keep_running = 1;
static const char *pid_file = "/tmp/file_size_daemon.pid";

void signal_handler(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        keep_running = 0;
    }
}

void write_pid_file() {
    FILE *f = fopen(pid_file, "w");
    if (f) {
        fprintf(f, "%d\n", getpid());
        fclose(f);
    }
}

void remove_pid_file() {
    unlink(pid_file);
}

void write_log(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        time_t now = time(NULL);
        char *time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = '\0';  // убираем \n
        fprintf(log, "[%s] %s\n", time_str, message);
        fclose(log);
    }
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);
}

int main() {
    struct stat st;
    off_t last_size = -1;
    char log_msg[256];

    daemonize();

    write_pid_file();

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    write_log("Демон запущен, отслеживание файла " DATA_FILE);

    while (keep_running) {
        if (stat(DATA_FILE, &st) == 0) {
            if (last_size == -1) {
                last_size = st.st_size;
                snprintf(log_msg, sizeof(log_msg), 
                         "Начальный размер %s: %ld байт", DATA_FILE, last_size);
                write_log(log_msg);
            } else if (st.st_size != last_size) {
                snprintf(log_msg, sizeof(log_msg),
                         "Размер изменился: %ld -> %ld байт (изменение: %+ld)",
                         last_size, st.st_size, st.st_size - last_size);
                write_log(log_msg);
                last_size = st.st_size;
            }
        } else {
            write_log("Файл " DATA_FILE " не найден!");
        }

        sleep(CHECK_INTERVAL);
    }

    write_log("Демон завершён");
    remove_pid_file();
    return 0;
}