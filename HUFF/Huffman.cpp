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
		outputFile = fileNameWithoutExtension + ".huf" ;
	}
	cout << inputFile << " will be encoded to " << outputFile << endl;

	ifstream inputStream;
	ofstream outputStream;
	inputStream.open(inputFile, ios::binary);
	outputStream.open(outputFile, ios::binary);
	if (inputStream.fail())
	{
		cout << "Input stream failed to open" << endl;
		return;
	}
	if (outputStream.fail())
	{
		cout << "Output stream failed to open" << endl;
		return;
	}
	if (inputStream.is_open())
	{
		cout << "Input stream is open" << endl;
	}
	if (outputStream.is_open())
	{
		cout << "Output stream is open" << endl;
	}
	for (int i = 0; i < numChars; i++)
	{
		frequencyTable[i] = 0;
	}
	char character;
	unsigned int size = 0;
	while (inputStream.get(character))
	{ 
		unsigned char realChar = (unsigned char)character;
		frequencyTable[realChar]++;
		size++;
	}
	cout << "Size: " << size << endl;
	for (int i = 0; i < numChars; i++)
	{
		node* newNode = new node();
		newNode->weight = frequencyTable[i];
		newNode->symbol = (char)i;
		newNode->left = newNode->right = nullptr;
		nodes[i] = newNode;
	}
	for (int i = 0; i < numChars - 1; i++)
	{
		int smallestNodeIndex = getSmallestNodeIndex(-1);
		int nextSmallestNodeIndex = getSmallestNodeIndex(smallestNodeIndex);
		node* smallestNode = nodes[smallestNodeIndex];
		node* nextSmallestNode = nodes[nextSmallestNodeIndex];
		node* parent = new node();
		parent->weight = smallestNode->weight + nextSmallestNode->weight;
		if (smallestNodeIndex < nextSmallestNodeIndex)
		{
			parent->left = smallestNode;
			parent->right = nextSmallestNode;
			nodes[nextSmallestNodeIndex] = nullptr;
			nodes[smallestNodeIndex] = parent;
			//cout << smallestNodeIndex << " " << nextSmallestNodeIndex << endl;
			outputStream.put((char)smallestNodeIndex);
			outputStream.put((char)nextSmallestNodeIndex);
		}
		else {
			parent->left = nextSmallestNode;
			parent->right = smallestNode; 
			nodes[smallestNodeIndex] = nullptr;
			nodes[nextSmallestNodeIndex] = parent;
			//cout << nextSmallestNodeIndex << " " << smallestNodeIndex << endl;
			outputStream.put((char)nextSmallestNodeIndex);
			outputStream.put((char)smallestNodeIndex);
		}
	}
	buildEncodingStrings(nodes[0], "");
	if (nodes[0]->weight != size)
	{
		cout << "Nodes[0] weight is " << nodes[0]->weight << ", while size (amt of chars read) is " << size << endl;
	}
	int fries;
	cin >> fries;
	inputStream.close();
}

int Huffman::getSmallestNodeIndex(int indexToSkip)
{
	int smallestWeight = INT16_MAX;
	int smallestIndex = -1;
	for (int i = 0; i < numChars; i++)
	{
		if (i == indexToSkip || nodes[i] == nullptr) continue;
		if (nodes[i]->weight < smallestWeight)
		{
			smallestIndex = i;
			smallestWeight = nodes[i]->weight;
		}
	}
	return smallestIndex;

}

void Huffman::buildEncodingStrings(node* startingPoint, string currentPath)
{
	if (startingPoint == nullptr) return;
	if (startingPoint->symbol)
	{
		// We arrived at a leaf
		encodingStrings[startingPoint->symbol] = currentPath;
	}
	if (startingPoint->left != nullptr)
	{
		buildEncodingStrings(startingPoint->left, currentPath += "0");
	}
	if (startingPoint->right != nullptr)
	{
		buildEncodingStrings(startingPoint->right, currentPath += "1");
	}
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
