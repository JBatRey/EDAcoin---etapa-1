#include <iostream>
#include <memory>
#include "UserInterface.h"
#include <iostream>

using namespace std;

int main(void) {

    UserInterface gui; // <-- Contruís 

    if (!gui.GetError())
    {
        cout << "Gui correctly initialized!" << endl;
        while (gui.Running()) {
        
        }
    }
    else {
        cout << "Error, couldn't initialize Gui object!" << endl;
    }

    return 0;
}
