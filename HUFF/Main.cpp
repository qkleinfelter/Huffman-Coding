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
    if (argc == 2 && (flag == "-?" || flag == "-h" || flag == "-help"))
    {
        huffman->DisplayHelp();
    }
    if ((argc == 3 || argc == 4) && flag == "-t")
    {
        if (argc == 3)
        {
            huffman->MakeTreeBuilder(argv[2], "");
        }
        else
        {
            huffman->MakeTreeBuilder(argv[2], argv[3]);
        }
    }
    if (argc == 4 && flag == "-d")
    {
        huffman->DecodeFile(argv[2], argv[3]);
    }
    delete huffman;
}
