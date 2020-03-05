#pragma once
#include <string>
#include <fstream>
using namespace std;

class Huffman
{
public:
	Huffman();
	~Huffman();
	void MakeTreeBuilder(string inputFile, string outputFile);
	void EncodeFile(string inputFile, string outputFile);
	void DecodeFile(string inputFile, string outputFile);
	void EncodeFileWithTree(string inputFile, string treeFile, string outputFile);
	void DisplayHelp();

private:
	struct node 
	{
		unsigned char symbol;
		unsigned int weight;
		node* left;
		node* right;
	};
	const static int numChars = 256;
	int frequencyTable[256];
	node* nodes[256];
	string encodingStrings[256];
	string paddingBits = "";
	ifstream inputStream;
	ofstream outputStream;

	void openFiles(string inputFile, string outputFile);
	void buildFrequencyTable();
	int getSmallestNodeIndex(int indexToSkip);
	void buildTree();
	void buildTreeFromFile();
	void buildEncodingStrings(node* startingPoint, string currentPath);
	void encode();
	void decode();
	void closeFiles();
};