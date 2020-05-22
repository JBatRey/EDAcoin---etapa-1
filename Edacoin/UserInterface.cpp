#include "UserInterface.h"

UserInterface::UserInterface()
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
		EventoActual = Event::dummyEvent;
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
		if(print_blockSelection()) userEvento = true;
		break;
	case Estado::MerkelTree:					
		//if(print_MerkerTree()) userEvento = true;
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
	ImGui::SetNextWindowSize(ImVec2(600, 150));

		ImGui::Begin("Welcome to the EDAcoin", 0, window_flags);
		
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
		
		ImGui::End();

		//Rendering
		ImGui::Render();

		al_clear_to_color(al_map_rgb(211, 211, 211));

		ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
		al_flip_display();
		return eventHappened;

}

bool UserInterface::print_blockSelection(void)
{
	ImGui_ImplAllegro5_NewFrame();
	ImGui::NewFrame();


	ImGui::SetNextWindowPos(ImVec2(200, 10));
	ImGui::SetNextWindowSize(ImVec2(600, 150));

	ImGui::Begin("Welcome to the EDAcoin", 0, window_flags);

	ImGui::Text("Hola!");

	bool show_demo_window;
	ImGui::ShowDemoWindow(&show_demo_window);

	bool eventHappened = false;

	ImGui::End();

	//Rendering
	ImGui::Render();

	al_clear_to_color(al_map_rgb(211, 211, 211));

	ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
	al_flip_display();
	return eventHappened;

}
	
	
vector<string> UserInterface::lookForJsonFiles(const char * directoryName)
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

	//Checkbox con imgui
	static bool checks[MAX_FILES] = { false };
	int i;
	for (i = 0; i < nombres.size(); i++)
	{
		if (ImGui::Checkbox(nombres[i].c_str(), &checks[i])) {
			for (int j = 0; j < nombres.size(); j++)
			{
				if (j != i && checks[j]) {
					checks[j] = false;
				}
			}
		}
	}

	if (ImGui::Button("Seleccionar"))
	{
		for (i = 0; i < nombres.size(); i++)
		{
			if (checks[i]) {
				filename = nombres[i];
			}
		}

		if(true /*parseallOk(directory + '/' + filename)*/)
		{
		EventoActual = Event::fileSelected;
		return true;
		}
		else {
		//Impimir pop-up que avise que no tiene un formato válido (hubo un error en el parseo)
		}
		

		cout << filename << endl;
		cout << "full path: " << endl << directory + "/" + filename << endl;
	}
	else {
		return false;
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
