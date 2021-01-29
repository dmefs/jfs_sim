#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lba.h"
#include "pba.h"


void start_parsing(struct disk* d, char *file_name)
{
    FILE *stream;
    unsigned long lba, n, val;
    char *line, c;
    ssize_t nread;
    size_t len;

    line = NULL;
    len = 0;

    stream = fopen(file_name, "r");
    if (!stream) {
        fprintf(stderr, "ERROR: open file failed. %s\n", strerror(errno));
        return;
    }
    while ((nread = getline(&line, &len, stream)) != -1) {
        char *p = line;
        while (*p == ' ')
            p++;
        if (*p == '#')
            continue;
        if ((val = sscanf(p, "%c %lu %lu\n", &c, &lba, &n)) == 3) {
            printf("Command: ");
            switch (c) {
            case 'R':
                printf("R %lu, %lu\n", lba, n);
                d->d_op->read(d, lba, n);
                break;
            case 'W':
                printf("W %lu, %lu\n", lba, n);
                d->d_op->write(d, lba, n);
                break;
            default:
                fprintf(stderr, "ERROR: parsing instructions failed. Unrecongnized mode.\n");
                break;
            }
        } else {
            fprintf(stderr, "ERROR: parsing instructions failed. Unrecongnized format.\n");
            exit(EXIT_FAILURE);
        }
    }
    free(line);
    fclose(stream);
}

int main(int argc, char **argv)
{
    int size, opt, len;
    char input_file[MAX_LENS+1];

    opt         = 0;

    /* parse arguments */
    while ((opt = getopt(argc, argv, "s:i:")) != -1) {
        switch (opt)
        {
        case 's':
            size = atoi(optarg);
            break;
        case 'i':
            len = strlen(optarg);
            if (len > MAX_LENS)
                len = MAX_LENS;
            strncpy(input_file, optarg, len);
            input_file[len] = '\0';
            break;
        default:
            fprintf(stderr, "Usage: %s [-s size(GB)] [-i input_file_name]\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }

    /* create virtual disk */
    struct disk d = {0};
    if (init_disk(&d, size)) {
        fprintf(stderr, "ERROR: init_disk failed\n");
        exit(EXIT_FAILURE);
    } else {
        printf("[OK] Init disk\n");
    }

    /* parse operations file */
    start_parsing(&d, input_file);
    printf("Total access time   = %lu ns\n", d.total_access_time);
    printf("Total read size     = %lu sector\n", d.total_read_size / SECTOR_SIZE);
    printf("Total read size     = %lu sector\n", d.total_read_virtual_size / SECTOR_SIZE);
    printf("Total write size    = %lu sector\n", d.total_write_size / SECTOR_SIZE);
    end_disk(&d);
    return 0;
}
