//
// Created by matic on 30/03/2025.
//

#include <vector>
#include <cstdio>
#include "EBdatatypes.h"
#include "HEPolMesh.h"
#include <iostream>

#ifndef EDGEBRAKER_EDGEBREAKERDECOMPRESS_H
#define EDGEBRAKER_EDGEBREAKERDECOMPRESS_H

int countNumberOfMeshes(char*);
void saveOFF(char* filename, vector<HEmesh> hems);
void saveOBJ(const char* filename, vector<HEmesh>& hems);

class EdgebreakerDecompress {

public:
    EdgebreakerDecompress();
    ~EdgebreakerDecompress();

    void loadAscii(char* directory, int i);
    void loadBinary(char* filename);

    void uncompress();


    void InitDecompressionModule();
    void initDecompression();
    void DecompressConnectivity(int c);
    bool CheckHandle(int c);
    void Zip(int c);
    void DecompressVertices(int c);
    Vertex DecodeDelta(int c);
    Vertex DecodeNoPrediction(int c);
    Vertex DecodeWithPrediction(int c);

    void ReadMeshType(FILE* fclers);
    void ReadNumTriOnFirstVertex(FILE* fclers);
    void ReadClersAndUppendClersForFirstVertexTri(FILE* fclers);

    void cleanupDummy();

    MeshType eMeshType;
    FileFormat eFileFormat;
    int nNumOfTriangles;
    int nNumOfVertices;
    int* O;
    int* V;
    Vertex* G;

    int T;
    int N;
    int I;
    int A;
    int *M;
    int *U;

    vector<int> H;
    vector<char> C;
    vector<Vertex> G_in;
    vector<int> dummy;

    HEmesh hem;
};

#endif //EDGEBRAKER_EDGEBREAKERDECOMPRESS_H
