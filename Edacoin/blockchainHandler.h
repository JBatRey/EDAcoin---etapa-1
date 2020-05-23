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
	json BlockChainJSON;
	vector<string> getKeys() { return keys; }
	string hexCodexASCII(unsigned int);
	vector<vector<string>> makeMerkleTree(int blockNumber);

	
private:
	vector<string> keys;
	unsigned int generateID(unsigned char* str);
	
};
