#pragma once
#define SIZE_SCREEN_X 1000
#define SIZE_SCREEN_Y 700
#define SIZE_PICX 256
#define SIZE_PICY 256
#define POS_X 275
#define POS_Y 360

#define MAX_PATH 500
#define MAX_FILES 40
#define MAX_BLOCKS 40

#include <vector>
#include<string>
#include <list> 
#include <iterator>
#include <stdio.h>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h> 
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h> 

#include "blockchainHandler.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "imgui_files/imgui.h"
#include "imgui_files/imgui_impl_allegro5.h"

using namespace std;

namespace fs = boost::filesystem;

enum class Event { Close, ShowmerkleTree, Error, fileSelected, dummyEvent, BackToMainMenu };
enum class Estado { MainMenu, FileView, Error };
enum class POPS { Nothing, Merkle ,Failed };

typedef struct {
	string blockId;
	string previousBlockId;
	string NTransactions;
	string BlockNumber;
	string nonce;
	bool show = false;
} BlockInfo;

typedef struct {
	string tree;
	bool show = false;
} BlockTree;

class UserInterface
{

	struct Node
	{
		string data;
		Node* left, * right;

		Node(string data = "")
		{
			this->data = data;
			this->left = this->right = nullptr;
		}
	};

	struct Trunk
	{
		Trunk* prev;
		string str;

		Trunk(Trunk* prev, string str)
		{
			this->prev = prev;
			this->str = str;
		}
	};

public:
	UserInterface();
	~UserInterface();
	bool GetError();
	bool Running();

private:
	bool AllegroInit();
	bool ImguiInit();
	bool checkIfEvent();
	void Dispatch();
	bool print_current_state(Estado);
	bool print_MainMenu();
	bool print_blockSelection();
	bool print_SelectJsons(vector<string>& nombres);
	vector<string> lookForJsonFiles(const char* directoryName);
	void blockActions();
	void showBlockInfo(int index);
	void printTree(vector<vector<string>> Tree);
	void showTrunks(Trunk* p);
	void printTreeConsole(Node* root, Trunk* prev, bool isLeft);

	ALLEGRO_TIMER* timer;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_EVENT ev;
	ALLEGRO_EVENT_QUEUE* timer_queue;
	ALLEGRO_EVENT timerev;
	ALLEGRO_DISPLAY* display;
	ALLEGRO_BITMAP* tempBitmap;

	bool Error;
	bool close;
	bool failed;
	bool stillRunning;
	bool correctlyInitialized;
	bool WorkInProgress;
	bool validate;
	string directory;
	vector<string> jsonPaths;
	string filename;
	BlockInfo displayInfo;
	BlockTree displayTree;
	vector<vector<Node>> NodeTree;
	string stringPop;
	Event EventoActual;
	Estado EstadoActual;
	POPS pop;
	ImGuiWindowFlags window_flags;
	string errorString;
	blockchainHandler blockchainHandler;
};

