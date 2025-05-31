//
// Created by matic on 30/03/2025.
//

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <sstream>
#include "EdgeBreakerCompress.h"
#include "EBBinStreamIO.h"
#include "EBUtils.h"
using namespace std;

struct RawMeshInfo {
    vector<Vertex> vectexs_info;
    vector<int> poly_info;
};

RawMeshInfo cleanUpRawMeshInfo(RawMeshInfo meshInfo) {
    map<pair<int, int>, int> edgeCount;

    for (int i = 0; i < meshInfo.poly_info.size(); i += 3) {
        int v[3] = { meshInfo.poly_info[i], meshInfo.poly_info[i+1], meshInfo.poly_info[i+2] };

        for (int j = 0; j < 3; ++j) {
            int a = v[j];
            int b = v[(j+1)%3];

            pair<int, int> edge = (a < b) ? make_pair(a, b) : make_pair(b, a);
            edgeCount[edge]++;
        }
    }

    int sharedEdgeCount = 0;
    for (const auto& entry : edgeCount) {
        if (entry.second > 2) {
            sharedEdgeCount++;
            cout << "Edge (" << entry.first.first << ", " << entry.first.second
                 << ") is shared by " << entry.second << " polygons." << endl;
        }
    }

    vector<Vertex> cleanedVertices = meshInfo.vectexs_info;
    vector<int> cleanedFaces = meshInfo.poly_info;

    for (int i = 0; i < meshInfo.poly_info.size(); i += 3) {
        int v[3] = { meshInfo.poly_info[i], meshInfo.poly_info[i+1], meshInfo.poly_info[i+2] };
        bool fixUp = false;


        for (int j = 0; j < 3; ++j) {
            int a = v[j], b = v[(j+1)%3];
            auto edge = (a < b) ? make_pair(a, b) : make_pair(b, a);
            if (edgeCount[edge] > 2) {
                fixUp = true;
            }

            if (fixUp) {

                cleanedVertices.push_back(meshInfo.vectexs_info[v[j]]);
                cleanedVertices.push_back(meshInfo.vectexs_info[v[(j+1)%3]]);
                cleanedVertices.push_back(meshInfo.vectexs_info[v[(j+2)%3]]);

                meshInfo.poly_info[i+j] = cleanedVertices.size()-3;
                meshInfo.poly_info[i+(j+1)%3] = cleanedVertices.size()-2;
                meshInfo.poly_info[i+(j+2)%3] = cleanedVertices.size()-1;
                edgeCount[edge]--;
                fixUp = false;
            }
        }
    }

    meshInfo.vectexs_info = cleanedVertices;

    return meshInfo;
}

vector<HEmesh> loadOBJ(char *filename, bool color) {

    RawMeshInfo meshInfo;
    std::ifstream in(filename);
    std::string line;
    std::vector<Vertex>& vertices = meshInfo.vectexs_info;
    std::vector<int>& indices = meshInfo.poly_info;

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vertex v;
            ss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (prefix == "f") {
            std::vector<int> face_indices;
            std::string token;
            while (ss >> token) {
                std::stringstream fs(token);
                std::string v_idx_str;
                std::getline(fs, v_idx_str, '/'); // Get vertex index (ignore vt/vn)
                int idx = std::stoi(v_idx_str) - 1; // .obj is 1-based
                face_indices.push_back(idx);
            }

            // Triangulate if needed (fan method)
            for (size_t i = 1; i + 1 < face_indices.size(); ++i) {
                indices.push_back(face_indices[0]);
                indices.push_back(face_indices[i]);
                indices.push_back(face_indices[i + 1]);
            }
        }
    }

    cout << "Mesh loaded" << endl;

    HEmesh tempMesh;

    meshInfo = cleanUpRawMeshInfo(meshInfo);

    int num_points = meshInfo.vectexs_info.size();
    int num_triangles = meshInfo.poly_info.size();

    for(int i = 0; i < meshInfo.vectexs_info.size(); i++){

        tempMesh.addVertex(meshInfo.vectexs_info[i]);

    }


    for(int i = 0; i < meshInfo.poly_info.size(); i+=3){
        int v[3];

        v[0] = meshInfo.poly_info[i];
        v[1] = meshInfo.poly_info[i+1];
        v[2] = meshInfo.poly_info[i+2];

        tempMesh.addPolygon(3,v);
    }


    vector<HEmesh> hems = tempMesh.findDisconnectedMeshes();

    return hems;

}

vector<HEmesh> loadOFF(char *filename, bool color) {

    assert(filename);
    ifstream  ifs(filename);

    HEmesh hem;

    char ch;
    do {
        ifs.get(ch);
    } while (ch != '\n');

    int num_points = 0;
    int num_triangles = 0;
    int tmp = 0;

    ifs >> num_points >> num_triangles >> tmp;

    RawMeshInfo meshInfo;


    for(int i = 0; i < num_points; i++){
        Vertex p;
        ifs >> p.x >> p.y >> p.z;

        meshInfo.vectexs_info.push_back(p);

    }

    for(int i = 0; i < num_triangles; i++){
        int v[3];
        int junk = 0;
        if(!color) {
            ifs >> junk >> v[0] >> v[1] >> v[2];
        }
        else {
            ifs >> junk >> v[0] >> v[1] >> v[2] >> junk >> junk >> junk;
        }

        meshInfo.poly_info.push_back(v[0]);
        meshInfo.poly_info.push_back(v[1]);
        meshInfo.poly_info.push_back(v[2]);

    }

    ifs.close();

    meshInfo = cleanUpRawMeshInfo(meshInfo);
    HEmesh tempMesh;

    num_points = meshInfo.vectexs_info.size();
    num_triangles = meshInfo.poly_info.size();

    for(int i = 0; i < meshInfo.vectexs_info.size(); i++){

        tempMesh.addVertex(meshInfo.vectexs_info[i]);

    }


    for(int i = 0; i < meshInfo.poly_info.size(); i+=3){
        int v[3];

        v[0] = meshInfo.poly_info[i];
        v[1] = meshInfo.poly_info[i+1];
        v[2] = meshInfo.poly_info[i+2];

        tempMesh.addPolygon(3,v);
    }

    vector<HEmesh> hems = tempMesh.findDisconnectedMeshes();

    return hems;
}

void EdgeBreakerCompress::PrepMesh(){
    vector<int> boundary_vert;
    vector<int> loop_xtents;


    int num_loops = hem.getBoundaries(boundary_vert,loop_xtents);
    cout << "Mesh has " << num_loops << " loop(s)." << endl;

    for (int i = 0; i < num_loops; i++) {
        cout << "loop " << i << ":";
        for(int j = loop_xtents[2*i]; j<loop_xtents[2*i+1];j++){
            cout << " " << boundary_vert[j];
        }
        cout << endl;
    }

    if(num_loops>0){
        int dummy_points= 0;
        int dummy_triangles= 0;
        dummy_triangles = hem.triangulateLoops(boundary_vert,loop_xtents,dummyVertices);
        dummy_points = dummyVertices.size();
        cout << "Inserted " << dummy_points << " dummy points and "
             << dummy_triangles << " dummy triangles." << endl;
    }

    cout << "No mem problem" << endl;

    updateOVTable();
}

void EdgeBreakerCompress::loadOVTTable(char *filename) {

    int i = 0;
    FILE*  pInFile = NULL;

    int nNumberOfTriangles = 0;
    int nNumberOfVertices  = 0;

    pInFile = fopen(filename,"rt");
    if(pInFile == NULL){
        PrintErrorAndQuit((char*)"Can not open input file\n");
    }

    if (fscanf(pInFile, "%d", &nNumberOfTriangles) == -1) {
        PrintErrorAndQuit((char *) "Error reading file\n");
    }

    V = new int[nNumberOfTriangles * 3];
    O = new int[nNumberOfVertices * 3];

    for(int i = 0; i < nNumberOfTriangles; i++){
        if (fscanf(pInFile, "%d %d", &(V[i*3]), &(O[i*3]))==-1) {
            PrintErrorAndQuit((char *) "Error reading file\n");
        }
        if (fscanf(pInFile, "%d %d", &(V[i*3+1]), &(O[i*3+1])) == -1) {
            PrintErrorAndQuit((char *) "Error reading file\n");
        }
        if (fscanf(pInFile, "%d %d", &(V[i*3+2]), &(O[i*3+2])) == -1) {
            PrintErrorAndQuit((char *) "Error reading file\n");
        }
    }

    if (fscanf(pInFile, "%d", &nNumberOfVertices) == -1) {
        PrintErrorAndQuit((char *) "Error reading file\n");
    }

    G = new Vertex[nNumberOfVertices];
    G_est = new Vertex[nNumberOfVertices];

    for (int i = 0; i < nNumberOfVertices; ++i) {
        if (fscanf(pInFile, "%f %f %f", &(G[i].x), &(G[i].y), &(G[i].z)) == -1) {
            PrintErrorAndQuit((char *) "Error reading file\n");
        }
    }

    M = new int [nNumberOfVertices];
    U = new int [nNumberOfTriangles];

    fclose(pInFile);
}

void EdgeBreakerCompress::updateOVTable() {
    int num_points = 0;
    int num_triangles = 0;

    hem.size(num_points, num_triangles);

    V = new int[num_triangles*3];
    O = new int[num_triangles*3];

    for(int i = 0; i < num_triangles; i++){
        vector<int> vtxs;

        hem.getVertices(i,vtxs);

        int t[3], o[3];

        hem.oppositeTrisVerts(i,t,o);
        V[3*i]   = vtxs[0]; O[3*i]   = 3*t[0] + o[0];
        V[3*i+1] = vtxs[1]; O[3*i+1] = 3*t[1] + o[1];
        V[3*i+2] = vtxs[2]; O[3*i+2] = 3*t[2] + o[2];
    }

    G = new Vertex[num_points];
    G_est = new Vertex[num_points];

    for (int i = 0; i < num_points; ++i) {
        Vertex  p = hem.vertexlist[i].pt;
        G[i].x = p.x;
        G[i].y = p.y;
        G[i].z = p.z;
    }

    M = new int[num_points];
    U = new int[num_triangles];

    for (int i = 0; i<num_points; i++) {
        M[i] = 0;
    }

    for (int i = 0; i<num_triangles; i++) {
        U[i] = 0;
    }
}

void EdgeBreakerCompress::cleanupOFF(){
    hem.vertexlist.erase(hem.vertexlist.begin(), hem.vertexlist.end());
    hem.edgelist.erase(hem.edgelist.begin(), hem.edgelist.end());
    hem.polylist.erase(hem.polylist.begin(), hem.polylist.end());
    hem.clearTable();
}