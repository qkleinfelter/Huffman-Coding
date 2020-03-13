/*
	File: Huffman.cpp - Implementation of Huffman Encoding using a tree
	c.f.: Huffman.h

	This class implements a Huffman encoding of a given file using a tree.
	Each node contains a symbol, the number of times the symbol appears in the input, 
	and a pointer to its left and right children.

	Author: Quinn Kleinfelter
	Class: EECS 2510-001 Non Linear Data Structures Spring 2020
	Instructor: Dr. Thomas
	Last Edited: 3/13/20
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
	// This method constructs tree building information based on an
	// inputFile and then places it into outputFile. This implements 
	// our -t command line parameter, and can be used for future -et runs.
	if (inputFile == outputFile)
	{
		// The input can't also be the output, so we display an error and exit
		cout << "Input File can not be equal to Output File" << endl;
		return;
	}
	if (outputFile == "")
	{
		// If we don't have an output file, we want to figure it out based on our input
		// First we look for a period
		auto dotLoc = inputFile.find(".");
		if (dotLoc == string::npos)
		{
			// If there isn't a period (meaning no extension), we simply append
			// .htree to the inputFile name for our output
			outputFile = inputFile + ".htree";
		}
		else
		{
			// Otherwise, we want to remove the extension and replace it with .htree
			string fileNameWithoutExtension = inputFile.substr(0, dotLoc);
			outputFile = fileNameWithoutExtension + ".htree";
		}
	}
	openFiles(inputFile, outputFile, ""); // Open up our input and output streams, we don't need a tree stream for this
	buildFrequencyTable(); // Build out a frequency table from our input file
	buildTree(); // Build the tree based on that frequency table
	closeFiles(); // Close out the files since that's all we want to do!
	printActionDetail(); // Print out information about what we did!
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
	// This is a helper function that loops through our nodes array
	// to find the node that has the smallest weight and return its index.
	// We include an index we want to skip so we can run this method twice and
	// not get the same index both times
	unsigned int smallestWeight = INT32_MAX; // Initialize a smallest weight to be the maximum number our unsigned int can hold
	int smallestIndex = -1; // Set up a variable to hold our smallest index, which by default is -1
	for (int i = 0; i < numChars; i++)
	{
		// Loop through the array
		if (i == indexToSkip || nodes[i] == nullptr) continue; // If we are at the index we want to skip, or the current node is null, continue on to the next iteration
		if (nodes[i]->weight < smallestWeight) // If the weight of our current node is smaller than the smallest weight we have seen yet
		{
			smallestIndex = i; // Set the smallest index to our current index
			smallestWeight = nodes[i]->weight; // Set the smallest weight to our current weight
		}
	}
	return smallestIndex; // Once we've looped through the whole thing, return the index of the smallest weight node

}

void Huffman::buildEncodingStrings(node* startingPoint, string currentPath)
{
	// Recursive helper method to build out a table of encoding strings for the symbols
	if (startingPoint == nullptr) return; // If the node we are at is null, return
	if (isLeaf(startingPoint))
	{
		// We arrived at a leaf so we need to keep track of the path
		encodingStrings[startingPoint->symbol] = currentPath; // Set the encoding string for the symbol we are at to the currentPath
		if (currentPath.length() > 7) // If our currentPath has a length greater than 7 we know it is fine to use as padding
		{
			paddingBits = currentPath; // Set our padding to be the current path since its long enough
		}

		return; // There is nothing more we need to do on this path so return our
	}
	if (startingPoint->left != nullptr)
	{
		// If the left child isn't null recursively build the encoding strings down that path, adding a 0 to the current path
		buildEncodingStrings(startingPoint->left, currentPath + "0");
	}
	if (startingPoint->right != nullptr)
	{
		// If the right child isn't null recursively build the encoding strings down that path, adding a 1 to the current path
		buildEncodingStrings(startingPoint->right, currentPath + "1");
	}
}

void Huffman::DecodeFile(string inputFile, string outputFile)
{
	// This method Decodes an inputFile into our outputFile
	// This implements the -d command line parameter
	if (inputFile == outputFile)
	{
		// Our input can't be the same as our output, so display an error and exit
		cout << "Input File can not be equal to Output File" << endl;
		return;
	}
	openFiles(inputFile, outputFile, ""); // Open up our input and output files, we don't need a tree stream here
	buildTreeFromFile(inputStream); // Build a tree from our input file, since our input must contain tree builder info
	decode(); // Decode the file based on the tree we built
	closeFiles(); // Close out the files now that we're done
	printActionDetail(); // Print info about the work we did
}

void Huffman::EncodeFileWithTree(string inputFile, string treeFile, string outputFile)
{
	// This method encodes an input file into an output file,
	// using tree builder information from a third tree file
	// This implements the -et command line parameter
	if (inputFile == outputFile)
	{
		// Our input and output files can't be the same so display an error and exit
		cout << "Input File can not be equal to Output File" << endl;
		return;
	}
	if (outputFile == "")
	{
		// If our output file is empty, we want to decide it based on
		// our input file, so first we look for a period
		auto dotLoc = inputFile.find(".");
		if (dotLoc == string::npos)
		{
			// If we don't have a period in the input file simply append .huf to it for our output
			outputFile = inputFile + ".huf";
		}
		else
		{
			// Otherwise, replace the extension with .huf
			string fileNameWithoutExtension = inputFile.substr(0, dotLoc);
			outputFile = fileNameWithoutExtension + ".huf";
		}
	}
	openFiles(inputFile, outputFile, treeFile); // Open up all three of our files as we need
	buildTreeFromFile(treeStream); // Build our tree based on the information from our treeStream
	buildEncodingStrings(nodes[0], ""); // Build our table of encoding strings from that tree
	encode(); // Encode the file
	closeFiles(); // Close our files since we are done
	printActionDetail(); // Print info about what we did
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
	inputStream.clear(); // Clear out any errors we may have in our inputStream
	inputStream.seekg(0); // Make sure our input is at the beginning of the file
	char character; // Character to get the input into 
	string buffer = ""; // A buffer string that we will fill up with encoding strings
	while (inputStream.get(character)) // Loop through the input, getting each character into character
	{
		bytesIn++; // We read in a byte so increment it
		unsigned char realChar = character; // Coerce our character into an unsigned char so we don't get array access errors
		buffer += encodingStrings[realChar]; // Add the encoding string for our current character to the buffer
		while (buffer.length() >= 8) // While our buffer length is greater than or equal to 8, we need to encode it into our output file
		{
			unsigned char byte = 0; // Make a new unsigned char to keep track of our byte
			for (int i = 0; i <= 7; i++) // Loop 8 times
			{
				bool on = buffer[i] == '1'; // Make a boolean variable to determine if the current spot we are on in the buffer is 1 or not
				byte |= (on << (7 - i)); // Use the bitwise or operator on our byte to set the current location (0-7) to that locations value in the buffer
			}
			outputStream.put(byte); // Output our byte to the file
			bytesOut++; // Increment our bytesOut counter
			buffer = buffer.substr(8); // Set our buffer equal to the substring from position 8 (after the bits we have encoded) to the end
		}
	}
	if (buffer.length() < 8 && buffer.length() > 0) // If we still have something left in our buffer after looping through then we need to handle padding
	{
		buffer += paddingBits; // Add our padding bits onto the buffer
		// Now do the same encoding process as above, but only once
		unsigned char byte = 0; // Make a new unsigned char to keep track of our byte
		for (int i = 0; i <= 7; i++) // Loop 8 times
		{
			bool on = buffer[i] == '1'; // Make a boolean variable to determine if the current spot we are on in the buffer is 1 or not
			byte |= (on << (7 - i)); // Use the bitwise or operator on our byte to set the current location (0-7) to that locations value in the buffer
		}
		outputStream.put(byte); // Output our byte to the file
		bytesOut++; // Increment our bytesOut counter
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
	if (isLeaf(currentNode))
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
	// Helper method to print out information for what work we did
	clock_t end = clock(); // Grab the current time
	double secondsElapsed = difftime(end, start) / 1000; // Determine the amount of seconds elapsed from when we started our work on the file
	cout << "Time: " << secondsElapsed << " seconds.   "; // Output the elapsed time followed by a few spaces
	cout << "Bytes in / Bytes Out: " << formatNumber(bytesIn) << " / " << formatNumber(bytesOut) << endl; // Output our nicely formatted bytesIn and bytesOut numbers using a helper method below
}

string Huffman::formatNumber(unsigned int num)
{
	// Helper method that formats an unsigned int we take as a parameter
	// to contain commas in the appropriate places (American format, not using period like some European countries)
	string number = to_string(num); // Turn the number into a string
	for (int i = number.length() - 3; i > 0; i -= 3) // Starting 3 in from the end of the string, and looping through decrementing by 3 each time, until we reach the beginning of the string
	{
		number = number.substr(0, i) + "," + number.substr(i); // Update our number string to be the substring before i, where we want a comma, plus a comma, plus the substring after i
	}
	return number; // Return our nicely formatted number!
}

bool Huffman::isLeaf(node* node)
{
	// Helper method to check if a node is a leaf
	// If both children are null, the node is a leaf
	return node->left == nullptr && node->right == nullptr;
}