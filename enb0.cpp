#include <iostream>
#include <fstream>
#include <gtkmm/application.h>
#include "logwindow.h"
#include "logwindow.cpp"

using namespace std;


int main(int argc, char *argv[])
{
auto app = 
    Gtk::Application::create(argc, argv);
    LogWindow window;
  
return app->run(window);

    
}