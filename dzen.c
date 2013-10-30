#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
  int swap_total;
  int swap_free;
};

struct cpu_info* cpu_current;
struct cpu_info* cpu_old;
struct cpu_info* cpu_diff;
struct cpu_info* cpu_scaled;
int cpus;
int* cpu_sum;

struct mem_info mem;
struct mem_info mem_scaled;
int mem_sum;

int length;

int update_cpu(int initial) {
  int i;

  for (i = 0; i < cpus; i++) {
    cpu_old[i] = cpu_current[i];
  }

  FILE* fp = fopen("/proc/stat", "r");

  if (fp == NULL) {
    perror("open");
    return EXIT_FAILURE;
  }

  for (i = 0; i < cpus; i++) {
    char line[80];
    char* result = fgets(line, 80, fp);

    if (result == NULL) {
      fclose(fp);
      perror("fgets");
      return EXIT_FAILURE;
    }

    char buffer[80];
    int scanned = sscanf(line, "%s %d %d %d %d %d %d %d %d %d %d", buffer,
                         &(cpu_current[i].user), &(cpu_current[i].nice),
                         &(cpu_current[i].system), &(cpu_current[i].idle),
                         &(cpu_current[i].iowait), &(cpu_current[i].irq),
                         &(cpu_current[i].softirq), &(cpu_current[i].steal),
                         &(cpu_current[i].guest), &(cpu_current[i].guest_nice));

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

  for (i = 0; i < cpus; i++) {
    cpu_diff[i].user = cpu_current[i].user - cpu_old[i].user;
    cpu_diff[i].nice = cpu_current[i].nice - cpu_old[i].nice;
    cpu_diff[i].system = cpu_current[i].system - cpu_old[i].system;
    cpu_diff[i].idle = cpu_current[i].idle - cpu_old[i].idle;
    cpu_diff[i].iowait = cpu_current[i].iowait - cpu_old[i].iowait;
    cpu_diff[i].irq = cpu_current[i].irq - cpu_old[i].irq;
    cpu_diff[i].softirq = cpu_current[i].softirq - cpu_old[i].softirq;
    cpu_diff[i].steal = cpu_current[i].steal - cpu_old[i].steal;
    cpu_diff[i].guest = cpu_current[i].guest - cpu_old[i].guest;
    cpu_diff[i].guest_nice = cpu_current[i].guest_nice - cpu_old[i].guest_nice;

    cpu_sum[i] = cpu_diff[i].user + cpu_diff[i].nice + cpu_diff[i].system + cpu_diff[i].idle + cpu_diff[i].iowait + cpu_diff[i].irq + cpu_diff[i].softirq + cpu_diff[i].steal + cpu_diff[i].guest + cpu_diff[i].guest_nice;

    int len;

    if (i == 0) {
      len = length;
    } else {
      len = length/2;
    }

    cpu_scaled[i].user = (int) (0.5 + len*cpu_diff[i].user/cpu_sum[i]);
    cpu_scaled[i].nice = (int) (0.5 + len*cpu_diff[i].nice/cpu_sum[i]);
    cpu_scaled[i].system = (int) (0.5 + len*cpu_diff[i].system/cpu_sum[i]);
    cpu_scaled[i].idle = (int) (0.5 + len*cpu_diff[i].idle/cpu_sum[i]);
    cpu_scaled[i].iowait = (int) (0.5 + len*cpu_diff[i].iowait/cpu_sum[i]);
    cpu_scaled[i].irq = (int) (0.5 + len*cpu_diff[i].irq/cpu_sum[i]);
    cpu_scaled[i].softirq = (int) (0.5 + len*cpu_diff[i].softirq/cpu_sum[i]);
    cpu_scaled[i].steal = (int) (0.5 + len*cpu_diff[i].steal/cpu_sum[i]);
    cpu_scaled[i].guest = (int) (0.5 + len*cpu_diff[i].guest/cpu_sum[i]);
    cpu_scaled[i].guest_nice = (int) (0.5 + len*cpu_diff[i].guest_nice/cpu_sum[i]);
  }

  return EXIT_SUCCESS;
}

int update_memory(int initial) {
  FILE* fp = fopen("/proc/meminfo", "r");

  if (fp == NULL) {
    perror("open");
    return EXIT_FAILURE;
  }

  int i;

  for (i = 0; i < 15; i++) {
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
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
      continue;
    case 13:
      scanned = sscanf(line, "SwapTotal: %d kB", &mem.swap_total);
      break;
    case 14:
      scanned = sscanf(line, "SwapFree: %d kB", &mem.swap_free);
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
  mem_scaled.swap_total = length;
  mem_scaled.swap_free = (int) (0.5 + length*mem.swap_free/mem.swap_total);

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

int init_cpu() {
  FILE* fp = fopen("/proc/stat", "r");

  if (fp == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  cpus = 0;

  while (1) {
    char line[80];
    char* got = fgets(line, 80, fp);

    if (got == NULL) {
      fclose(fp);
      perror("fgets");
      return EXIT_FAILURE;
    }

    if (strncmp(line, "cpu", 3) != 0) {
      break;
    } else {
      cpus++;
    }
  }

  cpu_current = malloc(cpus*sizeof(struct cpu_info));
  cpu_old = malloc(cpus*sizeof(struct cpu_info));
  cpu_diff = malloc(cpus*sizeof(struct cpu_info));
  cpu_scaled = malloc(cpus*sizeof(struct cpu_info));
  cpu_sum = malloc(cpus*sizeof(int));
  return EXIT_SUCCESS;
}

int main(int argc, char** argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: dzen LENGTH\n");
    return EXIT_FAILURE;
  }

  int initialized = init_cpu();

  if (initialized != EXIT_SUCCESS) {
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
      free(cpu_current);
      free(cpu_old);
      free(cpu_diff);
      free(cpu_scaled);
      free(cpu_sum);
      return EXIT_FAILURE;
    }

    int i;

    for (i = 1; i < cpus; i++) {
      printf("CPU ");
      int len;

      if (i == 0) {
        len = length;
      } else {
        len = length/2;
        printf("%d ", i);
      }

      printf("^fg(%s)^r(%dx10)", BLUE, cpu_scaled[i].nice);
      printf("^fg(%s)^r(%dx10)", GREEN, cpu_scaled[i].user);
      printf("^fg(%s)^r(%dx10)", RED, cpu_scaled[i].system);
      printf("^fg(%s)^r(%dx10)", ORANGE, cpu_scaled[i].irq);
      printf("^fg(%s)^r(%dx10)", MAGENTA, cpu_scaled[i].softirq);
      printf("^fg(%s)^r(%dx10)", GREY, cpu_scaled[i].iowait);
      printf("^fg(%s)^r(%dx10)", CYAN, cpu_scaled[i].steal);
      printf("^r(%dx10)", cpu_scaled[i].guest);
      printf("^r(%dx10)", cpu_scaled[i].guest_nice);
      printf("^fg(%s)^r(%dx10)", DARK_GREY, len-cpu_scaled[i].nice-cpu_scaled[i].user-cpu_scaled[i].system-cpu_scaled[i].irq-cpu_scaled[i].softirq-cpu_scaled[i].iowait-cpu_scaled[i].steal-cpu_scaled[i].guest-cpu_scaled[i].guest_nice);
      printf("^fg()");

      printf("   ");
    }

    printf("RAM ");
    printf("^fg(%s)^r(%dx10)", GREEN, mem_scaled.total-mem_scaled.free-mem_scaled.buffered-mem_scaled.cached);
    printf("^fg(%s)^r(%dx10)", BLUE, mem_scaled.buffered);
    printf("^fg(%s)^r(%dx10)", ORANGE, mem_scaled.cached);
    printf("^fg(%s)^r(%dx10)", DARK_GREY, mem_scaled.free);
    printf("^fg()");

    printf("   ");

    printf("Swap ");
    printf("^fg(%s)^r(%dx10)", RED, mem_scaled.swap_total-mem_scaled.swap_free);
    printf("^fg(%s)^r(%dx10)", DARK_GREY, mem_scaled.swap_free);
    printf("^fg()");

    printf("\n");
    fflush(stdout);
  }

  free(cpu_current);
  free(cpu_old);
  free(cpu_diff);
  free(cpu_scaled);
  free(cpu_sum);
  return EXIT_SUCCESS;
}
