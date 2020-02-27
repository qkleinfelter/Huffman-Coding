#pragma once
#include <string>
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
		int weight;
		node* left;
		node* right;
	};
	int frequencyTable[256];
	node* nodes[256];
};