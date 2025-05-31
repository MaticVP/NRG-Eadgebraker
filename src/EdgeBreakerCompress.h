//
// Created by matic on 30/03/2025.
//


#ifndef EDGEBRAKER_EDGEBREAKERCOMPRESS_H
#define EDGEBRAKER_EDGEBREAKERCOMPRESS_H

#include "EBdatatypes.h"
#include <vector>
#include "HEPolMesh.h"

vector<HEmesh> loadOFF(char* filename, bool color);
vector<HEmesh> loadOBJ(char *filename, bool color);

class EdgeBreakerCompress {
public:
    EdgeBreakerCompress();
    ~EdgeBreakerCompress();

    void loadOVTTable(char* filename);


    void PrepMesh();

    void compress();

    void saveAscii(char* savepath, int i);
    void saveBinary(char* filename);

    vector<HEmesh> hems;

    void updateOVTable();
    void cleanupOFF();

    HEmesh hem;
    vector<int> dummyVertices;

    void initCompression(int c,MeshType eMeshType);
    void Compress(int c);

    void CheckHandle(int c);

    void EncodeNoPrediction(int c);
    void EncodeWithPrediction(int c);
    void EncodeDelta(int c);

    char sOVTable[MAX_SIZE];
    MeshType eMeshType;
    FileFormat eFileFormat;
    int nStartCorner;

    int* O;
    int* V;
    Vertex*  G;
    Vertex*  G_est;

    int T;
    int N;
    int *M;
    int *U;

    int it1_count;
    vector<char> clers;
    vector<int> handles;
    vector<Vertex> vertices;
    vector<int> newDummy;

    void changeMesh(int i);
};


#endif //EDGEBRAKER_EDGEBREAKERCOMPRESS_H
