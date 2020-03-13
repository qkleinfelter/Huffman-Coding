/*
    Quinn Kleinfelter
    EECS 2510-001 Non Linear Data Structures Spring 2020
    Dr. Thomas
    Last Edited: 3/13/20
*/

#include "Huffman.h"
#include <iostream>


int main(int argc, char* argv[])
{
    // Instantiate a huffman object to work with
    Huffman* huffman = new Huffman();
    if (argc < 2)
    {
        // If we have less than 2 arguments, then we must only have the name of the program
        // so we print an error, help message, delete our object and return
        cout << "No arguments provided" << endl;
        huffman->DisplayHelp();
        delete huffman;
        return 0;
    }
    string flag = argv[1]; // Create a string variable to hold the contents of argv[1] so we can compare it to strings
    if ((argc == 3 || argc == 4) && flag == "-e")
    {
        if (argc == 3)
        {
            // If we have 3 args and our flag is -e, encode the file with an empty outputFile string
            huffman->EncodeFile(argv[2], "");
        }
        else
        {
            // If we have 4 args and our flag is -e, encode the file 
            huffman->EncodeFile(argv[2], argv[3]);
        }
    }
    if (argc == 2 && (flag == "-?" || flag == "-h" || flag == "-help"))
    {
        // If we have 2 args and the flag is -? -h or -help simply display help
        huffman->DisplayHelp();
    }
    if ((argc == 3 || argc == 4) && flag == "-t")
    {
        if (argc == 3)
        {
            // If we have 3 args and our flag is -t make a tree builder with an empty outputFile
            huffman->MakeTreeBuilder(argv[2], "");
        }
        else
        {
            // If we have 4 args and our flag is -t make a tree builder
            huffman->MakeTreeBuilder(argv[2], argv[3]);
        }
    }
    if (argc == 4 && flag == "-d")
    {
        // If we have 4 args and the flag is -d, decode the file
        huffman->DecodeFile(argv[2], argv[3]);
    }
    if ((argc == 4 || argc == 5) && flag == "-et")
    {
        if (argc == 4)
        {
            // If we have 4 args and the flag is -et, encode using the second file as the tree builder with an empty output
            huffman->EncodeFileWithTree(argv[2], argv[3], "");
        }
        else
        {
            // If we have 5 args and the flag is -et, encode using the second file as the tree builder
            huffman->EncodeFileWithTree(argv[2], argv[3], argv[4]);
        }
    }
    delete huffman; // Delete our object once we're done
}
