//
// Created by matic on 30/03/2025.
//

#ifdef _WIN32
#include <io.h>
#define read  _read
#define write _write
#define cclose _close

#include <fcntl.h>
// Define POSIX permission macros for Windows
#ifndef S_IRUSR
#define S_IRUSR _S_IREAD
#endif
#ifndef S_IWUSR
#define S_IWUSR _S_IWRITE
#endif
#ifndef S_IRGRP
#define S_IRGRP 0
#endif
#ifndef S_IROTH
#define S_IROTH 0
#endif

#else
#include <unistd.h>    // for read, write, close on Linux/macOS
    #include <sys/stat.h>  // for S_IRUSR etc.
#endif

#include <fcntl.h>
#include <string.h>
#include <cassert>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "EBBinStreamIO.h"

// You might want to add this if Vector is a custom class or struct, or include its header
// #include "Vector.h"

using std::string;
using std::vector;

int EBBinStreamIO::openFileWrite(char *filename) {
    // Mode flags combined for file permissions cross-platform

    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC);
    return fd;
}

int EBBinStreamIO::openFileRead(char *filename) {
    int fd = open(filename, O_RDONLY);
    return fd;
}

void EBBinStreamIO::close(int fd) {
    if (fd != -1) {
        ::cclose(fd);
    }
}

void EBBinStreamIO::writeShortArray(int fd, short *buffer, int count) {
    int retval = write(fd, buffer, count * sizeof(short));
    assert(retval == count * sizeof(short));
}

void EBBinStreamIO::writeFloatArray(int fd, float *buffer, int count) {
    int retval = write(fd, buffer, count * sizeof(float));
    assert(retval == count * sizeof(float));
}

void EBBinStreamIO::writeIntArray(int fd, int *buffer, int count) {
    int retval = write(fd, buffer, count * sizeof(int));
    assert(retval == count * sizeof(int));
}

void EBBinStreamIO::writeDoubleArray(int fd, double *buffer, int count) {
    int retval = write(fd, buffer, count * sizeof(double));
    assert(retval == count * sizeof(double));
}

void EBBinStreamIO::writeUCharArray(int fd, unsigned char *buffer, int count) {
    int retval = write(fd, buffer, count * sizeof(unsigned char));
    assert(retval == count * sizeof(unsigned char));
}

void EBBinStreamIO::writeCharArray(int fd, char *buffer, int count) {
    int retval = write(fd, buffer, count * sizeof(char));
    assert(retval == count * sizeof(char));
}

void EBBinStreamIO::writeString(int fd, char *buffer) {
    int len = (int)strlen(buffer);
    int retval = write(fd, buffer, len * sizeof(char));
    assert(retval == len * sizeof(char));
}

void EBBinStreamIO::writeString(int fd, string &s) {
    short len = static_cast<short>(s.size());
    // write length first
    int retval = write(fd, &len, sizeof(short));
    assert(retval == sizeof(short));

    // write string content
    retval = write(fd, s.data(), len * sizeof(char));
    assert(retval == len * sizeof(char));
}

void EBBinStreamIO::readShortArray(int fd, short *buffer, int count) {
    int retval = read(fd, buffer, count * sizeof(short));
    assert(retval == count * sizeof(short));
}

void EBBinStreamIO::readIntArray(int fd, int *buffer, int count) {
    int retval = read(fd, buffer, count * sizeof(int));
    assert(retval == count * sizeof(int));
}

void EBBinStreamIO::readFloatArray(int fd, float *buffer, int count) {
    int retval = read(fd, buffer, count * sizeof(float));
    assert(retval == count * sizeof(float));
}

void EBBinStreamIO::readDoubleArray(int fd, double *buffer, int count) {
    int retval = read(fd, buffer, count * sizeof(double));
    assert(retval == count * sizeof(double));
}

void EBBinStreamIO::readCharArray(int fd, char *buffer, int count) {
    int retval = read(fd, buffer, count * sizeof(char));
    assert(retval == count * sizeof(char));
}

void EBBinStreamIO::readUCharArray(int fd, unsigned char *buffer, int count) {
    int retval = read(fd, buffer, count * sizeof(unsigned char));
    assert(retval == count * sizeof(unsigned char));
}

void EBBinStreamIO::readString(int fd, char **buffer) {
    short len;
    int retval = read(fd, &len, sizeof(short));
    assert(retval == sizeof(short));
    *buffer = new char[len + 1];
    retval = read(fd, *buffer, len * sizeof(char));
    assert(retval == len * sizeof(char));
    (*buffer)[len] = '\0';
}

void EBBinStreamIO::readString(int fd, string &s) {
    short len;
    int retval = read(fd, &len, sizeof(short));
    assert(retval == sizeof(short));
    char *buffer = new char[len + 1];
    retval = read(fd, buffer, len * sizeof(char));
    assert(retval == len * sizeof(char));
    buffer[len] = '\0';

    s.assign(buffer, len);

    delete[] buffer;
}

void EBBinStreamIO::writeFloatVectorArray(int fd, vector<Vector> &vectors) {
    int len = (int)vectors.size();
    float *buffer = new float[3 * len];
    for (int i = 0; i < len; i++) {
        buffer[3 * i] = vectors[i].x;
        buffer[3 * i + 1] = vectors[i].y;
        buffer[3 * i + 2] = vectors[i].z;
    }
    int retval = write(fd, buffer, 3 * len * sizeof(float));
    assert(retval == 3 * len * sizeof(float));
    delete[] buffer;
}

void EBBinStreamIO::readFloatVectorArray(int fd, vector<Vector> &vectors, int count) {
    float *buffer = new float[3 * count];
    int retval = read(fd, buffer, 3 * count * sizeof(float));
    assert(retval == 3 * count * sizeof(float));

    vectors.clear();
    vectors.reserve(count);

    for (int i = 0; i < count; i++) {
        Vector v;
        v.x = buffer[3 * i];
        v.y = buffer[3 * i + 1];
        v.z = buffer[3 * i + 2];
        vectors.push_back(v);
    }
    delete[] buffer;
}
