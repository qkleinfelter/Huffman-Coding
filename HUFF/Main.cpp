#include "Huffman.h"
#include <iostream>


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
    string flag = argv[1];
    if ((argc == 3 || argc == 4) && flag == "-e")
    {
        if (argc == 3)
        {
            huffman->EncodeFile(argv[2], "");
        }
        else
        {
            huffman->EncodeFile(argv[2], argv[3]);
        }
    }
}
