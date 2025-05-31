//
// Created by matic on 30/03/2025.
//

#include "EdgeBreakerCompress.h"

#include <assert.h>
#include "EdgebreakerCompress.h"

EdgeBreakerCompress::EdgeBreakerCompress()
{
    // initialize the data member of EB
    eMeshType = MANIFOLD;
    eFileFormat = ASKII;
    nStartCorner = 0;
    O = NULL;
    V = NULL;
    G = NULL;
    G_est = NULL;
    T = 0;
    N = 0;
    M = NULL;
    U = NULL;
}

EdgeBreakerCompress::~EdgeBreakerCompress()
{
    // disallocate memory
    if (O != NULL)
        delete[] O;
    if (V != NULL)
        delete[] V;
    if (G != NULL)
        delete[] G;
    if (U != NULL)
        delete[] U;
    if (M != NULL)
        delete[] M;
}

void EdgeBreakerCompress::compress()
{
    // Compress Mesh
    initCompression(nStartCorner, eMeshType);
}
