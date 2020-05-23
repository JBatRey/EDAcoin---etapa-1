#include "blockchainHandler.h"

#include <fstream>

size_t blockchainHandler::chainSize() { return BlockChainJSON.size(); }


blockchainHandler::blockchainHandler() : keys{ "blockid", "height", "merkleroot", "nTx", "nonce", "previousblockid", "tx" } {};

string blockchainHandler::getFieldAsString(int blockNumber, const char* fieldname) {
	if (blockNumber < 0) {
		return "";
	}
	else {
		return string(BlockChainJSON[blockNumber][fieldname].get<string>());
	}
}

bool blockchainHandler::parseallOk(string str, string * errorString)
{

	bool retVal = false;
	//Abro el archivo y lo asigno a mi variable miembro blocks (de tipo json)
	ifstream blocks_file(str, ifstream::binary);

	try {
		blocks_file >> BlockChainJSON;
		if (!BlockChainJSON.empty()) { //chequea que no esté vacío así no crashea todo con el parser

		//Recorro todos los elementos de la lista y me fijo si cada diccionario tiene 7 keys
			BlockChainJSON.size();
			for (int i = 0; i < BlockChainJSON.size(); i++)
			{
				if (BlockChainJSON[i].size() != 7)
				{
					cout << "Error uno de los bloques tiene menos de 7 keys" << endl;
					*errorString = "Error parsing \nwrong key number!";
					retVal = false;
				}
			}

			//Recorro nuevamente todos los elementos de la lista y todos los elementos del diccionario 
			//y para cada uno de ellos me fijo que su nombre corresponda con los del vector keys
			for (auto item : BlockChainJSON)
			{
				for (int i = 0; i < item.size(); i++) //item.size es 7 igual ya lo comprobamos arriba
				{
					if (item.find(keys[i]) == item.end())
					{
						cout << "Error, una de las keys no corresponde con lo esperado" << endl;
						*errorString = "Error parsing \nwrong key value!";
						retVal = false;
					}
				}
			}

			retVal = true;
		}
	}
	catch (json::parse_error& e)
	{
		std::cerr << e.what() << std::endl;
		*errorString = "Error parsing \nEmpty or corrupt file!";
	}
	return retVal;
}