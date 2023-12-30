// tarsau.h

#ifndef TARSAU_H
#define TARSAU_H

#include <stdio.h>

void bundleFiles(int argc, char *argv[]);
void extractFiles(char *archiveFile, char *directory);
int isTextFile(const char *filename);
void writeArchiveInfo(FILE *archive, const char *filename, long size);
void writeArchiveContents(FILE *archive, const char *filename, long size);
void extractArchiveContents(FILE *archive, const char *directory);

#endif // TARSAU_H
