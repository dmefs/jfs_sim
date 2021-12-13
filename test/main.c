#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "jfs.h"
#include "lba.h"
#include "pba.h"

const char *size_fname = "bin/size.log";
const char *time_fname = "bin/time.log";
const char *feature_fname = "bin/feature.log";
bool log_mode = false;
bool is_csv_flag = false;
#ifdef VIRTUAL_GROUPS
extern unsigned long granularity;
#endif
unsigned long long bytes;

void parsing_csv(jfs_t *fs, FILE *stream)
{
    unsigned long lba, n, val;
    char *line = NULL;
    ssize_t nread;
    size_t len;
    unsigned long fid, remain, remainder, num_bytes, left, num_traces, percent,
        ten_percent;
    int c;
    num_traces = 4500000;
    percent = num_traces / 100;
    ten_percent = num_traces / 10;
    while ((num_traces--) && ((nread = getline(&line, &len, stream)) != -1)) {
        char *p = line;
        while (*p == ' ')
            p++;
        if (*p == '#')
            continue;
        if ((val = sscanf(p, "%d,%lu,%llu,%lu\n", &c, &fid, &bytes,
                          &num_bytes)) == 4) {
            fs->ins_count++;
            lba = bytes / SECTOR_SIZE;
            remainder = bytes % SECTOR_SIZE;
            remain = (remainder == 0 ? 0 : SECTOR_SIZE - remainder);
            n = 0;
            if (num_bytes == 0) {
                n = 0;
            } else if (remain < num_bytes) {
                n = !!remain;
                left = num_bytes - remain;
                n += (left / SECTOR_SIZE) + !!(left % SECTOR_SIZE);
            } else {
                n = 1;
            }
            switch (c) {
            case 1:
                fs->read_ins_count++;
                fs->jfs_op->read(fs, lba, n, fid);
                break;
            case 2:
                fs->write_ins_count++;
                fs->jfs_op->write(fs, lba, n, fid);
                break;
            case 3:
                fs->delete_ins_count++;
                fs->jfs_op->delete (fs, lba, n, fid);
                break;
            default:
                fprintf(stderr,
                        "ERROR: parsing instructions failed. Unrecongnized "
                        "mode. mode: %d\n",
                        c);
                break;
            }
        } else {
            fprintf(
                stderr,
                "ERROR: parsing instructions failed. Unrecongnized format.\n");
            exit(EXIT_FAILURE);
        }
        if (!(num_traces % percent)) {
            putchar('#');
            fflush(stdout);
        }
        if (!(num_traces % ten_percent)) {
            putchar('\n');
            fflush(stdout);
        }
    }
    free(line);
}

void parsing_postmark(jfs_t *fs, FILE *stream)
{
    unsigned long lba, n, val;
    char *line;
    int c;
    ssize_t nread;
    size_t len;
    unsigned long fid;
    line = NULL;

    while ((nread = getline(&line, &len, stream)) != -1) {
        char *p = line;
        while (*p == ' ')
            p++;
        if (*p == '#')
            continue;
        fs->ins_count++;
        if ((val = sscanf(p, "%d,%lu,%lu,%lu\n", &c, &fid, &lba, &n)) == 4) {
            switch (c) {
            case 1:
                fs->read_ins_count++;
                fs->jfs_op->read(fs, lba, n, fid);
                break;
            case 2:
                fs->write_ins_count++;
                fs->jfs_op->write(fs, lba, n, fid);
                break;
            case 3:
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
    free(line);
}

void start_parsing(jfs_t *fs, char *file_name)
{
    FILE *stream;

    stream = fopen(file_name, "r");
    if (!stream) {
        fprintf(stderr, "ERROR: open file failed. %s\n", strerror(errno));
        return;
    }
    if (is_csv_flag)
        parsing_csv(fs, stream);
    else
        parsing_postmark(fs, stream);
    jfs_check_out(fs);
    fclose(stream);
}

FILE *fsize, *ftime, *ffeature;
int open_log_files()
{
    if (!(fsize = fopen(size_fname, "a"))) {
        printf("Couldn't open file : %s\n", size_fname);
        return EXIT_FAILURE;
    }
    if (!(ftime = fopen(time_fname, "a"))) {
        printf("Couldn't open file : %s\n", time_fname);
        return EXIT_FAILURE;
    }
    if (!(ffeature = fopen(feature_fname, "a"))) {
        printf("Couldn't open file : %s\n", feature_fname);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

#if 0
void log_info(struct report *re)
{
    char s[200];
    int n = 0;
    if (EXIT_SUCCESS != open_log_files()) {
        fprintf(stderr, "Couldn't open log files.\n");
        exit(EXIT_FAILURE);
    }
    n = sprintf(s, "%llu,%llu,%llu,%llu,%llu,%llu,%llu\n",
                re->total_read_actual_size / MEGABYTE,
                re->total_read_virtual_size / MEGABYTE,
                re->total_write_actual_size / MEGABYTE,
                re->total_write_virtual_size / MEGABYTE,
                re->total_delete_write_actual_size / MEGABYTE,
                re->total_delete_write_virtual_size / MEGABYTE,
                re->total_delete_read_virtual_size / MEGABYTE);
    fwrite(s, 1, n, fsize);
    n = sprintf(s, "%llu,%llu,%llu,%llu,%llu,%llu\n", re->total_access_time,
                re->total_write_time, re->total_read_time,
                re->total_read_virtual_time, re->total_delete_write_time,
                re->total_delete_read_time);
    fwrite(s, 1, n, ftime);
    fclose(fsize);
    fclose(ftime);
    fclose(ffeature);
}
#endif

int main(int argc, char **argv)
{
    int size, opt, len;
    char input_file[MAX_LENS + 1];
    time_t start_time, end_time;

    opt = 0;

    /* parse arguments */
    while ((opt = getopt(argc, argv, "cls:i:g:")) != -1) {
        switch (opt) {
        case 'c':
            is_csv_flag = true;
            break;
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
        case 'l':
            log_mode = true;
            break;
#ifdef VIRTUAL_GROUPS
        case 'g':
            granularity = atoi(optarg);
            break;
#endif
        default:
            fprintf(stderr, "Usage: %s [-s size(GB)] [-i input_file_name]\n",
                    argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }

    /* create virtual jfs */
    jfs_t *jj;
    if (!(jj = init_jfs(size))) {
        fprintf(stderr, "ERROR: Failed to init_jfs\n");
        exit(EXIT_FAILURE);
    } else {
        printf("[OK] Init init_jfs\n");
    }

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

    struct report *report = &jj->d->report;
    // if (log_mode)
    //     log_info(report);
    printf("-------------------------\n");
    printf("Disk information.\n");
    printf("Size of disk = %d GB\n", size);
    printf("-------------------------\n");
    printf("Transaction information:\n\n");
    printf("Total number of read instructions   = %16lld instructions\n",
           jfs.read_ins_count);
    printf("Total number of write instructions  = %16lld instructions\n",
           jfs.write_ins_count);
    printf("Total number of delete instructions = %16lld instructions\n",
           jfs.delete_ins_count);
    printf("Total number of invalid read        = %16llu blocks\n",
           report->num_invalid_read);
    printf("Total number of invalid write       = %16llu blocks\n",
           report->num_invalid_write);

#ifdef TOP_BUFFER
    printf("-------------------------\n");
    printf("Total write top buffer size = %17llu MB\n",
           report->total_write_top_buffer_size / MEGABYTE);
    printf("Total read scp size         = %17llu MB\n",
           report->total_read_scp_size / MEGABYTE);
    printf("Total scp count             = %17d times\n", report->scp_count);
#endif
#ifdef BLOCK_SWAP
    printf("Total block swap count      = %17lld blocks\n",
           report->current_block_swap_count);
#endif
#ifdef VIRTUAL_GROUPS
    printf("granularity                 =  %lu tracks\n", granularity);
    printf("Total dual swap count       = %17lld blocks\n",
           report->dual_swap_count);
#endif

    printf("-------------------------\n");
    printf("Secure Deletion Latency     = %17llu ns\n",
           report->total_delete_write_time + report->total_delete_read_time);
    printf("#########################\n");
    printf("###### Normal Mode ######\n");
    printf("#########################\n");
    printf("Read Latency                = %17llu ns\n",
           report->normal.total_read_time);
    printf("Write Latency               = %17llu ns\n",
           report->normal.total_write_time +
               report->normal.total_read_virtual_time);
    printf("Accumulated Write Size      = %17llu B\n",
           report->normal.total_write_actual_size +
               report->normal.total_write_virtual_size);
    printf("Accumulated Read Size       = %17llu B\n",
           report->normal.total_read_actual_size);
    printf("#########################\n");
    printf("#### Journaling Mode ####\n");
    printf("#########################\n");
    printf("Read Latency                = %17llu ns\n",
           report->journaling.total_read_time);
    printf("Write Latency               = %17llu ns\n",
           report->journaling.total_write_time +
               report->journaling.total_read_virtual_time);
    printf("Accumulated Write Size      = %17llu B\n",
           report->journaling.total_write_actual_size +
               report->journaling.total_write_virtual_size);
    printf("Accumulated Read Size       = %17llu B\n",
           report->journaling.total_read_actual_size);
    printf("#########################\n");
    printf("Accumulated SD Size         = %17llu B\n",
           report->total_delete_write_actual_size +
               report->total_delete_write_virtual_size);

    printf("Accumulated TR Size         = %17llu B\n",
           report->normal.total_write_virtual_size +
               report->journaling.total_write_virtual_size +
               report->total_delete_write_virtual_size);
    printf("-------------------------\n");
    end_jfs(jj);

    return 0;
}
