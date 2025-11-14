#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::fstream hashes("hashes.txt");
    hashes << "peepee\n";

    return 0;
}
