#include <stdio.h>

typedef struct
{
    int n;
    double elapsed_s;
} result;

void export_results(const result *results, const int n, const char *path)
{
    FILE *fp = fopen(path, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", path);
        return;
    }

    for (int i = 0; i < n; i++)
        fprintf(fp, "%d\t%f\n", results[i].n, results[i].elapsed_s * 1e3);

    fclose(fp);
}