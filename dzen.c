#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct cpu_info {
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

struct mem_info {
  int total;
  int free;
  int buffered;
  int cached;
};

struct cpu_info cpu_current;
struct cpu_info cpu_old;
struct cpu_info cpu_diff;
struct cpu_info cpu_scaled;
int cpu_sum;

struct mem_info mem;
struct mem_info mem_scaled;
int mem_sum;

int length;

int update_cpu(int initial) {
  cpu_old = cpu_current;
  FILE* fp = fopen("/proc/stat", "r");

  if (fp == NULL) {
    perror("open");
    return EXIT_FAILURE;
  }

  char line[80];
  char* result = fgets(line, 80, fp);

  if (result == NULL) {
    fclose(fp);
    perror("fgets");
    return EXIT_FAILURE;
  }

  int scanned = sscanf(line, "cpu %d %d %d %d %d %d %d %d %d %d", &cpu_current.user, &cpu_current.nice, &cpu_current.system, &cpu_current.idle, &cpu_current.iowait, &cpu_current.irq, &cpu_current.softirq, &cpu_current.steal, &cpu_current.guest, &cpu_current.guest_nice);

  if (scanned < 1 || scanned == EOF) {
    fclose(fp);
    perror("sscanf");
    return EXIT_FAILURE;
  }

  fclose(fp);

  if (initial) {
    return EXIT_SUCCESS;
  }

  cpu_diff.user = cpu_current.user - cpu_old.user;
  cpu_diff.nice = cpu_current.nice - cpu_old.nice;
  cpu_diff.system = cpu_current.system - cpu_old.system;
  cpu_diff.idle = cpu_current.idle - cpu_old.idle;
  cpu_diff.iowait = cpu_current.iowait - cpu_old.iowait;
  cpu_diff.irq = cpu_current.irq - cpu_old.irq;
  cpu_diff.softirq = cpu_current.softirq - cpu_old.softirq;
  cpu_diff.steal = cpu_current.steal - cpu_old.steal;
  cpu_diff.guest = cpu_current.guest - cpu_old.guest;
  cpu_diff.guest_nice = cpu_current.guest_nice - cpu_old.guest_nice;

  cpu_sum = cpu_diff.user + cpu_diff.nice + cpu_diff.system + cpu_diff.idle + cpu_diff.iowait + cpu_diff.irq + cpu_diff.softirq + cpu_diff.steal + cpu_diff.guest + cpu_diff.guest_nice;

  cpu_scaled.user = (int) (0.5 + length*cpu_diff.user/cpu_sum);
  cpu_scaled.nice = (int) (0.5 + length*cpu_diff.nice/cpu_sum);
  cpu_scaled.system = (int) (0.5 + length*cpu_diff.system/cpu_sum);
  cpu_scaled.idle = (int) (0.5 + length*cpu_diff.idle/cpu_sum);
  cpu_scaled.iowait = (int) (0.5 + length*cpu_diff.iowait/cpu_sum);
  cpu_scaled.irq = (int) (0.5 + length*cpu_diff.irq/cpu_sum);
  cpu_scaled.softirq = (int) (0.5 + length*cpu_diff.softirq/cpu_sum);
  cpu_scaled.steal = (int) (0.5 + length*cpu_diff.steal/cpu_sum);
  cpu_scaled.guest = (int) (0.5 + length*cpu_diff.guest/cpu_sum);
  cpu_scaled.guest_nice = (int) (0.5 + length*cpu_diff.guest_nice/cpu_sum);
  return EXIT_SUCCESS;
}

int update_memory(int initial) {
  FILE* fp = fopen("/proc/meminfo", "r");

  if (fp == NULL) {
    perror("open");
    return EXIT_FAILURE;
  }

  int i;

  for (i = 0; i < 4; i++) {
    char line[80];
    char* result = fgets(line, 80, fp);
    int scanned;

    if (result == NULL) {
      fclose(fp);
      perror("fgets");
      return EXIT_FAILURE;
    }

    switch (i) {
    case 0:
      scanned = sscanf(line, "MemTotal: %d kB", &mem.total);
      break;
    case 1:
      scanned = sscanf(line, "MemFree: %d kB", &mem.free);
      break;
    case 2:
      scanned = sscanf(line, "Buffers: %d kB", &mem.buffered);
      break;
    case 3:
      scanned = sscanf(line, "Cached: %d kB", &mem.cached);
      break;
    }

    if (scanned < 1 || scanned == EOF) {
      fclose(fp);
      perror("sscanf");
      return EXIT_FAILURE;
    }
  }

  fclose(fp);

  if (initial) {
    return EXIT_SUCCESS;
  }

  mem_scaled.total = length;
  mem_scaled.free = (int) (0.5 + length*mem.free/mem.total);
  mem_scaled.buffered = (int) (0.5 + length*mem.buffered/mem.total);
  mem_scaled.cached = (int) (0.5 + length*mem.cached/mem.total);

  return EXIT_SUCCESS;
}

int update(int initial) {
  int result = update_cpu(initial);

  if (result != EXIT_SUCCESS) {
    return result;
  }

  result = update_memory(initial);
  return result;
}

int main(int argc, char** argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: dzen LENGTH\n");
    return EXIT_FAILURE;
  }

  const char* BLUE = "#0046bf";
  const char* GREEN = "#00bf00";
  const char* ORANGE = "#bf9900";
  const char* RED = "#b62300";
  const char* CYAN = "#00bfbf";
  const char* MAGENTA = "#b400a3";
  const char* GREY = "#969696";
  const char* DARK_GREY = "#444444";

  length = atoi(argv[1]);

  update(1);

  while (1) {
    sleep(1);

    if (update(0) != EXIT_SUCCESS) {
      return EXIT_FAILURE;
    }

    printf("CPU ");
    printf("^fg(%s)^r(%dx10)", BLUE, cpu_scaled.nice);
    printf("^fg(%s)^r(%dx10)", GREEN, cpu_scaled.user);
    printf("^fg(%s)^r(%dx10)", RED, cpu_scaled.system);
    printf("^fg(%s)^r(%dx10)", ORANGE, cpu_scaled.irq);
    printf("^fg(%s)^r(%dx10)", MAGENTA, cpu_scaled.softirq);
    printf("^fg(%s)^r(%dx10)", GREY, cpu_scaled.iowait);
    printf("^fg(%s)^r(%dx10)", CYAN, cpu_scaled.steal);
    printf("^r(%dx10)", cpu_scaled.guest);
    printf("^r(%dx10)", cpu_scaled.guest_nice);
    printf("^fg(%s)^r(%dx10)", DARK_GREY, length-cpu_scaled.nice-cpu_scaled.user-cpu_scaled.system-cpu_scaled.irq-cpu_scaled.softirq-cpu_scaled.iowait-cpu_scaled.steal-cpu_scaled.guest-cpu_scaled.guest_nice);
    printf("^fg()");

    printf("   ");

    printf("RAM ");
    printf("^fg(%s)^r(%dx10)", GREEN, mem_scaled.total-mem_scaled.free-mem_scaled.buffered-mem_scaled.cached);
    printf("^fg(%s)^r(%dx10)", BLUE, mem_scaled.buffered);
    printf("^fg(%s)^r(%dx10)", ORANGE, mem_scaled.cached);
    printf("^fg(%s)^r(%dx10)", DARK_GREY, mem_scaled.free);

    printf("\n");
    fflush(stdout);
  }

  return EXIT_SUCCESS;
}
