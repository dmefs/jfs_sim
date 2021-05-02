#include "jfs.h"
#include "lba.h"
#include "pba.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void
start_parsing(jfs_t* fs, char* file_name)
{
    FILE* stream;
    unsigned long lba, n, val;
    char *line, c;
    ssize_t nread;
    size_t len;
    int fid = 0;

    line = NULL;
    len = 0;

    stream = fopen(file_name, "r");
    if (!stream) {
        fprintf(stderr, "ERROR: open file failed. %s\n", strerror(errno));
        return;
    }
    while ((nread = getline(&line, &len, stream)) != -1) {
        char* p = line;
        while (*p == ' ')
            p++;
        if (*p == '#')
            continue;
        fs->ins_count++;
        if ((val = sscanf(p, "%c %d %lu %lu\n", &c, &fid, &lba, &n)) == 4) {
            switch (c) {
                case 'R':
                    fs->read_ins_count++;
                    fs->jfs_op->read(fs, lba, n, fid);
                    break;
                case 'W':
                    fs->write_ins_count++;
                    fs->jfs_op->write(fs, lba, n, fid);
                    break;
                case 'D':
                    fs->delete_ins_count++;
                    fs->jfs_op->delete (fs, lba, n, fid);
                    break;
                default:
                    break;
            }
        } else {
            fprintf(
              stderr,
              "ERROR: parsing instructions failed. Unrecongnized format.\n");
            exit(EXIT_FAILURE);
        }
    }
    jfs_check_out(fs);
    free(line);
    fclose(stream);
}

int
main(int argc, char** argv)
{
    int size, opt, len;
    char input_file[MAX_LENS + 1];
    time_t start_time, end_time;

    opt = 0;

    /* parse arguments */
    while ((opt = getopt(argc, argv, "s:i:")) != -1) {
        switch (opt) {
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
                fprintf(stderr,
                        "Usage: %s [-s size(GB)] [-i input_file_name]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    /* create virtual jfs */
    jfs_t* jj;
    if (!(jj = init_jfs(size))) {
        fprintf(stderr, "ERROR: Failed to init_jfs\n");
        exit(EXIT_FAILURE);
    } else {
        printf("[OK] Init init_jfs\n");
    }

    struct report* report = &jj->d->report;
    /* parse operations file */
    printf("Start parsing...\n");
    time(&start_time);
    start_parsing(jj, input_file);
    time(&end_time);
    printf("Start parsing[OK]\n");

    double elapsed = difftime(end_time, start_time);
    printf("-------------------------\n");
    printf("Time information:\n\n");
    printf("%f seconds total\n", elapsed);

    double elapsed = difftime(end_time, start_time);
    printf("-------------------------\n");
    printf("Time information:\n\n");
    printf("%f seconds total\n", elapsed);

    printf("-------------------------\n");
    printf("Disk information.\n");
    printf("Size of disk = %d GB\n", size);

    printf("-------------------------\n");
    printf("Transaction information:\n\n");
    printf("Total number of instructions        = %16lu instructions\n",
           report->ins_count);
    printf("Total number of read instructions   = %16lu instructions\n",
           report->read_ins_count);
    printf("\n");
    printf("Disk information.\n");
    printf("Size of disk = %d GB\n", size);

           jfs.ins_count);
           printf("Total number of read instructions   = %16lld instructions\n",
                  jfs.read_ins_count);
           printf("Total number of write instructions  = %16lld instructions\n",
                  jfs.write_ins_count);
           printf("Total number of delete instructions = %16lld instructions\n",
                  jfs.delete_ins_count);
           printf("\n");
           printf("Total read actual size      = %17lu MB\n",
                  d->total_read_actual_size / MEGABYTE);
           printf("Total read virtual size     = %17lu MB\n",
                  d->total_read_virtual_size / MEGABYTE);
           printf("Total write virtual size    = %17lu MB\n",
                  d->total_write_virtual_size / MEGABYTE);
           printf("Total write actual size     = %17lu MB\n",
                  d->total_write_actual_size / MEGABYTE);
           printf("Total delete virtual size   = %17lu MB\n",
                  d->total_delete_write_virtual_size / MEGABYTE);
           printf("Total delete actual size    = %17lu MB\n",
                  d->total_delete_write_actual_size / MEGABYTE);

           printf("-------------------------\n");
           printf("Total access time           = %17lu ns\n",
                  report->total_access_time);
           printf("Total write time            = %17lu ns\n",
                  report->total_write_time);
           printf("Total read time             = %17lu ns\n",
                  report->total_read_time);
           printf("Total read virtual time     = %17lu ns\n",
                  report->total_read_time);
           printf("\n");
           printf("Total write virtual size    = %17lu MB\n",
                  report->total_write_virtual_size / MEGABYTE);
           printf("Total write actual size     = %17lu MB\n",
                  report->total_write_actual_size / MEGABYTE);
           report->total_delete_write_virtual_size / MEGABYTE);
    printf("Total delete actual size    = %17lu MB\n",
    end_jfs(jj);

    return 0;
}
