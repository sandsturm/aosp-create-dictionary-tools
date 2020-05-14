#include "dictionary.h"

int main(int argc, const char *argv[]) {
    // Exit if no filename provided
    if (!argv[1]){
      std::cout << "Please provide a correct path and filename." << '\n';
      exit(0);
    }



    Dictionary dictionary;
    dictionary.run(argv[1]);

    return 0;
}
