// main.c

#include <stdio.h>
#include "tarsau.h"
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: tarsau -b files... -o archive\n");
        fprintf(stderr, "       tarsau -a archive [directory]\n");
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0)
    {
        bundleFiles(argc, argv);
    }
    else if (strcmp(argv[1], "-a") == 0)
    {
        char *archiveFile = argv[2];
        char *directory = argc > 3 ? argv[3] : ".";
        extractFiles(archiveFile, directory);
    }
    else
    {
        fprintf(stderr, "Invalid option\n");
        return 1;
    }

    return 0;
}
