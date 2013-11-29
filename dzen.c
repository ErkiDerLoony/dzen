#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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

struct rx {
  long long bytes;
  long long packets;
  long long errs;
  long long drop;
  long long fifo;
  long long frame;
  long long compressed;
  long long multicast;
};

struct tx {
  long long bytes;
  long long packets;
  long long errs;
  long long drop;
  long long fifo;
  long long colls;
  long long carrier;
  long long compressed;
};

struct net_info {
  char* name;
  struct rx r;
  struct tx t;
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

struct net_info* net_current;
struct net_info* net_old;
struct net_info* net_diff;
struct net_info* net_scaled;
int nets;
int* net_sum;

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

int update_net(int initial) {
  FILE* fp = fopen("/proc/net/dev", "r");

  if (fp == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  char line[1024];
  char* result = fgets(line, 1024, fp);

  if (result == NULL) {
    fclose(fp);
    perror("fgets");
    return EXIT_FAILURE;
  }

#ifdef DEBUG
  printf("Dropping line »%s«.\n", line);
#endif

  result = fgets(line, 1024, fp);

  if (result == NULL) {
    fclose(fp);
    perror("fgets");
    return EXIT_FAILURE;
  }

#ifdef DEBUG
  printf("Dropping line »%s«.\n", line);
#endif

  for (int i = 0; i < nets; i++) {
    net_old[i] = net_current[i];
  }

  for (int i = 0; i < nets; i++) {
    result = fgets(line, 1024, fp);

    if (result == NULL) {
      fclose(fp);
      perror("fgets");
      return EXIT_FAILURE;
    }

#ifdef DEBUG
    printf("Scanning line »%s«.\n", line);
#endif

    int scanned = sscanf(line, " %[^:]: %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
                         net_current[i].name,
                         &(net_current[i].r.bytes),
                         &(net_current[i].r.packets),
                         &(net_current[i].r.errs),
                         &(net_current[i].r.drop),
                         &(net_current[i].r.fifo),
                         &(net_current[i].r.frame),
                         &(net_current[i].r.compressed),
                         &(net_current[i].r.multicast),
                         &(net_current[i].t.bytes),
                         &(net_current[i].t.packets),
                         &(net_current[i].t.errs),
                         &(net_current[i].t.drop),
                         &(net_current[i].t.fifo),
                         &(net_current[i].t.colls),
                         &(net_current[i].t.carrier),
                         &(net_current[i].t.compressed));

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

  for (int i = 0; i < nets; i++) {
    net_diff[i].name = net_current[i].name;
    net_diff[i].r.bytes = net_current[i].r.bytes - net_old[i].r.bytes;
    net_diff[i].r.packets = net_current[i].r.packets - net_old[i].r.packets;
    net_diff[i].r.errs = net_current[i].r.errs - net_old[i].r.errs;
    net_diff[i].r.drop = net_current[i].r.drop - net_old[i].r.drop;
    net_diff[i].r.fifo = net_current[i].r.fifo - net_old[i].r.fifo;
    net_diff[i].r.frame = net_current[i].r.frame - net_old[i].r.frame;
    net_diff[i].r.compressed = net_current[i].r.compressed - net_old[i].r.compressed;
    net_diff[i].r.multicast = net_current[i].r.multicast - net_old[i].r.multicast;
    net_diff[i].t.bytes = net_current[i].t.bytes - net_old[i].t.bytes;
    net_diff[i].t.packets = net_current[i].t.packets - net_old[i].t.packets;
    net_diff[i].t.errs = net_current[i].t.errs - net_old[i].t.errs;
    net_diff[i].t.drop = net_current[i].t.drop - net_old[i].t.drop;
    net_diff[i].t.fifo = net_current[i].t.fifo - net_old[i].t.fifo;
    net_diff[i].t.colls = net_current[i].t.colls - net_old[i].t.colls;
    net_diff[i].t.carrier = net_current[i].t.carrier - net_old[i].t.carrier;
    net_diff[i].t.compressed = net_current[i].t.compressed - net_old[i].t.compressed;
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
      scanned = sscanf(line, "MemTotal: %ld kB", &mem.total);
      done |= 1;
    } else if (strncmp(line, "MemFree:", 8) == 0) {
      scanned = sscanf(line, "MemFree: %ld kB", &mem.free);
      done |= 2;
    } else if (strncmp(line, "Buffers:", 8) == 0) {
      scanned = sscanf(line, "Buffers: %ld kB", &mem.buffered);
      done |= 4;
    } else if (strncmp(line, "Cached:", 7) == 0) {
      scanned = sscanf(line, "Cached: %ld kB", &mem.cached);
      done |= 8;
    } else if (strncmp(line, "SwapTotal:", 10) == 0) {
      scanned = sscanf(line, "SwapTotal: %ld kB", &mem.swap_total);
      done |= 16;
    } else if (strncmp(line, "SwapFree:", 9) == 0) {
      scanned = sscanf(line, "SwapFree: %ld kB", &mem.swap_free);
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

  if (initial || battery != -1) {
    result = update_battery();

    if (result != EXIT_SUCCESS) {
      return result;
    }
  }

  result = update_memory();

  if (result != EXIT_SUCCESS) {
    return result;
  }

  result = update_net(initial);
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

  if (cpu_current == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  cpu_old = malloc(cpus*sizeof(struct cpu_info));

  if (cpu_old == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  cpu_diff = malloc(cpus*sizeof(struct cpu_info));

  if (cpu_diff == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  cpu_scaled = malloc(cpus*sizeof(struct cpu_info));

  if (cpu_scaled == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  cpu_sum = malloc(cpus*sizeof(int));

  if (cpu_sum == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int init_net() {
  FILE* fp = fopen("/proc/net/dev", "r");

  if (fp == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  // First two lines are header.
  char line[1024];
  char* got = fgets(line, 1024, fp);

  if (got == NULL) {
    fclose(fp);
    perror("fgets");
    return EXIT_FAILURE;
  }

  got = fgets(line, 1024, fp);

  if (got == NULL) {
    fclose(fp);
    perror("fgets");
    return EXIT_FAILURE;
  }

  nets = 0;

  while (1) {
    got = fgets(line, 1024, fp);

    if (got == NULL) {
      break;
    }

    nets++;
  }

  fclose(fp);

  net_current = malloc(nets*sizeof(struct net_info));

  if (net_current == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  net_old = malloc(nets*sizeof(struct net_info));

  if (net_current == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  net_diff = malloc(nets*sizeof(struct net_info));

  if (net_diff == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  net_scaled = malloc(nets*sizeof(struct net_info));

  if (net_scaled == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  net_sum = malloc(nets*sizeof(int));

  if (net_sum == NULL) {
    perror("malloc");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < nets; i++) {
    net_current[i].name = malloc(1024*sizeof(char));

    if (net_current[i].name == NULL) {
      perror("malloc");
      return EXIT_FAILURE;
    }

    net_old[i].name = malloc(1024*sizeof(char));

    if (net_old[i].name == NULL) {
      perror("malloc");
      return EXIT_FAILURE;
    }

    net_diff[i].name = malloc(1024*sizeof(char));

    if (net_diff[i].name == NULL) {
      perror("malloc");
      return EXIT_FAILURE;
    }

    net_scaled[i].name = malloc(1024*sizeof(char));

    if (net_scaled[i].name == NULL) {
      perror("malloc");
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

void free_all() {
  free(cpu_current);
  free(cpu_old);
  free(cpu_diff);
  free(cpu_scaled);
  free(cpu_sum);

  for (int i = 0; i < nets; i++) {
    free(net_current[i].name);
    free(net_old[i].name);
    free(net_diff[i].name);
    free(net_scaled[i].name);
  }

  free(net_current);
  free(net_old);
  free(net_diff);
  free(net_scaled);
  free(net_sum);
}

char* format(long long number) {
  char* result = malloc(1024*sizeof(char));

  if (result == NULL) {
    perror("malloc");
    return NULL;
  }

  double mod = 1024.0;
  double kb = number / mod;
  double mb = kb / mod;

  if (mb < 1) {
    long digits = (long)(log(kb)/log(10)) + 1;

    switch (digits) {
    case 3:
      sprintf(result, "%.1f KiB", kb);
      break;
    case 2:
      sprintf(result, "%.2f KiB", kb);
      break;
    default:
      sprintf(result, "%.3f KiB", kb);
    }

  } else {
    long digits = (long)(log(mb)/log(10)) + 1;

    switch (digits) {
    case 3:
      sprintf(result, "%.1f MiB", mb);
      break;
    case 2:
      sprintf(result, "%.2f MiB", mb);
      break;
    default:
      sprintf(result, "%.3f MiB", mb);
    }
  }

  return result;
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

  initialized = init_net();

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
      free_all();
      return EXIT_FAILURE;
    }

    for (int i = 1; i < cpus; i++) {
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

    if (nets > 0) {

#ifdef DEBUG
      for (int i = 0; i < nets; i++) {
        printf("   %s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld\n",
               net_diff[i].name,
               net_diff[i].r.bytes,
               net_diff[i].r.packets,
               net_diff[i].r.errs,
               net_diff[i].r.drop,
               net_diff[i].r.fifo,
               net_diff[i].r.frame,
               net_diff[i].r.compressed,
               net_diff[i].r.multicast,
               net_diff[i].t.bytes,
               net_diff[i].t.packets,
               net_diff[i].t.errs,
               net_diff[i].t.drop,
               net_diff[i].t.fifo,
               net_diff[i].t.colls,
               net_diff[i].t.carrier,
               net_diff[i].t.compressed);
      }
#endif

      for (int i = 0; i < nets; i++) {

        if (strncmp(net_diff[i].name, "lo", 3) != 0 && strncmp(net_diff[i].name, "virbr0", 7) != 0) {
          printf("   %s ", net_diff[i].name);
          char* size = format(net_diff[i].r.bytes);

          if (size == NULL) {
            return EXIT_FAILURE;
          }

          printf("^fg(%s)%s ", GREEN, size);
          free(size);
          size = format(net_diff[i].t.bytes);

          if (size == NULL) {
            return EXIT_FAILURE;
          }

          printf("^fg(%s)%s", RED, size);
          free(size);
          printf("^fg()");
        }
      }
    }

    printf("\n");
    fflush(stdout);
  }

  free_all();
  return EXIT_SUCCESS;
}
