#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct info {
  int user;
  int nice;
  int system;
  int idle;
  int iowait;
  int irq;
  int softirq;
  int steal;
  int guest;
  int guest_nice;
};

struct info current;
struct info old;
struct info diff;
struct info scaled;
int sum;
int length;

int update(int initial) {
  old = current;
  FILE* fp = fopen("/proc/stat", "r");

  if (fp == NULL) {
    perror("open");
    return EXIT_FAILURE;
  }

#ifdef DEBUG
  printf("File opened.\n");
#endif

  char line[80];
  char* result = fgets(line, 80, fp);

  if (result != NULL) {
    sscanf(line, "cpu %d %d %d %d %d %d %d %d %d %d", &current.user, &current.nice, &current.system, &current.idle, &current.iowait, &current.irq, &current.softirq, &current.steal, &current.guest, &current.guest_nice);
  } else {
    perror("fgets");
  }

  fclose(fp);
#ifdef DEBUG
  printf("File closed.\n");
#endif

  if (result == NULL) {
    return EXIT_FAILURE;
  }

  if (initial) {
    return EXIT_SUCCESS;
  }

  diff.user = current.user - old.user;
  diff.nice = current.nice - old.nice;
  diff.system = current.system - old.system;
  diff.idle = current.idle - old.idle;
  diff.iowait = current.iowait - old.iowait;
  diff.irq = current.irq - old.irq;
  diff.softirq = current.softirq - old.softirq;
  diff.steal = current.steal - old.steal;
  diff.guest = current.guest - old.guest;
  diff.guest_nice = current.guest_nice - old.guest_nice;

  sum = diff.user + diff.nice + diff.system + diff.idle + diff.iowait + diff.irq + diff.softirq + diff.steal + diff.guest + diff.guest_nice;

  scaled.user = (int) (0.5 + length*diff.user/sum);
  scaled.nice = (int) (0.5 + length*diff.nice/sum);
  scaled.system = (int) (0.5 + length*diff.system/sum);
  scaled.idle = (int) (0.5 + length*diff.idle/sum);
  scaled.iowait = (int) (0.5 + length*diff.iowait/sum);
  scaled.irq = (int) (0.5 + length*diff.irq/sum);
  scaled.softirq = (int) (0.5 + length*diff.softirq/sum);
  scaled.steal = (int) (0.5 + length*diff.steal/sum);
  scaled.guest = (int) (0.5 + length*diff.guest/sum);
  scaled.guest_nice = (int) (0.5 + length*diff.guest_nice/sum);
  return EXIT_SUCCESS;
}

int main(int argc, char** argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: dzen LENGTH\n");
    return EXIT_FAILURE;
  }

  length = atoi(argv[1]);

  update(1);

  while (1) {
    sleep(1);

    if (update(0) != EXIT_SUCCESS) {
      return EXIT_FAILURE;
    }

    printf("CPU ^fg(blue)^r(%dx10)^fg(green)^r(%dx10)^fg(red)^r(%dx10)^fg(orange)^r(%dx10)^fg(magenta)^r(%dx10)^fg(grey)^r(%dx10)^fg(cyan)^r(%dx10)^r(%dx10)^r(%dx10)^fg(darkgrey)^r(%dx10)\n", scaled.nice, scaled.user, scaled.system, scaled.irq, scaled.softirq, scaled.iowait, scaled.steal, scaled.guest, scaled.guest_nice, 100-scaled.nice-scaled.user-scaled.system-scaled.irq-scaled.softirq-scaled.iowait-scaled.steal-scaled.guest-scaled.guest_nice);
    fflush(stdout);
  }

  return EXIT_SUCCESS;
}
