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
  long total;
  long free;
  long buffered;
  long cached;
  long swap_total;
  long swap_free;
};

struct cpu_info* cpu_current;
struct cpu_info* cpu_old;
struct cpu_info* cpu_diff;
struct cpu_info* cpu_scaled;
int cpus;
int* cpu_sum;

struct mem_info mem;
struct mem_info mem_scaled;
long mem_sum;

int battery;
int battery_scaled;

int length;

int update_cpu(int initial) {
  int i;

  for (i = 0; i < cpus; i++) {
    cpu_old[i] = cpu_current[i];
  }

  FILE* fp = fopen("/proc/stat", "r");
#ifdef DEBUG
  printf("/proc/stat opened.\n");
#endif

  if (fp == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  for (i = 0; i < cpus; i++) {
    char line[80];
    char* result = fgets(line, 80, fp);

    if (result == NULL) {
      fclose(fp);
#ifdef DEBUG
      printf("/proc/stat closed.\n");
#endif
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
#ifdef DEBUG
      printf("/proc/stat closed.\n");
#endif
      perror("sscanf");
      return EXIT_FAILURE;
    }
  }

  fclose(fp);
#ifdef DEBUG
  printf("/proc/stat closed.\n");
#endif

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

int update_memory() {
  FILE* fp = fopen("/proc/meminfo", "r");
#ifdef DEBUG
  printf("/proc/meminfo opened.\n");
#endif

  if (fp == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  unsigned int done = 0;

  while (done != 64-1) {
    char line[80];
    char* result = fgets(line, 80, fp);
    int scanned;

    if (result == NULL) {
      fclose(fp);
#ifdef DEBUG
      printf("/proc/meminfo closed.\n");
#endif
      perror("fgets");
      return EXIT_FAILURE;
    }

    if (strncmp(line, "MemTotal:", 9) == 0) {
      scanned = sscanf(line, "MemTotal: %d kB", &mem.total);
      done |= 1;
    } else if (strncmp(line, "MemFree:", 8) == 0) {
      scanned = sscanf(line, "MemFree: %d kB", &mem.free);
      done |= 2;
    } else if (strncmp(line, "Buffers:", 8) == 0) {
      scanned = sscanf(line, "Buffers: %d kB", &mem.buffered);
      done |= 4;
    } else if (strncmp(line, "Cached:", 7) == 0) {
      scanned = sscanf(line, "Cached: %d kB", &mem.cached);
      done |= 8;
    } else if (strncmp(line, "SwapTotal:", 10) == 0) {
      scanned = sscanf(line, "SwapTotal: %d kB", &mem.swap_total);
      done |= 16;
    } else if (strncmp(line, "SwapFree:", 9) == 0) {
      scanned = sscanf(line, "SwapFree: %d kB", &mem.swap_free);
      done |= 32;
    } else {
      continue;
    }

    if (scanned == EOF) {
      fclose(fp);
#ifdef DEBUG
      printf("/proc/meminfo closed.\n");
#endif
      perror("sscanf");
      return EXIT_FAILURE;
    }
  }

  fclose(fp);
#ifdef DEBUG
  printf("/proc/meminfo closed.\n");
#endif

  mem_scaled.total = length;
  mem_scaled.free = (int) (0.5 + length*mem.free/mem.total);
  mem_scaled.buffered = (int) (0.5 + length*mem.buffered/mem.total);
  mem_scaled.cached = (int) (0.5 + length*mem.cached/mem.total);
  mem_scaled.swap_total = length;

  if (mem.swap_total > 0) {
    mem_scaled.swap_free = (int) (0.5 + length*mem.swap_free/mem.swap_total);
  } else {
    mem_scaled.swap_free = length;
  }

  return EXIT_SUCCESS;
}

int update_battery() {
  battery = -1;

  if (access("/sys/class/power_supply/BAT0/capacity", R_OK) == -1) {
    return EXIT_SUCCESS;
  }

  FILE* fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
#ifdef DEBUG
  printf("/sys/class/power_supply/BAT0/capacity opened.\n");
#endif

  if (fp == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  char line[80];
  char* got = fgets(line, 80, fp);

  if (got == NULL) {
    perror("fgets");
    fclose(fp);
#ifdef DEBUG
    printf("/sys/class/power_supply/BAT0/capacity closed.\n");
#endif
    return EXIT_FAILURE;
  }

  fclose(fp);
#ifdef DEBUG
  printf("/sys/class/power_supply/BAT0/capacity closed.\n");
#endif
  int scanned = sscanf(line, "%d", &battery);

  if (scanned == EOF || scanned < 1) {
    perror("sscanf");
    fclose(fp);
#ifdef DEBUG
    printf("/sys/class/power_supply/BAT0/capacity closed.\n");
#endif
    return EXIT_FAILURE;
  }

  battery_scaled = (int) (0.5 + length*battery/100);
  return EXIT_SUCCESS;
}

int update(int initial) {
  int result = update_cpu(initial);

  if (result != EXIT_SUCCESS) {
    return result;
  }

  if (initial || battery  != -1) {
    result = update_battery();

    if (result != EXIT_SUCCESS) {
      return result;
    }
  }

  result = update_memory();
  return result;
}

int init_cpu() {
  FILE* fp = fopen("/proc/stat", "r");
#ifdef DEBUG
  printf("/proc/stat opened.\n");
#endif

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
#ifdef DEBUG
      printf("/proc/stat closed.\n");
#endif
      perror("fgets");
      return EXIT_FAILURE;
    }

    if (strncmp(line, "cpu", 3) != 0) {
      break;
    } else {
      cpus++;
    }
  }

  fclose(fp);
#ifdef DEBUG
  printf("/proc/stat closed.\n");
#endif
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

  int initial_update = update(1);

  if (initial_update != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }

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
    printf("^fg(%s)^r(%ldx10)", GREEN, mem_scaled.total-mem_scaled.free-mem_scaled.buffered-mem_scaled.cached);
    printf("^fg(%s)^r(%ldx10)", BLUE, mem_scaled.buffered);
    printf("^fg(%s)^r(%ldx10)", ORANGE, mem_scaled.cached);
    printf("^fg(%s)^r(%ldx10)", DARK_GREY, mem_scaled.free);
    printf("^fg()");

    if (mem.swap_total > 0) {
      printf("   ");

      printf("Swap ");
      printf("^fg(%s)^r(%ldx10)", RED, mem_scaled.swap_total-mem_scaled.swap_free);
      printf("^fg(%s)^r(%ldx10)", DARK_GREY, mem_scaled.swap_free);
      printf("^fg()");
    }

    if (battery != -1) {
      printf("   ");
      printf("Akku ");

      if (battery < 11) {
        printf("^fg(%s)", RED);
      } else if (battery < 21) {
        printf("^fg(%s)", ORANGE);
      } else {
        printf("^fg(%s)", GREEN);
      }

      printf("^r(%dx10)", battery_scaled);
      printf("^fg(%s)^r(%dx10)", DARK_GREY, length-battery_scaled);
      printf("^fg()");
    }

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
