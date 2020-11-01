#include "brainstorm.h"

using namespace std;

int main(int argc, char* argv[]) {

    if(argc < 2) {
        cout << "No input file" << endl;
        return EXIT_FAILURE;
    }

    BrainstormInterpreter<unsigned char> bs;

    if(bs.loadFile(argv[1]) == EXIT_FAILURE) {
        cout << "Can not load " << argv[1] << endl;
        return EXIT_FAILURE;
    }

    if(bs.compile() == EXIT_FAILURE) {
        cout << "Can not compile " << argv[1] << endl;
        return EXIT_FAILURE;
    }

    bs.allocMem(30000);
    bs.run();

    return EXIT_SUCCESS;
}
