#pragma once
#include <string>
#include <fstream>
#include <time.h>
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
	unsigned int bytesIn = 0;
	unsigned int bytesOut = 0;
	clock_t start = clock();

	void openFiles(string inputFile, string outputFile);
	void buildFrequencyTable();
	int getSmallestNodeIndex(int indexToSkip);
	void buildTree();
	void buildTreeFromFile();
	void buildEncodingStrings(node* startingPoint, string currentPath);
	void encode();
	void decode();
	void followTree(unsigned char byte, int checkBit, node*& currentNode);
	void closeFiles();
	void deleteSubtree(node* startingNode);

	void printActionDetail();
	string formatNumber(unsigned int num);
};