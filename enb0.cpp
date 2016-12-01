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

     


/*string line, last_msg;
mac_log msgs[10000];
rrc_log rrc_msgs[10000];
nas_log nas_msgs[10000];
int macmsgcnt = 0, rrcmsgcnt = 0, nasmsgcnt = 0;
ifstream log_file ("enb0.log");
if (log_file.is_open())
  {
    while ( getline (log_file,line) )
    {
      if (line[0] == '#')
      {	
      cout << line << '\n';
      }

      // This is where it begins
      if (isdigit(line[0]))
      {
        //If MAC Message
        if (line.find("[MAC]") != string::npos)
        {
         if (line.find("DL") == string::npos && line.find("UL") == string::npos)
         {
          msgs[macmsgcnt].mac_message(line);
          window.get_data(msgs[macmsgcnt].time, msgs[macmsgcnt].layer, msgs[macmsgcnt].dir, msgs[macmsgcnt].ue_id, msgs[macmsgcnt].message);
          macmsgcnt++;
         }
         else {
         msgs[macmsgcnt].mac_pdu(line);
         window.get_data(msgs[macmsgcnt].time, msgs[macmsgcnt].layer, msgs[macmsgcnt].dir, msgs[macmsgcnt].ue_id, msgs[macmsgcnt].short_content);
         macmsgcnt++;
         }
         last_msg = "[MAC]";
        }
        //If RRC Message
        if (line.find("[RRC]") != string::npos)
        {
         if (line.find("DL") == string::npos && line.find("UL") == string::npos)
         {
          rrc_msgs[rrcmsgcnt].rrc_message(line);
          rrcmsgcnt++;
         }
         else {
         rrc_msgs[rrcmsgcnt].rrc_pdu(line);
         rrcmsgcnt++;
         }
         last_msg = "[RRC]";
        }
        //If NAS Message
        if (line.find("[NAS]") != string::npos)
        {
         if (line.find("DL") == string::npos && line.find("UL") == string::npos)
         {
          nas_msgs[nasmsgcnt].nas_message(line);
          nasmsgcnt++;
         }
         else {
         nas_msgs[nasmsgcnt].nas_pdu(line);
         nasmsgcnt++;
         }
         last_msg = "[NAS]";
        }

      }

      // Storing Long Messages
      else if (!isdigit(line[0])) 
        {
          if(macmsgcnt > 0 && last_msg == "[MAC]")
          {msgs[macmsgcnt-1].long_content.append(line.append("\n"));}
          
          else if(rrcmsgcnt > 0 && last_msg == "[RRC]")
          {rrc_msgs[rrcmsgcnt-1].long_content.append(line.append("\n"));}

          else if(nasmsgcnt > 0 && last_msg == "[NAS]")
          {nas_msgs[nasmsgcnt-1].long_content.append(line.append("\n"));}  

        }
    }
    log_file.close();
  }
  else cout << "Unable to open file";
  system("/usr/bin/notify-send Notification \"Read complete log file\"");*/
  return app->run(window);
  //return 0;

    
}