// tarsau.c
#include "tarsau.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "tarsau.h"

#define MAX_FILES 32
#define MAX_TOTAL_SIZE (200 * 1024 * 1024)

int isTextFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        return 0;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF && ch <= 127)
        ; // ASCII characters

    fclose(file);
    return ch == EOF;
}

void writeArchiveInfo(FILE *archive, const char *filename, long size)
{
    struct stat st;
    if (stat(filename, &st) == -1)
    {
        perror("Error getting file stats");
        exit(1);
    }

    fprintf(archive, "|%s,%o,%ld|", filename, st.st_mode, size);
}

void writeArchiveContents(FILE *archive, const char *filename, long size)
{
    FILE *inputFile = fopen(filename, "rb");
    if (!inputFile)
    {
        perror("Error opening file");
        exit(1);
    }

    int ch;
    for (long i = 0; i < size; i++)
    {
        ch = fgetc(inputFile);
        if (ch == EOF)
        {
            perror("Error reading file contents");
            fclose(inputFile);
            exit(1);
        }
        fputc(ch, archive);
    }

    fclose(inputFile);
}

void bundleFiles(int argc, char *argv[])
{
    char *archiveFilename = "a.sau"; // Default archive file name
    long totalSize = 0;
    int numFiles = 0;

    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
        {
            archiveFilename = argv[i + 1];
            i += 2; // Skip both -o and its value
            continue;
        }

        if (strcmp(argv[i], "-o") == 0)
        {
            continue; // Skip the -o parameter itself without processing it as a file
        }

        if (!isTextFile(argv[i]))
        {
            fprintf(stderr, "%s input file format is incompatible!\n", argv[i]);
            exit(1);
        }

        struct stat st;
        if (stat(argv[i], &st) == -1)
        {
            perror("Error getting file stats");
            exit(1);
        }

        if (totalSize + st.st_size > MAX_TOTAL_SIZE)
        {
            fprintf(stderr, "Total file size exceeded %d MB limit\n", MAX_TOTAL_SIZE / (1024 * 1024));
            exit(1);
        }
        totalSize += st.st_size;
        numFiles++;

        if (numFiles > MAX_FILES)
        {
            fprintf(stderr, "Number of input files exceeded %d limit\n", MAX_FILES);
            exit(1);
        }
    }

    if (numFiles == 0)
    {
        fprintf(stderr, "No input files specified.\n");
        exit(1);
    }

    FILE *archive = fopen(archiveFilename, "w");
    if (!archive)
    {
        perror("Error creating archive file");
        exit(1);
    }

    fprintf(archive, "%010ld", totalSize); // Write total size as the first 10 characters

    for (int i = 2; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0)
        {
            i++; // Skip both -o and its value
            continue;
        }

        if (!isTextFile(argv[i]))
        {
            fprintf(stderr, "%s input file format is incompatible!\n", argv[i]);
            exit(1);
        }

        struct stat st;
        if (stat(argv[i], &st) == -1)
        {
            perror("Error getting file stats");
            exit(1);
        }

        totalSize += st.st_size;

        if (totalSize > MAX_TOTAL_SIZE)
        {
            fprintf(stderr, "Total file size exceeded %d MB limit\n", MAX_TOTAL_SIZE / (1024 * 1024));
            exit(1);
        }

        writeArchiveInfo(archive, argv[i], st.st_size);
        writeArchiveContents(archive, argv[i], st.st_size);
    }

    fclose(archive);
    printf("The files have been merged.\n");
}

void extractFiles(char *archiveFile, char *directory)
{
    FILE *archive = fopen(archiveFile, "rb");
    if (!archive)
    {
        perror("Error opening archive file");
        exit(1);
    }

    struct stat st = {0};
    if (stat(directory, &st) == -1)
    {
        if (mkdir(directory, 0700) != 0)
        {
            perror("Error creating directory");
            exit(1);
        }
    }

    extractArchiveContents(archive, directory);

    fclose(archive);
}

void extractArchiveContents(FILE *archive, const char *directory)
{
    // Read the total size from the archive
    char totalSizeStr[11];
    fread(totalSizeStr, 10, 1, archive);
    totalSizeStr[10] = '\0';

    char filename[256], permissions[10], sizeStr[20];
    long size;

    while (fscanf(archive, "|%255[^,],%9[^,],%19[^|]|", filename, permissions, sizeStr) == 3)
    {
        size = strtol(sizeStr, NULL, 10);

        mode_t mode = strtol(permissions, NULL, 8);

        char fullPath[512];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, filename);

        FILE *outFile = fopen(fullPath, "wb");
        if (!outFile)
        {
            perror("Error creating file in directory");
            exit(1);
        }
        printf("%s ", filename);
        chmod(fullPath, mode);

        int ch;
        for (long i = 0; i < size; i++)
        {
            ch = fgetc(archive);
            if (ch == EOF)
            {
                perror("Error reading file contents");
                fclose(outFile);
                exit(1);
            }
            fputc(ch, outFile);
        }

        fclose(outFile);
    }
    printf("files opened in the %s directory.\n", directory);
}
