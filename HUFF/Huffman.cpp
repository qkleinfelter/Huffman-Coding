#include "Huffman.h"
#include <iostream>

Huffman::Huffman() : nodes{ nullptr }, frequencyTable { 0 }
{

}

Huffman::~Huffman()
{
	for (int i = 0; i < numChars; i++)
	{
		if (nodes[i] != nullptr)
			deleteSubtree(nodes[i]);
	}
}

void Huffman::deleteSubtree(node* startingNode)
{
	if (startingNode->left != nullptr)
	{
		deleteSubtree(startingNode->left);
	}
	if (startingNode->right != nullptr)
	{
		deleteSubtree(startingNode->right);
	}
	delete startingNode;
}

void Huffman::MakeTreeBuilder(string inputFile, string outputFile)
{
	if (inputFile == outputFile)
	{
		cout << "Input File can not be equal to Output File" << endl;
		DisplayHelp();
		return;
	}
	if (outputFile == "")
	{
		auto dotLoc = inputFile.find(".");
		if (dotLoc == string::npos)
		{
			outputFile += ".htree";
		}
		else
		{
			string fileNameWithoutExtension = inputFile.substr(0, dotLoc);
			outputFile = fileNameWithoutExtension + ".htree";
		}
	}
	cout << "A tree will be created from " << inputFile << " and placed into " << outputFile << endl;
	openFiles(inputFile, outputFile);
	buildFrequencyTable();
	buildTree();
	closeFiles();
}

void Huffman::EncodeFile(string inputFile, string outputFile)
{
	if (inputFile == outputFile)
	{
		cout << "Input File can not be equal to Output File" << endl;
		DisplayHelp();
		return;
	}
	if (outputFile == "")
	{
		auto dotLoc = inputFile.find(".");
		string fileNameWithoutExtension = inputFile.substr(0, dotLoc);
		outputFile = fileNameWithoutExtension + ".huf" ;
	}
	cout << inputFile << " will be encoded to " << outputFile << endl;
	openFiles(inputFile, outputFile);
	buildFrequencyTable();
	buildTree();
	buildEncodingStrings(nodes[0], "");
	encode();
}

int Huffman::getSmallestNodeIndex(int indexToSkip)
{
	unsigned int smallestWeight = INT32_MAX;
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
		if (currentPath.length() > 7 && paddingBits.length() != 0)
			paddingBits = currentPath;
		//cout << startingPoint->symbol << ": " << currentPath << endl;
	}
	if (startingPoint->left != nullptr)
	{
		buildEncodingStrings(startingPoint->left, currentPath + "0");
	}
	if (startingPoint->right != nullptr)
	{
		buildEncodingStrings(startingPoint->right, currentPath + "1");
	}
}

void Huffman::DecodeFile(string inputFile, string outputFile)
{
	if (inputFile == outputFile)
	{
		cout << "Input File can not be equal to Output File" << endl;
		DisplayHelp();
		return;
	}
	openFiles(inputFile, outputFile);
	buildTreeFromFile();
	decode();
	closeFiles();
}

void Huffman::EncodeFileWithTree(string inputFile, string treeFile, string outputFile)
{
}

void Huffman::DisplayHelp()
{
	cout << "A help message" << endl;
}

void Huffman::buildFrequencyTable()
{
	for (int i = 0; i < numChars; i++)
	{
		frequencyTable[i] = 0;
	}
	char character;
	while (inputStream.get(character))
	{
		unsigned char realChar = (unsigned char)character;
		frequencyTable[realChar]++;
		bytesIn++;
	}
	cout << "Size: " << bytesIn << endl;
}

void Huffman::openFiles(string inputFile, string outputFile)
{
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
}

void Huffman::buildTree()
{
	for (int i = 0; i < numChars; i++)
	{
		node* newNode = new node();
		newNode->weight = frequencyTable[i];
		newNode->symbol = (unsigned char)i;
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
			bytesOut += 2;
		}
		else {
			parent->left = nextSmallestNode;
			parent->right = smallestNode;
			nodes[smallestNodeIndex] = nullptr;
			nodes[nextSmallestNodeIndex] = parent;
			//cout << nextSmallestNodeIndex << " " << smallestNodeIndex << endl;
			outputStream.put((char)nextSmallestNodeIndex);
			outputStream.put((char)smallestNodeIndex);
			bytesOut += 2;
		}
	}
}

void Huffman::encode()
{
	inputStream.clear();
	inputStream.seekg(0);
	char character;
	string buffer = "";
	while (inputStream.get(character))
	{
		unsigned char realChar = character;
		buffer += encodingStrings[realChar];
		if (buffer.length() >= 8)
		{
			// Do some bitwise shit
			unsigned char byte = 0;
			for (int i = 0; i <= 7; i++)
			{
				bool on = buffer[i] == '1';
				byte |= (on << (7 - i));
			}
			outputStream.put(byte);
			bytesOut++;
			//cout << "output " << byte << " to file" << endl;
			buffer = buffer.substr(8);
		}
	}
	if (buffer.length() < 8)
	{
		buffer += paddingBits;
		unsigned char byte = 0;
		for (int i = 0; i <= 7; i++)
		{
			bool on = buffer[i] == '1';
			byte |= (on << (7 - i));
		}
		outputStream.put(byte);
		bytesOut++;
		//cout << "output " << byte << " to file" << endl;
	}
}

void Huffman::buildTreeFromFile()
{
	for (int i = 0; i < numChars; i++)
	{
		node* newNode = new node();
		newNode->weight = 0;
		newNode->symbol = (unsigned char)i;
		newNode->left = newNode->right = nullptr;
		nodes[i] = newNode;
	}
	for (int i = 0; i < numChars - 1; i++)
	{
		char character1, character2;
		inputStream.get(character1);
		inputStream.get(character2);
		bytesIn += 2;
		unsigned char char1 = character1;
		unsigned char char2 = character2;
		node* parent = new node();
		parent->weight = 0;
		parent->symbol = NULL;
		parent->left = nodes[char1];
		parent->right = nodes[char2];
		nodes[char2] = nullptr;
		nodes[char1] = parent;
	}
}

void Huffman::decode()
{
	char character;
	node* currentNode = nodes[0];
	while (inputStream.get(character))
	{
		bytesIn++;
		unsigned char byte = character;
		followTree(byte, 128, currentNode);
		followTree(byte, 64, currentNode);
		followTree(byte, 32, currentNode);
		followTree(byte, 16, currentNode);
		followTree(byte, 8, currentNode);
		followTree(byte, 4, currentNode);
		followTree(byte, 2, currentNode);
		followTree(byte, 1, currentNode);
	}
}

void Huffman::followTree(unsigned char byte, int checkBit, node*& currentNode)
{
	currentNode = byte & checkBit ? currentNode->right : currentNode->left;
	if (currentNode->left == nullptr && currentNode->right == nullptr)
	{
		outputStream.put(currentNode->symbol);
		//cout << "printed " << currentNode->symbol << " to file" << endl;
		bytesOut++;
		currentNode = nodes[0];
	}
}

void Huffman::closeFiles()
{
	if (inputStream.is_open())
		inputStream.close();
	if (outputStream.is_open())
		outputStream.close();
}