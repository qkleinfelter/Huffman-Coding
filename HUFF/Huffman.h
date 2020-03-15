/*
	Quinn Kleinfelter
	EECS 2520-001 Non Linear Data Structures Spring 2020
	Dr. Thomas
	Last Edited: 3/15/20

	Header file to contain the class definition for all of
	our Huffman work.
*/

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
	void MakeTreeBuilder(string inputFile, string outputFile); // Makes a tree builder file from inputFile in the specified outputFile
	void EncodeFile(string inputFile, string outputFile); // Encodes inputFile into outputFile (will also contain tree builder information in the first 510 bytes)
	void DecodeFile(string inputFile, string outputFile); // Decodes inputFile into outputFile
	void EncodeFileWithTree(string inputFile, string treeFile, string outputFile); // Encodes inputFile, using the tree builder information in treeFile, into outputFile
	void DisplayHelp(); // Displays Help information

private:
	struct node // Basic node struct used as the baseline for our huffman trees 
	{
		unsigned char symbol; // The ASCII symbol this node represents, only matters in the leaves
		unsigned int weight; // The weight of the node, either the frequency that the symbol occurs, or the added weights of the children
		node* left = nullptr; // Left child node
		node* right = nullptr; // Right child node
	};
	const static int numChars = 256; // Constant to keep track of the number of distinct characters we have, in this case 256, to represent 0-255 in ASCII
	unsigned int frequencyTable[numChars]; // A frequency table array, keeps track of the count of each character
	node* nodes[numChars]; // An array of nodes to be used to build the huffman tree
	string encodingStrings[numChars]; // An array of encoding strings used to keep track of the path in the tree to each character
	string paddingBits = ""; // An initially empty string that we will eventually fill with a path > 7 to ensure we have sufficient padding when encoding
	ifstream inputStream; // A stream used for our input files
	ifstream treeStream; // A stream used optionally for a secondary input for a separate tree file
	ofstream outputStream; // A stream used for our output files
	unsigned int bytesIn = 0; // Unsigned int to keep track of the amount of bytes we read in, so we can output this number eventually
	unsigned int bytesOut = 0; // Unsigned int to keep track of the amount of bytes we print out, so we can output this number eventually
	clock_t start = clock(); // The time we started running the program in clock ticks, so we can keep track of how long our program runs

	bool openFiles(string inputFile, string outputFile, string treeFile); // Helper method to open up our files into the appropriate streams
	void buildFrequencyTable(); // Helper method that builds the frequency table for the input file
	int getSmallestNodeIndex(int indexToSkip); // Helper method that gets the smallest node index from our nodes array, skipping the input parameter so we don't output the same index twice
	void buildTree(); // Helper method that combines items in the nodes[] array to build our tree
	void buildTreeFromFile(ifstream& file); // Helper method that builds a tree from the parameter file (either inputStream, or treeStream)
	void buildEncodingStrings(node* startingPoint, string currentPath); // Helper method to build all encoding strings starting at a given node with a given path
	void encode(); // Helper method that encodes a file
	void decode(); // Helper method that decodes a file
	void followTree(unsigned char byte, int checkBit, node*& currentNode); // Helper method that navigates through the tree checking the bit of the byte and deciding whether to go left or right based on it
	void closeFiles(); // Helper method to close our files when we are done
	void deleteSubtree(node* startingNode); // Helper method that deletes the subtrees of a node - used for destructing our huffman object

	void printActionDetail(); // Helper method to print out information about how the file ran, i.e., elapsed time and bytes in / out
	string formatNumber(unsigned int num); // Helper method that formats an unsigned int to contain commas in the correct places

	bool isLeaf(node* node); // Helper method that we use to check if we are at a leaf
};