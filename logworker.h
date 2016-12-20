#ifndef GTKMM_LOGWORKER_H
#define GTKMM_LOGWORKER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include "logMessage.h"

class LogWindow;

class LogWorker
{
public:
  LogWorker();

  // Thread function.
  void do_work(LogWindow * caller);
  void get_data(string* a, string* b, string* c, string* d, string* e, int* f) const;
  void get_text(int msg_num, std::string & text) const;
  void arrange_data(string time, string layer, string dir, string ue_id, string message, string& t, string& l, string& d, string& u, string& m);
  void get_line(char * buff, int l, string& line);
  void stop_work();
  void pause_work();
  bool has_stopped() const;
  bool has_paused() const;
  void notify_thread();
  

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
  bool m_shall_pause;
  bool m_has_paused;
  bool resume_work;
  int counter, strEnd, strStart;
  log_message log_msgs[10000];
  int msgcnt;
  string line, last_msg;
  condition_variable wait_condition;
};

#endif // GTKMM_EXAMPLEWORKER_H