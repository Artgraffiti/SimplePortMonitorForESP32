#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  int fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror("Error opening port");
    exit(EXIT_FAILURE);
  }
  printf("%s Opened Successfully", argv[1]);

  struct termios newPortSettings, oldPortSettings;
  tcgetattr(fd, &oldPortSettings);
  newPortSettings = oldPortSettings;

  cfsetispeed(&newPortSettings, B115200);

  newPortSettings.c_cflag &= ~PARENB;
  newPortSettings.c_cflag &= ~CSTOPB;
  newPortSettings.c_cflag &= ~CSIZE;
  newPortSettings.c_cflag |= CS8;

  newPortSettings.c_cflag &= ~CRTSCTS;
  newPortSettings.c_cflag |= CREAD | CLOCAL;

  newPortSettings.c_iflag &= ~(INLCR | ICRNL | IXON | IXOFF | IXANY);

  newPortSettings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHOCTL | ECHOKE);

  newPortSettings.c_oflag &= ~OPOST;
  newPortSettings.c_oflag &= ~ONLCR;

  newPortSettings.c_cc[VMIN] = 0;
  newPortSettings.c_cc[VTIME] = 2;

  if ((tcsetattr(fd, TCSANOW, &newPortSettings)) != 0) {
    fprintf(stderr, "ERROR! in Setting new termios attributes\n");
    exit(EXIT_FAILURE);
  }

  size_t buff_sz = 1024, line_sz = 512;
  char read_buffer[buff_sz];
  char line[line_sz];
  int bytes_read = 0;
  int line_pos = 0;

  while (1) {
    bytes_read = read(fd, &read_buffer, sizeof(read_buffer));

    if (bytes_read == 0) {
      printf("Timeout reached, no data received\n");
      break;
    } else if (bytes_read < 0) {
      perror("Read Error");
      break;
    }

    // Пользовательское построчное чтение
    for (int i = 0; i < bytes_read; i++) {
      if (read_buffer[i] == '\n' || read_buffer[i] == '\0' ||
          line_pos >= (int)(sizeof(line) - 1)) {
        line[line_pos] = '\0'; // Завершаем строку
        printf("%s\n", line);
        line_pos = 0; // Сбрасываем позицию для следующей строки
      } else {
        line[line_pos++] = read_buffer[i]; // Добавляем символ в строку
      }
    }
  }
  printf("----------------End-of-Read----------------\n");

  if ((tcsetattr(fd, TCSANOW, &oldPortSettings)) != 0)
    fprintf(stderr, "ERROR! in restore old termios attributes\n");
  close(fd);

  return 0;
}