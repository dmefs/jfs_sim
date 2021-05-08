#include <stdio.h>
#include <stdlib.h>

#define SIZE_STR                                                               \
    "Read actual size,"                                                        \
    "Read virtual size"
#define TIME_STR                                                               \
    "Access time,"                                                             \
    " write time"
#define FEATURE_STR                                                            \
    "Write TopBuffer size,"                                                    \
    "Read SCP size"

int
main(int argc, char const* argv[])
{
    if (argc != 4) {
        printf("Usage:\n");
        printf("./title <size.txt> <time.txt> <feature.txt>.");
        exit(EXIT_FAILURE);
    }
    const char* size_fname = argv[1];
    const char* time_fname = argv[2];
    const char* feature_fname = argv[3];
    FILE *fsize, *ftime, *ffeature;
    fsize = ftime = ffeature = NULL;
    if (!(fsize = fopen(size_fname, "w"))) {
        printf("Couldn't open file : %s\n", size_fname);
        goto end_fsize;
    }
    if (!(ftime = fopen(time_fname, "w"))) {
        printf("Couldn't open file : %s\n", time_fname);
        goto end_ftime;
    }
    if (!(ffeature = fopen(feature_fname, "w"))) {
        printf("Couldn't open file : %s\n", feature_fname);
        goto end_ffeature;
    }
    fwrite(SIZE_STR, 1, sizeof(SIZE_STR), fsize);
    fwrite(TIME_STR, 1, sizeof(TIME_STR), ftime);
    fwrite(FEATURE_STR, 1, sizeof(FEATURE_STR), ffeature);
    fclose(fsize);
    fclose(ftime);
    fclose(ffeature);
    return EXIT_SUCCESS;
end_ffeature:
    fclose(ftime);
end_ftime:
    fclose(fsize);
end_fsize:
    return EXIT_FAILURE;
}
