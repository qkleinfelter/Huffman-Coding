#include "Huffman.h"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    Huffman* huffman = new Huffman();
    if (argc < 2)
    {
        cout << "No arguments provided" << endl;
        huffman->DisplayHelp();
        delete huffman;
        return 0;
    }
}
