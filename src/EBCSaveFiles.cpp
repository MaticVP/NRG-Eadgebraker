//
// Created by matic on 30/03/2025.
//

#include <assert.h>
#include "EdgebreakerCompress.h"
#include "EBUtils.h"
#include <stdio.h>
#include "EBLabelStream.h"
#include "EBBinStreamIO.h"
#include <algorithm>

void EdgeBreakerCompress::saveAscii(char* save_path, int i) {
    FILE* file_clers = NULL;
    FILE* file_vertices = NULL;
    FILE* file_handles = NULL;
    FILE* file_dummy = NULL;

    char path_clers[512];
    char path_vertices[512];
    char path_handles[512];
    char path_dummy[512];
    printf("%s\n", save_path);

    snprintf(path_clers, sizeof(path_clers), "%s/%d_clers.txt", save_path, i);
    snprintf(path_vertices, sizeof(path_vertices), "%s/%d_vertices.txt", save_path, i);
    snprintf(path_handles, sizeof(path_handles), "%s/%d_handles.txt", save_path, i);
    snprintf(path_dummy, sizeof(path_dummy), "%s/%d_dummy.txt", save_path, i);

    file_clers = fopen(path_clers, "w+t");
    if (file_clers == NULL)
        PrintErrorAndQuit((char*)"Can not open clers file\n");

    file_vertices = fopen(path_vertices, "w+t");
    if (file_vertices == NULL)
        PrintErrorAndQuit((char*)"Can not open vertices file\n");

    file_handles = fopen(path_handles, "w+t");
    if (file_handles == NULL)
        PrintErrorAndQuit((char*)"Can not open handles file\n");

    file_dummy = fopen(path_dummy, "w+t");
    if (file_dummy == NULL)
        PrintErrorAndQuit((char*)"Can not open handles file\n");

    //write mesh type to clers file
    if (eMeshType == MANIFOLD)
    {
        if (eFileFormat == ASKII)
            fprintf(file_clers, "%s\n", "MANIFOLD");
    }
    else if (eMeshType == TPATCH)
    {
        if (eFileFormat == ASKII)
            fprintf(file_clers, "%s\n", "TPATCH");
    }
    else
        PrintErrorAndQuit((char*)"Not supported mesh type\n");

    if (eFileFormat == ASKII)
        fprintf(file_clers, "%d\n", it1_count);
    int clers_count = clers.size();
    for (int i = 0; i < clers_count; i++) {
        fprintf(file_clers, "%c\n", clers[i]);
    }

    int handles_count = handles.size() / 2;
    for (int i = 0; i < handles_count; i++) {
        fprintf(file_handles, "%d %d\n", handles[2*i], handles[2*i+1]);
    }

    int vtx_count = vertices.size();
    for (int i = 0; i < vtx_count; i++) {
        fprintf(file_vertices, "%f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
    }

    int dummy_count = newDummy.size();
    for (int i = 0; i < dummy_count; i++) {
        fprintf(file_dummy, "%d\n", newDummy[i]);
    }

    fclose(file_clers);
    fclose(file_vertices);
    fclose(file_handles);
    fclose(file_dummy);
}

void EdgeBreakerCompress::saveBinary(char *filename) {
    assert(filename);
    EBBinStreamIO bsio;

    int fd = bsio.openFileWrite(filename);

    unsigned char type = (unsigned char) 0;
    bsio.writeUCharArray(fd, &type, 1);
    bsio.writeIntArray(fd, &it1_count, 1);

    int numclers = clers.size();
    bio_buffer buffer;
    EBLabelStream ebls;
    ebls.encodeStream(clers, buffer);
    int buff_size = buffer._buffer.size();
    bsio.writeIntArray(fd, &numclers, 1);
    bsio.writeIntArray(fd, &buff_size, 1);
    unsigned char *ucbuff = (unsigned char*) new unsigned char[buff_size];
    copy(buffer._buffer.begin(), buffer._buffer.end(), ucbuff);
    bsio.writeUCharArray(fd, ucbuff, buff_size);
    delete[] ucbuff;

    int numhandles = handles.size() / 2;		// handles
    bsio.writeIntArray(fd, &numhandles, 1);
    if (numhandles > 0) {
        int* hands = (int*) new int[2*numhandles];
        copy(handles.begin(), handles.end(), hands);
        bsio.writeIntArray(fd, hands, 2*numhandles);
        delete[] hands;
    }

    int numvert = vertices.size();
    bsio.writeIntArray(fd, &numvert, 1);
    float* coords = (float*) new float[3*numvert];
    for (int i = 0; i < numvert; i++) {
        coords[3*i]     = vertices[i].x;
        coords[3*i + 1] = vertices[i].y;
        coords[3*i + 2] = vertices[i].z;
    }
    bsio.writeFloatArray(fd, coords, 3*numvert);
    delete[] coords;

    int numdummy = newDummy.size();
    bsio.writeIntArray(fd, &numdummy, 1);
    if (numdummy > 0) {
        int* dum = (int*) new int[numdummy];
        copy(newDummy.begin(), newDummy.end(), dum);
        bsio.writeIntArray(fd, dum, numdummy);
        delete[] dum;
    }

    bsio.close(fd);
}
