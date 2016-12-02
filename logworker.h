#ifndef GTKMM_LOGWORKER_H
#define GTKMM_LOGWORKER_H

#include <gtkmm.h>
#include <thread>
#include <mutex>
#include "logMessage.h"
using namespace std;

class LogWindow;

class LogWorker
{
public:
  LogWorker();

  // Thread function.
  void do_work(LogWindow * caller);
  void get_data(string* a, string* b, string* c, string* d, string* e, int* f) const;
  void arrange_data(string time, string layer, string dir, string ue_id, string message, string& t, string& l, string& d, string& u, string& m);
  void get_line(char * buff, int l, string& line);
  void stop_work();
  bool has_stopped() const;
  

private:
  // Synchronizes access to member data.
  mutable std::mutex m_Mutex;

  // Data used by both GUI thread and worker thread.
  string time;
  string layer;
  string dir;
  string ue_id;
  string msg;
  bool m_shall_stop;
  bool m_has_stopped;
  int counter, strEnd, strStart;
  log_message log_msgs[10000];
  int msgcnt;
  string line, last_msg;
};

#endif // GTKMM_EXAMPLEWORKER_H