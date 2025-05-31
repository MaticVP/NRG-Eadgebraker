//
// Created by matic on 30/03/2025.
//

#ifndef EDGEBRAKER_EBDATATYPES_H
#define EDGEBRAKER_EBDATATYPES_H

struct EBTriangle {
    int v[3];
};

struct Coord3D
{
    float x;
    float y;
    float z;

    Coord3D() {
        x = y = z = 0.0;
    }

    Coord3D(float xx, float yy, float zz) {
        x = xx; y = yy; z = zz;
    }
};

typedef Coord3D Vertex;
typedef Coord3D Vector;

#define MAX_SIZE 256

enum MeshType {MANIFOLD, TPATCH};
enum FileFormat {BINARY, ASKII};

#endif //EDGEBRAKER_EBDATATYPES_H
