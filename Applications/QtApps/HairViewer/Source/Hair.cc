/*
Szymon Rusinkiewicz
Princeton University

Hair.cc
Photobooth hairstyle format reading
*/

#include <cstdio>
#include <algorithm>
#include <string>
#include "Hair.h"
using namespace std;
using namespace trimesh;


// Read hair
bool Hair::read(const char *filename, bool flip_strands /* = false */)
{
    bool ok = ends_with(filename, ".data") ?
        read_bin(filename) : read_asc(filename);

    if (!ok)
        return false;

    if (flip_strands) {
        int nstrands = strands.size();
        for (int i = 0; i < nstrands; i++)
            reverse(strands[i].begin(), strands[i].end());
    }

    // Look for a .xf file, and apply it if found
    xform xf;
    if (xf.read(xfname(filename)))
        apply_xf(xf);

    return true;
}


// Read hair from an ASCII .txt file
bool Hair::read_asc(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Couldn't open %s\n", filename);
        return false;
    }

    int nstrands = 0;
    if (!fscanf(f, "%d", &nstrands)) {
        fprintf(stderr, "Couldn't read number of strands\n");
        fclose(f);
        return false;
    }
    strands.resize(nstrands);

    for (int i = 0; i < nstrands; i++) {
        int nverts = 0;
        if (!fscanf(f, "%d", &nverts)) {
            fprintf(stderr, "Couldn't read number of vertices\n");
            fclose(f);
            return false;
        }
        strands[i].resize(nverts);

        for (int j = 0; j < nverts; j++) {
            if (!fscanf(f, "%f%f%f", &strands[i][j][0],
                &strands[i][j][1], &strands[i][j][2])) {
                fprintf(stderr, "Couldn't read %d-th vertex in strand %d\n", j, i);
                fclose(f);
                return false;
            }
        }
    }

    fclose(f);
    return true;
}


// Read hair from a binary .data file
bool Hair::read_bin(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Couldn't open %s\n", filename);
        return false;
    }

    int nstrands = 0;
    if (!fread(&nstrands, 4, 1, f)) {
        fprintf(stderr, "Couldn't read number of strands\n");
        fclose(f);
        return false;
    }
    strands.resize(nstrands);

    for (int i = 0; i < nstrands; i++) {
        int nverts = 0;
        if (!fread(&nverts, 4, 1, f)) {
            fprintf(stderr, "Couldn't read number of vertices\n");
            fclose(f);
            return false;
        }
        strands[i].resize(nverts);

        for (int j = 0; j < nverts; j++) {
            if (!fread(&strands[i][j][0], 12, 1, f)) {
                fprintf(stderr, "Couldn't read %d-th vertex in strand %d\n", j, i);
                fclose(f);
                return false;
            }
        }
    }

    fclose(f);
    return true;
}


// Write hair
bool Hair::write(const char *filename, bool flip_strands /* = false */)
{
    if (flip_strands) {
        int nstrands = strands.size();
        for (int i = 0; i < nstrands; i++)
            reverse(strands[i].begin(), strands[i].end());
    }

    bool ok = ends_with(filename, ".data") ?
        write_bin(filename) : write_asc(filename);

    if (flip_strands) {
        int nstrands = strands.size();
        for (int i = 0; i < nstrands; i++)
            reverse(strands[i].begin(), strands[i].end());
    }

    return ok;
}


// Write hair to an ASCII .txt file
bool Hair::write_asc(const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Couldn't open %s\n", filename);
        return false;
    }

    int nstrands = strands.size();
    fprintf(f, "%d\n", nstrands);
    for (int i = 0; i < nstrands; i++) {
        int nverts = strands[i].size();
        fprintf(f, "%d\n", nverts);
        for (int j = 0; j < nverts; j++) {
            fprintf(f, "%f %f %f\n", strands[i][j][0],
                strands[i][j][1], strands[i][j][2]);
        }
    }

    fclose(f);
    return true;
}


// Write hair to a binary .data file
bool Hair::write_bin(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Couldn't open %s\n", filename);
        return false;
    }

    int nstrands = strands.size();
    fwrite(&nstrands, 4, 1, f);
    for (int i = 0; i < nstrands; i++) {
        int nverts = strands[i].size();
        fwrite(&nverts, 4, 1, f);
        for (int j = 0; j < nverts; j++) {
            fwrite(&strands[i][j][0], 12, 1, f);
        }
    }

    fclose(f);
    return true;
}


// Compute per-vertex strand tangents
void Hair::compute_tangents()
{
    int ns = strands.size();
    tangents.clear();
    tangents.resize(ns);

#pragma omp parallel for
    for (int i = 0; i < ns; i++) {
        int n = strands[i].size();
        if (!n) {
            continue;
        } else if (n == 1) {
            tangents[i].push_back(vec(0,0,-1));
            continue;
        }
        tangents[i].resize(n);
        for (int j = 0; j < n-1; j++) {
            vec t = strands[i][j+1] - strands[i][j];
            normalize(t);
            tangents[i][j] += t;
            tangents[i][j+1] += t;
        }
        for (int j = 0; j < n; j++)
            normalize(tangents[i][j]);
#if 0
        if (strands[i][0][1] <= strands[i].back()[1])
            continue;
        for (int j = 0; j < n; j++)
            tangents[i][j] = -tangents[i][j];
#endif
    }
}


// Apply a transform to the hair
void Hair::apply_xf(const xform &xf)
{
    int nstrands = strands.size();
#pragma omp parallel for
    for (int i = 0; i < nstrands; i++) {
        int nverts = strands[i].size();
        for (int j = 0; j < nverts; j++)
            strands[i][j] = xf * strands[i][j];
    }
}


// Apply subdivision to strands
void Hair::subdiv()
{
    int nstrands = strands.size();
    for (int i = 0; i < nstrands; i++) {
        int nverts = strands[i].size();
        if (nverts < 2)
            continue;
        Strand oldstrand(strands[i]);
        strands[i].clear();
        strands[i].reserve(2 * nverts - 1);
        for (int j = 0; j < nverts - 1; j++) {
            strands[i].push_back(oldstrand[j]);
            point p = 0.5f * (oldstrand[j] + oldstrand[j+1]);
            if (j > 0)
                p += 0.0625f * (oldstrand[j] - oldstrand[j-1]);
            if (j < nverts - 2)
                p += 0.0625f * (oldstrand[j+1] - oldstrand[j+2]);
            strands[i].push_back(p);
        }
        strands[i].push_back(oldstrand.back());
    }
}
