#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

volatile sig_atomic_t bit_received = 0;
volatile sig_atomic_t bit_to_send = 0;
volatile sig_atomic_t confirmation_received = 0;

void handle_sigusr1(int sig) {
    bit_received = 0;
    confirmation_received = 1;
}

void handle_sigusr2(int sig) {
    bit_received = 1;
    confirmation_received = 1;
}

void child(pid_t parent_pid) {
    signal(SIGUSR1, handle_sigusr1);
    signal(SIGUSR2, handle_sigusr2);

    char buffer = 0;
    int bit_count = 0;

    while (1) {
        while (!confirmation_received) {
            pause(); // Ожидаем сигнал
        }

        buffer = (buffer << 1) | bit_received;
        bit_count++;
        confirmation_received = 0;

        if (bit_count == 8) {
            write(STDOUT_FILENO, &buffer, 1);
            buffer = 0;
            bit_count = 0;
        }

        kill(parent_pid, SIGUSR1); // Подтверждаем получение бита
    }
}

void send_bit(pid_t child_pid, int bit) {
    kill(child_pid, bit ? SIGUSR2 : SIGUSR1);
    while (!confirmation_received) {
        pause(); // Ожидаем подтверждения
    }
    confirmation_received = 0;
}

void parent(pid_t child_pid, FILE *file) {
    char buffer;

    signal(SIGUSR1, handle_sigusr1);
    signal(SIGUSR2, handle_sigusr2);

    while ((buffer = fgetc(file)) != EOF) {
        for (int i = 0; i < 8; ++i) {
            int bit = (buffer >> (7 - i)) & 1;
            send_bit(child_pid, bit);
        }
    }
}

int main(int argc, char *argv[]) {
    pid_t pid;
    FILE *file = argc > 1 ? fopen(argv[1], "r") : stdin;

    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс
        child(getppid());
    } else {
        // Родительский процесс
        parent(pid, file);
        if (file != stdin) {
            fclose(file);
        }
    }

    return 0;
}
