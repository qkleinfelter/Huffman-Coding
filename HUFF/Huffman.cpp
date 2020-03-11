/*
	File: Huffman.cpp - Implementation of Huffman Encoding using a tree
	c.f.: Huffman.h

	This class implements a Huffman encoding of a given file using a tree.
	Each node contains a symbol, the number of times the symbol appears in the input, 
	and a pointer to its left and right children.

	Author: Quinn Kleinfelter
	Class: EECS 2510-001 Non Linear Data Structures Spring 2020
	Instructor: Dr. Thomas
	Last Edited: 3/11/20
	Copyright: Copyright 2020 by Quinn Kleinfelter. All rights reserved.
*/


#include "Huffman.h"
#include <iostream>
#include <time.h>

Huffman::Huffman() : nodes{ nullptr }, frequencyTable { 0 }
{
	// Constructor, not much to do here except start out our clock and make sure our
	// arrays are initialized to nullptr and 0, as seen above, to ensure we don't get
	// warnings from Visual Studio
	start = clock();
}

Huffman::~Huffman()
{
	// Destructor, all we need to do here is loop through all of our nodes and if the
	// current spot isn't null delete it and its subtrees
	for (int i = 0; i < numChars; i++)
	{
		if (nodes[i] != nullptr)
			deleteSubtree(nodes[i]);
	}
}

void Huffman::deleteSubtree(node* startingNode)
{
	// Helper method that deletes the subtrees of a node
	if (startingNode->left != nullptr)
	{
		// If the left child isn't null, recursively delete its subtrees
		deleteSubtree(startingNode->left);
	}
	if (startingNode->right != nullptr)
	{
		// If the right child isn't null, recursively delete its subtrees
		deleteSubtree(startingNode->right);
	}
	delete startingNode; // Once we've deleted its children it is safe to delete the start
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
	//cout << "A tree will be created from " << inputFile << " and placed into " << outputFile << endl;
	openFiles(inputFile, outputFile, "");
	buildFrequencyTable();
	buildTree();
	closeFiles();
	printActionDetail();
}

void Huffman::EncodeFile(string inputFile, string outputFile)
{
	// Encodes the inputFile into the outputFile
	if (inputFile == outputFile)
	{
		// If the input file and output file refer to the same thing display an error and exit out
		cout << "Input File can not be equal to Output File" << endl;
		return;
	}
	if (outputFile == "")
	{
		// If the output file isn't specified, we need to determine it from the inputfile
		auto dotLoc = inputFile.find("."); // Check if a . exists in the inputFile name
		if (dotLoc == string::npos)
		{
			// If there isn't a . in the inputFile name, simply append .huf to the inputFile and make it the output
			outputFile = inputFile + ".huf";
		}
		else
		{
			// Otherwise, since a . exists, grab the subtring before that dot
			string fileNameWithoutExtension = inputFile.substr(0, dotLoc);
			outputFile = fileNameWithoutExtension + ".huf"; // And make the output file equal to the inputFile but with its extension changed to .huf
		}
	}
	openFiles(inputFile, outputFile, ""); // Open up our files into our streams, treeFile is not needed so we don't use it
	buildFrequencyTable(); // Build the frequency table from our input file
	buildTree(); // Build the tree based on our frequency table
	buildEncodingStrings(nodes[0], ""); // Build our list of encoding strings based on the tree
	encode(); // Actually encode the file
	printActionDetail(); // Print out the runtime / space information
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
		if (currentPath.length() > 7)
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
		return;
	}
	openFiles(inputFile, outputFile, "");
	buildTreeFromFile(inputStream);
	decode();
	closeFiles();
	printActionDetail();
}

void Huffman::EncodeFileWithTree(string inputFile, string treeFile, string outputFile)
{
	if (inputFile == outputFile)
	{
		cout << "Input File can not be equal to Output File" << endl;
		return;
	}
	if (outputFile == "")
	{
		auto dotLoc = inputFile.find(".");
		if (dotLoc == string::npos)
		{
			outputFile += ".huf";
		}
		else
		{
			string fileNameWithoutExtension = inputFile.substr(0, dotLoc);
			outputFile = fileNameWithoutExtension + ".huf";
		}
	}
	openFiles(inputFile, outputFile, treeFile);
	buildTreeFromFile(treeStream);
	buildEncodingStrings(nodes[0], "");
	encode();
	closeFiles();
	printActionDetail();
}

void Huffman::DisplayHelp()
{
	cout << "A help message" << endl;
}

void Huffman::buildFrequencyTable()
{
	// Helper method to build out our frequency table
	char character; // Variable to store the next character from the file
	while (inputStream.get(character)) // inputStream.get(char) returns true if it was able to place the next char from the file into its parameter, so we can just loop on that
	{
		unsigned char realChar = (unsigned char)character; // Once we know we have read a character we want to cast it into an unsigned char so that we don't have array access errors
		frequencyTable[realChar]++; // Increment the frequencyTable in the appropriate location (an unsigned char will automatically cast into its int ASCII value for array access)
	}
}

void Huffman::openFiles(string inputFile, string outputFile, string treeFile)
{
	// Helper method to open up the given files
	inputStream.open(inputFile, ios::binary); // We ALWAYS want to open up an inputFile, in binary mode
	outputStream.open(outputFile, ios::binary); // We ALWAYS want to open up an outputFile, in binary mode
	if (treeFile.length() > 0) // If our treeFile string has a length greater than 0, we must want it so try to open it
	{
		treeStream.open(treeFile, ios::binary); // Open up the treeFile, in binary mode
		if (treeStream.fail()) // If we fail to open, display an error and return
		{
			cout << "Tree stream failed to open" << endl;
			return;
		}
	}
	if (inputStream.fail()) // If we failed to open the inputFile display an error and return
	{
		cout << "Input stream failed to open" << endl;
		return;
	}
	if (outputStream.fail()) // If we failed to open the outputFile display an error and return
	{
		cout << "Output stream failed to open" << endl;
		return;
	}
}

void Huffman::buildTree()
{
	// Helper method to build our tree from the frequency table
	for (int i = 0; i < numChars; i++)
	{
		// Loop through all of the characters in the frequency table,
		// building a node for each one with its given frequency
		node* newNode = new node(); // Create a new node
		newNode->weight = frequencyTable[i]; // Set the weight of the node equal to its value from the frequency table
		newNode->symbol = (unsigned char)i; // Make the symbol of the node equal to its int value casted to an unsigned char
		newNode->left = newNode->right = nullptr; // Make the left and right children nullptr (redundant)
		nodes[i] = newNode; // Set our current location in the nodes array to be the node we just made
	}
	for (int i = 0; i < numChars - 1; i++)
	{
		// Loop through the number of characters we have - 1, this is the amount we always need to build a tree
		int smallestNodeIndex = getSmallestNodeIndex(-1); // Grab the index of the smallest node in our nodes array, without skipping any indexes
		int nextSmallestNodeIndex = getSmallestNodeIndex(smallestNodeIndex); // Grab the index of the next smallest node in our nodes array, making sure we skip the smallest index
		node* smallestNode = nodes[smallestNodeIndex]; // Grab the node that is the smallest
		node* nextSmallestNode = nodes[nextSmallestNodeIndex]; // Grab the node that is the next smallest
		node* parent = new node(); // Make a new parent node to hold these nodes
		parent->weight = smallestNode->weight + nextSmallestNode->weight; // Set the weight of the parent to be the weights of the nodes added together
		if (smallestNodeIndex < nextSmallestNodeIndex) // If smallestNode occurs earlier in the list, we make it the left child
		{
			parent->left = smallestNode; // Make our parents left child smallestNode
			parent->right = nextSmallestNode; // Make our parents right child nextSmallestNode
			nodes[nextSmallestNodeIndex] = nullptr; // Set the location in nodes[] where nextSmallestNode used to be equal to nullptr
			nodes[smallestNodeIndex] = parent; // Set the location in nodes[] where smallestNode used to be equal to the parent
			outputStream.put((char)smallestNodeIndex); // Output the smallestNodeIndex casted to a char, so others can build the tree as needed
			outputStream.put((char)nextSmallestNodeIndex); // Output the nextSmallestNodeIndex casted to a char, so others can build the tree as needed
			bytesOut += 2; // Increase our bytesOut by 2 since we printed 2 to the file
		}
		else // If we get here, we know nextSmallestNode occurs earlier in the list, so it should be our left child
		{
			parent->left = nextSmallestNode; // Make our parents left child nextSmallestNode
			parent->right = smallestNode; // Make our parents right child smallestNode
			nodes[smallestNodeIndex] = nullptr; // Set the location in nodes[] where smallestNode used to be equal to nullptr
			nodes[nextSmallestNodeIndex] = parent; // Set the location in nodes[] where nextSmallestNode used to be equal to the parent
			outputStream.put((char)nextSmallestNodeIndex); // Output the nextSmallestNodeIndex casted to a char, so others can build the tree as needed
			outputStream.put((char)smallestNodeIndex); // Output the SmallestNodeIndex casted to a char, so others can build the tree as needed
			bytesOut += 2; // Increase our bytesOut by 2 since we printed 2 to the file
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
		bytesIn++;
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

void Huffman::buildTreeFromFile(ifstream& file)
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
		file.get(character1);
		file.get(character2);
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
	if (treeStream.is_open())
		treeStream.close();
}

void Huffman::printActionDetail()
{
	clock_t end = clock();
	double secondsElapsed = difftime(end, start) / 1000;
	cout << "Time: " << secondsElapsed << " seconds.   ";
	cout << "Bytes in / Bytes Out: " << formatNumber(bytesIn) << " / " << formatNumber(bytesOut) << endl;
}

string Huffman::formatNumber(unsigned int num)
{
	string number = to_string(num);
	for (int i = number.length() - 3; i > 0; i -= 3)
	{
		number = number.substr(0, i) + "," + number.substr(i);
	}
	return number;
}