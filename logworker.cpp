#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <mutex>
#include "logworker.h"

using namespace std;

LogWorker::LogWorker() :
  m_Mutex(),
  m_shall_stop(false),
  m_has_stopped(false),
  m_shall_pause(false),
  m_has_paused(false),
  time("0.0"),
  layer("0.0"),
  dir("0.0"),
  ue_id("0.0"),
  msg("0.0"),
  strEnd(-1),
  counter(0),
  msgcnt(0)
{
}

// Accesses to these data are synchronized by a mutex.

void LogWorker::get_data(string* a, string* b, string* c, string* d, string* e, int* f) const
{
  std::lock_guard<std::mutex> lock(m_Mutex);

    *a = time;
    *b = layer;
    *c = dir;
    *d = ue_id;
    *e = msg;
    *f = msgcnt-1;
}

void LogWorker::stop_work()
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  m_shall_stop = true;
}

bool LogWorker::has_stopped() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_stopped;
}

void LogWorker::pause_work()
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  m_shall_pause = true;
}

bool LogWorker::has_paused() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_paused;
}

void LogWorker::arrange_data(string time, string layer, string dir, string ue_id, string message, string& t, string& l, string& d, string& u, string& m){
          t = time;
          l = layer;
          d = dir;
          u = ue_id;
          m = message;
}

void LogWorker::get_line(char * buff, int l, string& line){
  if(buff==0) return;
  line = string(buff, l);
}

void LogWorker::do_work(LogWindow * caller)
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = false;
    m_has_paused = false;
  } // The mutex is unlocked here by lock's destructor.

    
ifstream log_file("enb0.log");
if (log_file)
  {
    log_file.seekg(0, log_file.end);
    int file_length = log_file.tellg();
    log_file.seekg(0, log_file.beg);
    cout << file_length << endl;
    char * buff = new char [file_length];
    log_file.read(buff, file_length);
    while (counter < file_length)
    {
      strStart = strEnd;
      counter = strEnd + 1;
      for (; counter < file_length; counter++)
      {
        if (buff[counter] == '\n')
        {
          strEnd = counter;
          break;
        }
      }

      get_line(buff + strStart + 1, strEnd - strStart, line);

      // This is where it begins
      if (isdigit(line[0]))
      { 
        {
        std::lock_guard<std::mutex> lock(m_Mutex);
        log_msgs[msgcnt].sort_message(line);
        arrange_data(log_msgs[msgcnt].time, log_msgs[msgcnt].layer, log_msgs[msgcnt].dir,
          log_msgs[msgcnt].ue_id, log_msgs[msgcnt].short_content, 
          time, layer, dir, ue_id, msg);
        msgcnt++;
        }
        caller -> notify();
        std::this_thread::sleep_for(std::chrono::milliseconds(45));
      }
        
      // Storing Long Messages
      if (!isdigit(line[0]) && msgcnt>0) 
        {
          {log_msgs[msgcnt-1].long_content.append(line.append("\n"));}
        }

      if (m_shall_stop || m_shall_pause)
      {
        break;
      }
    
    }
    delete[] buff;
  } 
 {
    //std::this_thread::sleep_for(std::chrono::milliseconds(30));
    std::lock_guard<std::mutex> lock(m_Mutex);
    if(m_shall_pause)
    {
      m_shall_pause = false;
      m_has_paused = true; 
    }
    else
    {
      m_shall_stop = false;  
      m_has_stopped = true;
      counter = 0;
      strEnd = -1;
      strStart = strEnd;
      msgcnt = 0;
    }
    log_file.close();
    caller -> notify();
  }
}

