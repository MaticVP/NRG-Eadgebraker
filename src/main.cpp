#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include "EdgebreakerCompress.h"
#include "EdgebreakerDecompress.h"
#include <string.h>
#include <iostream>
#include <chrono>
using namespace std::chrono;
using namespace std;

void usage()
{
    fprintf(stderr, "Usage: {C | D} input.{ov | off} {A | B}");
    exit(1);
}


int main(int argc, char** argv) {
    //Compress

    if (argc == 6 | argc == 4) {
        string action = argv[1];
        if (action == "C") {
            auto start = high_resolution_clock::now();

            string st = argv[2];

            vector<HEmesh> hems;
            string color_str = argv[5];
            bool color = "C" == color_str;
            if(st.substr(st.find_last_of(".") + 1) == "obj") {
                hems = loadOBJ(argv[2], color);
            }
            else {
                hems = loadOFF(argv[2], color);
            }

            for (int i = 0; i < hems.size();i++){
                EdgeBreakerCompress ebc;
                ebc.hem = hems[i];
                ebc.PrepMesh();
                ebc.compress();

                if (!strcmp(argv[4], "B"))
                    ebc.saveBinary((char *) "output.beb");
                else if (!strcmp(argv[4], "A"))
                    ebc.saveAscii(argv[3],i);
                else
                    usage();
            }

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);
            std::cout << "Compression completed in " << duration.count() << " ms\n";

            return 0;
        }
        else if(action == "D"){
            auto start = high_resolution_clock::now();

            string st = argv[2];
            int num_mesh = countNumberOfMeshes(argv[2]);
            vector<HEmesh> hems;
            for(int i=0;i<=num_mesh;i++) {

                EdgebreakerDecompress ebdc;

                if (st.substr(st.find_last_of(".") + 1) == "beb")
                    ebdc.loadBinary(argv[2]);
                else
                    ebdc.loadAscii(argv[2],i);

                cout << "Mesh " << i << " loaded"  << endl;
                ebdc.uncompress();
                cout << "Mesh " << i << " compressed"  << endl;
                hems.push_back(ebdc.hem);

            }
            st = argv[3];
            if(st.substr(st.find_last_of(".") + 1) == "obj") {
                saveOBJ(argv[3],hems);
            }
            else {
                saveOFF(argv[3], hems);
            }

            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start);
            std::cout << "Decompression completed in " << duration.count() << " ms\n";

        }


        else {
            usage();
        }
    }

    return 0;
}
