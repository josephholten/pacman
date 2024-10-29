#ifndef PNM_H
#define PNM_H

#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <raylib.h>

enum PNMType : uint8_t {
    PBM_ASCII = 1,
    PGM_ASCII,
    PPM_ASCII,
    PBM_RAW,
    PGM_RAW,
    PPM_RAW,
};

struct PNM {
    uint8_t type;
    uint8_t width;
    uint8_t height;
    uint8_t levels;
    void* data;
};

uint8_t PNMGetGray(PNM* pnm, int row, int col) {
    return ((uint8_t*)pnm->data)[row*pnm->width + col];
}

PNM LoadPNM(const char* filePath) {
    PNM pnm;

    int fd = open(filePath, O_RDONLY);
    if (fd < 0) {
        TraceLog(LOG_ERROR, "could not open %s", filePath);
        perror("open");
        return pnm;
    }

    char buf[1024];
    read(fd, buf, 1024);

    // parse magic number 'Px' with x in 1...6
    if (buf[0] != 'P' || buf[1] < '1' || buf[1] > '6') {
        TraceLog(LOG_ERROR, "PNM: magicnumber %.*s invalid", 2, buf);
        return pnm;
    }
    // set type based on Px
    pnm.type = buf[1];
    int offset = 3;
    if (buf[offset] == '\n') {
        offset++;
    }
    // ignore comments
    if (buf[offset] == '#') {
        int start = offset;
        while (buf[offset] != '\n')
            offset++;
        offset++; // skip newline
        int end = offset;
        TraceLog(LOG_INFO, "skipped comment: %.*s", end-start, buf+start);
    }
    // parse width
    pnm.width = 0;
    while (buf[offset] != ' ') {
        pnm.width *= 10;
        pnm.width += buf[offset] - '0';
        offset++;
    }
    offset++; // skip whitespace
    // parse height
    pnm.height = 0;
    while (buf[offset] != '\n') {
        pnm.height *= 10;
        pnm.height += buf[offset] - '0';
        offset++;
    }
    offset++; // skip newline
    // parse gray levels
    pnm.levels = 0;
    while (buf[offset] != '\n') {
        pnm.levels *= 10;
        pnm.levels += buf[offset] - '0';
        offset++;
    }
    offset++;
    TraceLog(LOG_INFO, "parsed pnm image of type P%c of size %dx%d, offset=%d, levels=%d", pnm.type, pnm.width, pnm.height, offset, pnm.levels);

    // +1 for the newline character at each end
    pnm.data = mmap(NULL, pnm.height*pnm.width, PROT_READ, MAP_PRIVATE, fd, 0);
    pnm.data = (uint8_t*)pnm.data + offset;
    close(fd);
    if (pnm.data == MAP_FAILED) {
        perror("mmap");
        return pnm;
    }

    return pnm;
}

void UnloadPNM(PNM* pnm) {
    munmap(pnm->data, pnm->height*pnm->width);
}

#endif /* PNM_H */