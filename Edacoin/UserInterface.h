#pragma once
#define SIZE_SCREEN_X 1000
#define SIZE_SCREEN_Y 700
#define SIZE_PICX 256
#define SIZE_PICY 256
#define POS_X 275
#define POS_Y 360

#define MAX_PATH 500
#define MAX_FILES 40

#include<vector>
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

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "imgui_files/imgui.h"
#include "imgui_files/imgui_impl_allegro5.h"

using namespace std;
namespace fs = boost::filesystem;

enum class Event {Close, ShowMerkelTree, Error, fileSelected, dummyEvent};
enum class Estado {MainMenu, FileView, MerkelTree, Error};


class UserInterface
{

public:
	UserInterface();
	~UserInterface();
	bool GetError();
	bool RunningOne();

private:
	bool AllegroInit();
	bool ImguiInit();
	bool checkIfEvent(); 
	void Dispatch();   
	bool print_current_state(Estado);	
	bool print_MainMenu();

	bool print_SelectJsons(vector<string>& nombres);
	vector<string> lookForJsonFiles(const char * directoryName);

	ALLEGRO_TIMER* timer;
	ALLEGRO_EVENT_QUEUE* queue;
	ALLEGRO_EVENT ev;
	ALLEGRO_EVENT_QUEUE* timer_queue;
	ALLEGRO_EVENT timerev;
	ALLEGRO_DISPLAY* display;
	ALLEGRO_BITMAP* tempBitmap;
	bool Error;
	bool close;
	bool stillRunning;
	bool correctlyInitialized;
	bool WorkInProgress;
	string directory;
	vector<string> jsonPaths;
	string filename;

	Event EventoActual;
	Estado EstadoActual;
	ImGuiWindowFlags window_flags;
};