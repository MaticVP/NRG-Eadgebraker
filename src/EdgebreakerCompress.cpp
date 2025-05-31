//
// Created by matic on 31/03/2025.
//

#include "EdgebreakerDecompress.h"

EdgebreakerDecompress::EdgebreakerDecompress()
{
    eMeshType = MANIFOLD;
    eFileFormat = ASKII;
    O = NULL;
    V = NULL;
    G = NULL;

    T = 0;
    N = 0;
    I = 0;
    A = 0;
    M = NULL;
    U = NULL;
}

EdgebreakerDecompress::~EdgebreakerDecompress()
{
    // deallocate memory
    if (O != NULL)
        delete [] O;
    if (V != NULL)
        delete [] V;
    if (G != NULL)
        delete [] G;
    if (U != NULL)
        delete [] U;
    if (M != NULL)
        delete [] M;
}

void EdgebreakerDecompress::uncompress()
{
    initDecompression();
    std::cout << "Mesh loaded"  << std::endl;
    cleanupDummy();
}