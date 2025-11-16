#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::fstream myfile;
    myfile.open("test.txt", std::ios::out | std::ios::in);
    myfile.close();
    return 0;
}
