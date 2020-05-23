#include "UserInterface.h"

#define EURO_ASCII "    __    \n  _/  |_  \n / $$   \ \n/$$$$$$  |\n$$ \__$$/ \n$$      \ \n $$$$$$  |\n/  \__$$ |\n$$    $$/ \n $$$$$$/  \n   $$/    "

UserInterface::UserInterface() : keys{ "blockid", "height", "merkleroot", "nTx", "nonce", "previousblockid", "tx" }
{
	if (AllegroInit() && ImguiInit())
	{
		timer = al_create_timer(1 / 60.0);
		queue = al_create_event_queue();
		al_register_event_source(queue, al_get_display_event_source(display));
		al_register_event_source(queue, al_get_mouse_event_source());
		al_register_event_source(queue, al_get_keyboard_event_source());
		timer_queue = al_create_event_queue();
		al_register_event_source(timer_queue, al_get_timer_event_source(timer));

		window_flags |= ImGuiWindowFlags_NoResize;
		Error = false;
		close = false;
		failed = false;
		EventoActual = Event::dummyEvent;
		errorString = "";
		EstadoActual = Estado::MainMenu;
	}
	else
	{
		Error = true;
	}
}

UserInterface::~UserInterface()
{
	ImGui_ImplAllegro5_Shutdown();
	ImGui::DestroyContext();
	if (!Error)
	{
		al_shutdown_primitives_addon();
		al_uninstall_keyboard();
		al_uninstall_mouse();
		al_shutdown_image_addon();
	}
	if (timer_queue)
		al_destroy_event_queue(timer_queue);
	if (queue)
		al_destroy_event_queue(queue);
	if (display)
		al_destroy_display(display);
	if (timer)
		al_destroy_timer(timer);

}

bool UserInterface::GetError()
{
	return Error;
}

bool UserInterface::Running()
{
	if (checkIfEvent())
	{
		Dispatch();
	}
	return (!close);
}

bool UserInterface::checkIfEvent(void)
{
	bool isThereAnEvent = false;

	while (al_get_next_event(queue, &ev))
	{
		ImGui_ImplAllegro5_ProcessEvent(&ev);
	}

	if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
	{
		EventoActual = Event::Close;
		isThereAnEvent = true;
	}

	if (print_current_state(EstadoActual))				//Devuelve true si huvo un evento (Usuario presiono un boton)
	{
		isThereAnEvent = true;
	}

	return isThereAnEvent;
}

void UserInterface::Dispatch(void)
{
	switch (EventoActual)
	{
	case Event::Close:
		close = true;
		break;

	case Event::Error:
		cout << "Hubo un error, capo (Event::Error)" << endl;
		break;

	case Event::fileSelected:
		EstadoActual = Estado::FileView;
		break;

	case Event::BackToMainMenu:
		EstadoActual = Estado::MainMenu;
		break;

	default:
		break;
	}
}

bool UserInterface::print_current_state(Estado CurrentState)
{
	bool userEvento = false;
	switch (CurrentState)
	{
	case Estado::MainMenu:
		if (print_MainMenu()) userEvento = true;
		break;
	case Estado::FileView:
		if (print_blockSelection()) userEvento = true;
		break;
	case Estado::Error:
		//if(print_error()) userEvento = true;
		break;
	default:
		break;
	}

	return userEvento;
}

bool UserInterface::print_MainMenu(void)
{
	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();


	ImGui::SetNextWindowPos(ImVec2(200, 10));
	ImGui::SetNextWindowSize(ImVec2(500, 500));

	ImGui::Begin("Welcome to the EDAcoin", 0, window_flags);

	/*bool show_demo_window = true;
	ImGui::ShowDemoWindow(&show_demo_window);*/

	static char path[MAX_PATH];
	ImGui::InputText("Directorio", path, sizeof(char) * MAX_PATH);

	bool eventHappened = false;


	if (string(path) != "") {

		directory = path;
		jsonPaths = lookForJsonFiles(path);
		if (jsonPaths.size() > 0 && print_SelectJsons(jsonPaths)) {
			eventHappened = true;
		}
	}
	else {
		for (int i = 0; i < 3; i++) {
				if (i != 0) {
					ImGui::SameLine;
				}
				ImGui::Text(EURO_ASCII);
		}
		
	}
	//entra aca solo si hubo un error en el parseo para mostrarlo en forma de pop-up
	if (failed == true)
	{
		ImGui::OpenPopup("Failed");

		if (ImGui::BeginPopupModal("Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(errorString.c_str());
			ImGui::Separator();

			if (ImGui::Button("OK", ImVec2(120, 0))) 
			{ 
				errorString = "";
				failed = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	
	ImGui::End();

	//Rendering
	ImGui::Render();

	al_clear_to_color(al_map_rgb(211, 211, 211));

	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
	al_flip_display();
	return eventHappened;

}


vector<string> UserInterface::lookForJsonFiles(const char* directoryName)
{

	vector<string>filenames;

	if (string(directoryName).size() > 2) {

		fs::path bPath(directoryName);

		if (exists(bPath) && is_directory(bPath))
		{
			for (fs::directory_iterator iterator(bPath); iterator != fs::directory_iterator(); iterator++)
			{
				if ((iterator->path().extension().string() == ".json"))
				{
					filenames.push_back(iterator->path().filename().string());
				}
			}
		}

	}

	return filenames;
}


bool UserInterface::print_SelectJsons(vector<string>& nombres)
{
	bool eventHappened;

	static int checked = -1;
	ImGui::BeginChild(".json files in current folder", ImVec2(300, 400), true, ImGuiWindowFlags_None);

		for (int i = 0; i < nombres.size(); i++)
		{
			ImGui::RadioButton(nombres[i].c_str(), &checked, i);
		}

	ImGui::EndChild();



	if (ImGui::Button("Seleccionar"))
	{
		for (int i = 0; i < nombres.size(); i++)
		{
			if (checked != -1) {
				filename = nombres[checked];
			}
			else {
				//sin esta parte daria un error al querer leer filename con nada guardado.
				printf("Tenes que seleccionar un radio button antes de clickear el boton\n");
				errorString = "Tenes que seleccionar un radio button antes de clickear el boton\n";
				failed = true;
				//return true;
				eventHappened = false;
			}
		}

		if (parseallOk(directory + "\\" + filename))
		{
			EventoActual = Event::fileSelected;
			eventHappened = true;
		}
		else {
			//indico que fallo para que muestre luego el pop-up con el fallo
			failed = true;
			eventHappened = false;
		}
		
		cout << filename << endl;
		cout << "full path: " << endl << directory + "\\" + filename << endl;
	}
	else {
		eventHappened = false;
	}
	return eventHappened;
}

bool UserInterface::print_blockSelection(void)
{
	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();


	ImGui::SetNextWindowPos(ImVec2(200, 10));
	ImGui::SetNextWindowSize(ImVec2(600, 500));

	ImGui::Begin(filename.c_str(), 0, window_flags);

	bool eventHappened = false;

	blockActions();

	if (ImGui::Button("Seleccionar otra block chain")) {
		EventoActual = Event::BackToMainMenu;
		eventHappened = true;
	}

	ImGui::End();

	if (displayInfo.show) {
		//ImGui::SetNextWindowPos(ImVec2(400, 10));  //lo quite por conveniencia podemos ponerlo despues pero en una pos mas comoda
		ImGui::SetNextWindowSize(ImVec2(600, 150));
		ImGui::Begin("Info", 0, window_flags);
		
		ImGui::Text(displayInfo.blockId.c_str());
		ImGui::Text(displayInfo.previousBlockId.c_str());
		ImGui::Text(displayInfo.BlockNumber.c_str());
		ImGui::Text(displayInfo.NTransactions.c_str());
		ImGui::Text(displayInfo.blockId.c_str());

		if (ImGui::Button("Close"))
		{
			displayInfo.show = false;
		}

		ImGui::End();
	}



	//Rendering
	ImGui::Render();

	al_clear_to_color(al_map_rgb(211, 211, 211));

	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
	al_flip_display();
	return eventHappened;

}


bool UserInterface::parseallOk(string str)
{
	
	bool retVal = false;
	//Abro el archivo y lo asigno a mi variable miembro blocks (de tipo json)
	std::ifstream blocks_file(str, std::ifstream::binary);

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
					errorString = "Error parsing \nwrong key number!";
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
						errorString = "Error parsing \nwrong key value!";
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
		errorString = "Error parsing \nEmpty or corrupt file!";
	}
	return retVal;
}


void UserInterface::blockActions() {

	static int checked = -1;
	for (int i = 0; i < BlockChainJSON.size(); i++)
	{
		string blockId = string(BlockChainJSON[i]["blockid"].get<string>());
		ImGui::RadioButton(blockId.c_str(), &checked, i);
	}

	/* Acciones sobre el bloque */

	if (ImGui::Button("Info"))
	{
		showBlockInfo(checked);
	}

	if (ImGui::Button("Calcular el Merkle root"))
	{
		// TODO: CALCULO DE MERKEL ROOT SE PUEDE MOSTRAR EN ESTA PANTALLA
	}

	if (ImGui::Button("Validar el Merkle root"))
	{
		// TODO: VALIDAR DE MERKEL ROOT SE PUEDE MOSTRAR EN ESTA PANTALLA
	}

	if (ImGui::Button("Merkle Tree"))
	{
		// TODO: ESTO IRIA EN OTRA PANTALLA APARTE COMO INFO
	}

}


bool UserInterface::AllegroInit()
{
	if (al_init())
	{
		if (al_init_image_addon())
		{
			if (al_install_mouse())
			{
				if (al_install_keyboard())
				{
					if (al_init_primitives_addon())
					{
						//Backgrounds[0] = al_load_bitmap("twitterbac.png");
						return true;
					}
					else
					{
						printf("ERROR al_init_primitives_addon");
						al_uninstall_keyboard();
						al_shutdown_image_addon();
						al_uninstall_mouse();
					}
				}
				else
				{
					printf("ERROR al_instal_keyboard\n");
					al_shutdown_image_addon();
					al_uninstall_mouse();
				}
			}
			else
			{
				printf("ERROR al_install_mouse\n");
				al_shutdown_image_addon();
			}
		}
		else
			printf("ERROR al_init_image_addon\n");
	}
	else
		printf("ERROR al_init\n");
	return false;
}

bool UserInterface::ImguiInit(void)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	display = al_create_display(SIZE_SCREEN_X, SIZE_SCREEN_Y);
	if (display)
	{
		al_set_window_position(display, 500, 100); //posicion del menu

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		ImGui_ImplAllegro5_Init(display);
	}
	else
		return false;

	return true;
}

void UserInterface::showBlockInfo(int index) {

	displayInfo.blockId = "Block Id: " + BlockChainJSON[index]["blockid"].get<string>();
	displayInfo.previousBlockId = "Previous block id: " + BlockChainJSON[index]["previousblockid"].get<string>();
	displayInfo.NTransactions = "Number of transactions: " + to_string(BlockChainJSON[index]["nTx"].get<int>());
	displayInfo.BlockNumber = "Block number: " + to_string(index);
	displayInfo.nonce = "Nonce: " + to_string(BlockChainJSON[index]["nonce"].get<int>());
	displayInfo.show = true;
}
