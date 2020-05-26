#pragma once

#include "json.hpp"
#include <string>
#include <vector>
#include <iterator>
#include <list> 
#include <iostream>
#include <map>

using namespace std;
using json = nlohmann::json;

class blockchainHandler {
public:
	blockchainHandler();
	bool parseallOk(string filepath, string * ErrorString = 0);
	size_t chainSize();
	json getBlockChainJSON() { return BlockChainJSON; }
	string getMerkleroot() { return merkleroot; }
	vector<string> getKeys() { return keys; }
	string hexCodexASCII(unsigned int);
	vector<vector<string>> makeMerkleTree(int blockNumber);

	
private:
	json BlockChainJSON;
	vector<string> keys;
	string merkleroot;
	unsigned int generateID(unsigned char* str);
	
};
