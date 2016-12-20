#include "logworker.h"
#include <chrono>

LogWorker::LogWorker() :
  m_Mutex(),
  m_shall_stop(false),
  m_has_stopped(false),
  m_shall_pause(false),
  m_has_paused(false),
  resume_work(true),
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

/****************************************************************************
* Function: get_data(string* a, string* b, string* c, string* d, string* e, int* f) const
*****************************************************************************
*
* Accesses to these data are synchronized by a mutex.
* 
*
****************************************************************************/ 

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

/****************************************************************************
* Function: get_text(int msg_num, std::string & text) const
*****************************************************************************
*
* Sets the messge to be displayed in the text view 
* 
*
****************************************************************************/ 

void LogWorker::get_text(int msg_num, std::string & text) const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  text = log_msgs[msg_num].long_content;
}


/****************************************************************************
* Function: stop_work()
*****************************************************************************
*
* Tells the worker to stop after finishing the current 
* task. Works with Stop and Quit buttons 
*
****************************************************************************/ 
void LogWorker::stop_work()
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_shall_stop = true;
  }
  wait_condition.notify_all();
}


/****************************************************************************
* Function: has_stopped() const
*****************************************************************************
*
* Confirms that the worker has stopped
* 
*
****************************************************************************/ 
bool LogWorker::has_stopped() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_stopped;
}


/****************************************************************************
* Function: pause_work()
*****************************************************************************
*
* Tells the worker to stop after finishing the current 
* task. Works with Pause button 
*
****************************************************************************/ 
void LogWorker::pause_work()
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  m_shall_pause = true;
}


/****************************************************************************
* Function: has_paused() const
*****************************************************************************
*
* Confirms that the worker has paused
* This functionality still has to be added
*
****************************************************************************/ 
bool LogWorker::has_paused() const
{
  std::lock_guard<std::mutex> lock(m_Mutex);
  return m_has_paused;
}


/****************************************************************************
* Function: arrange_data(string time, string layer, string dir, string ue_id, string message, string& t, string& l, string& d, string& u, string& m)
*****************************************************************************
*
* Assign Data to the correct variables for the caller to access
*
*
****************************************************************************/ 
void LogWorker::arrange_data(string time, string layer, string dir, string ue_id, string message, string& t, string& l, string& d, string& u, string& m)
{
  t = time;
  l = layer;
  d = dir;
  u = ue_id;
  m = message;
}


/****************************************************************************
* Function: get_line(char * buff, int l, string& line)
*****************************************************************************
*
* Arranges buffered content into a string
*
*
****************************************************************************/ 
void LogWorker::get_line(char * buff, int l, string& line)
{
  if(buff==0) return;
  line = string(buff, l);
}


/****************************************************************************
* Function: notify_thread()
*****************************************************************************
*
* Notifies the worker thread to resume work after caller finished printing
* in TreeView
*
****************************************************************************/ 
void LogWorker::notify_thread()
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    resume_work = true;
  }
  wait_condition.notify_all();
}


/****************************************************************************
* Function: resume_thread()
*****************************************************************************
*
* Notifies the worker thread to resume work after a pause occurred
*
*
****************************************************************************/ 
void LogWorker::resume_thread()
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    resume_work = true;
    m_shall_pause = false;
  }
  wait_condition.notify_all();
}


/****************************************************************************
* Function: do_work(LogWindow * caller)
*****************************************************************************
*
* Loads the log file in memory, reads the contents and sorts the data to be
* displayed in the GUI.
*
****************************************************************************/ 
void LogWorker::do_work(LogWindow * caller)
{
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_has_stopped = false;
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
        std::unique_lock<std::mutex> lock(m_Mutex);
        if(!resume_work || m_shall_pause)
        {
          //Wait for notification from the caller
          wait_condition.wait(lock, [this]{return (resume_work && !m_shall_pause) || m_shall_stop;});
        }
        
        //Sort pulled line in correct format
        log_msgs[msgcnt].sort_message(line);

        arrange_data(log_msgs[msgcnt].time, log_msgs[msgcnt].layer, log_msgs[msgcnt].dir,
          log_msgs[msgcnt].ue_id, log_msgs[msgcnt].short_content, 
          time, layer, dir, ue_id, msg);

        msgcnt++;

        resume_work = false;
        }
        caller -> notify();
      }
        
      // Storing Long Messages
      if (!isdigit(line[0]) && msgcnt>0) 
        {
          {log_msgs[msgcnt-1].long_content.append(line.append("\n"));}
        }

      if (m_shall_stop)
      {
        break;
      }
    
    }
    delete[] buff;
  } 
  {
    std::unique_lock<std::mutex> lock(m_Mutex);
    wait_condition.wait(lock, [this]{return resume_work || m_shall_stop;});
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

