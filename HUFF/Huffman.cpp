#include "Huffman.h"
#include <iostream>
#include <fstream>

Huffman::Huffman()
{

}

Huffman::~Huffman()
{

}

void Huffman::MakeTreeBuilder(string inputFile, string outputFile)
{
}

void Huffman::EncodeFile(string inputFile, string outputFile)
{
	if (outputFile == "")
	{
		int dotLoc = inputFile.find(".");
		string fileNameWithoutExtension = inputFile.substr(0, dotLoc);
		outputFile = fileNameWithoutExtension + ".huf";
	}
	cout << inputFile << " will be encoded to " << outputFile << endl;

	ifstream inputStream;
	inputStream.open(inputFile);
	if (inputStream.fail())
	{
		cout << "Input stream failed to open" << endl;
		return;
	}
	if (inputStream.is_open())
	{
		cout << "Input stream is open" << endl;
	}
	for (int i = 0; i < 256; i++)
	{
		frequencyTable[i] = 0;
	}
	char character;
	while (inputStream.get(character))
	{ 
		frequencyTable[character]++;
	}
	for (int i = 0; i < 256; i++)
	{
		node* newNode = new node();
		newNode->weight = frequencyTable[i];
		newNode->symbol = (char)i;
		newNode->left = newNode->right = nullptr;
		nodes[i] = newNode;
	}
	int fries;
	cin >> fries;
	inputStream.close();
}

void Huffman::DecodeFile(string inputFile, string outputFile)
{

}

void Huffman::EncodeFileWithTree(string inputFile, string treeFile, string outputFile)
{
}

void Huffman::DisplayHelp()
{
	cout << "A help message" << endl;
}
