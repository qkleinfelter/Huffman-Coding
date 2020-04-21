/*
    Quinn Kleinfelter
    EECS 2510-001 Non Linear Data Structures Spring 2020
    Dr. Thomas
    Last Edited: 4/21/20

    Main class to run through our Huffman work based on 
    command line parameters passed to our exe
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
    if (flag == "-e")
    {
        if (argc == 3)
        {
            // If we have 3 args and our flag is -e, encode the file with an empty outputFile string
            huffman->EncodeFile(argv[2], "");
        }
        else if (argc == 4)
        {
            // If we have 4 args and our flag is -e, encode the file 
            huffman->EncodeFile(argv[2], argv[3]);
        }
        else if (argc < 3)
        {
            cout << "Invalid command: too few arguments to run an encode" << endl;
            exit(0);
        }
        else
        {
            cout << "Invalid command: too many arguments to run an encode" << endl;
            exit(0);
        }
    }
    else if ((flag == "-?" || flag == "-h" || flag == "-help"))
    {
        // If we have 2 args and the flag is -? -h or -help simply display help
        huffman->DisplayHelp();
    }
    else if (flag == "-t")
    {
        if (argc == 3)
        {
            // If we have 3 args and our flag is -t make a tree builder with an empty outputFile
            huffman->MakeTreeBuilder(argv[2], "");
        }
        else if (argc == 4)
        {
            // If we have 4 args and our flag is -t make a tree builder
            huffman->MakeTreeBuilder(argv[2], argv[3]);
        }
        else if (argc < 3)
        {
            cout << "Invalid command: too few arguments to build a tree" << endl;
            exit(0);
        }
        else
        {
            cout << "Invalid command: too many arguments to build a tree" << endl;
            exit(0);
        }
    }
    else if (flag == "-d")
    {
        if (argc == 4)
        {
            // If we have 4 args and the flag is -d, decode the file
            huffman->DecodeFile(argv[2], argv[3]);
        }
        else if (argc < 4)
        {
            cout << "Invalid command: too few arguments to decode a file" << endl;
            exit(0);
        }
        else
        {
            cout << "Invalid command: too many arguments to decode a file" << endl;
            exit(0);
        }

    }
    else if (flag == "-et")
    {
        if (argc == 4)
        {
            // If we have 4 args and the flag is -et, encode using the second file as the tree builder with an empty output
            huffman->EncodeFileWithTree(argv[2], argv[3], "");
        }
        else if (argc == 5)
        {
            // If we have 5 args and the flag is -et, encode using the second file as the tree builder
            huffman->EncodeFileWithTree(argv[2], argv[3], argv[4]);
        }
        else if (argc < 4)
        {
            cout << "Invalid command: too few arguments to encode with tree" << endl;
            exit(0);
        }
        else
        {
            cout << "Invalid command: too many arguments to encode with tree" << endl;
            exit(0);
        }
    }
    else
    {
        cout << "Invalid command: flag not recognized" << endl;
        exit(0);
    }
    delete huffman; // Delete our object once we're done
}
