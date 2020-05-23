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
	string getFieldAsString(int blockNumber, const char * fieldname);
	size_t chainSize(); 
	json BlockChainJSON;
	vector<string> getKeys() { return keys; }
	//map returnFields();
	
private:
	vector<string> keys;
	
};
