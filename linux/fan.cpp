#include <hidapi/hidapi.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <endian.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

struct sensors {
    int32_t processor;
    int32_t motherboard;
    int32_t chipset;
    int32_t core0;
    int32_t core1;
    int32_t core2;
    int32_t core3;
    int32_t ambient;
    int32_t radeon;
} __attribute__((packed));

struct sensors sensor_data;

void processLine(char *line) {
    if (strncasecmp(line, "Processor Fan:", 14) == 0) {
        sensor_data.processor = strtol(line + 14, NULL, 10);
        return;
    }
    if (strncasecmp(line, "Motherboard Fan:", 16) == 0) {
        sensor_data.motherboard = strtol(line + 16, NULL, 10);
        return;
    }
    if (strncasecmp(line, "Chipset Fan:", 12) == 0) {
        sensor_data.chipset = strtol(line + 12, NULL, 10);
        return;
    }
    if (strncasecmp(line, "Core 0:", 7) == 0) {
        sensor_data.core0 = strtol(line + 7, NULL, 10);
        return;
    }
    if (strncasecmp(line, "Core 1:", 7) == 0) {
        sensor_data.core1 = strtol(line + 7, NULL, 10);
        return;
    }
    if (strncasecmp(line, "Core 2:", 7) == 0) {
        sensor_data.core2 = strtol(line + 7, NULL, 10);
        return;
    }
    if (strncasecmp(line, "Core 3:", 7) == 0) {
        sensor_data.core3 = strtol(line + 7, NULL, 10);
        return;
    }
    if (strncasecmp(line, "Ambient:", 8) == 0) {
        sensor_data.ambient = strtol(line + 8, NULL, 10);
        return;
    }
    if (strncasecmp(line, "temp1:", 6) == 0) {
        sensor_data.radeon = strtol(line + 6, NULL, 10);
        return;
    }
}

wchar_t *GetWC(const char *c) {
    const size_t cSize = strlen(c)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, c, cSize);

    return wc;
}

int main(int argc, char **argv) {

    const char *opts = "v:p:s:";

    int vid = -1;
    int pid = -1;
    char *serial = NULL;

    int opt;
    while ((opt = getopt(argc,argv, opts)) != -1) {
        switch (opt) {
            case 'v': vid = strtol(optarg, NULL, 16); break;
            case 'p': pid = strtol(optarg, NULL, 16); break;
            case 's': serial = optarg; break;
        }
    }

    if ((vid == -1) || (pid == -1)) {
        printf("Usage: fan -v <vid> -p <pid> [-s <serial>]\n");
        return 0;
    }
    wchar_t *wserial = NULL;
    if (serial != NULL) {
        wserial = GetWC(serial);
    }

    hid_device *dev = NULL;

    bool connected = false;

    while (1) {

        if (!connected) {
            dev = hid_open(vid, pid, wserial);
            if (dev) {
                connected = true;
            } else {
                connected = false;
            }
        } else {
            FILE *f = popen("sensors", "r");

            if (!f) {
                printf("Cannot get sensor data\n");
                return 10;
            }

            char line[100];
            int c;

            int pos = 0;

            while ((fgets(line, 100, f) != NULL)) {
                processLine(line);
            }
            pclose(f);


            uint8_t buf[sizeof(struct sensors) + 1];


            buf[0] = 1;
            memcpy(buf + 1, &sensor_data, sizeof(struct sensors));

            int res = hid_write(dev, (const unsigned char*)buf, sizeof(struct sensors) + 1);

            if (res == -1) {
                connected = false;
            }
            usleep(100000);
        }

    }

    hid_close(dev);
    return 0;
}
