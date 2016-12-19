#include "get_time.h"
#include "printing_log.h"

class log_message{
public:
  int hour, mins, secs, mil_sec;
	string time, layer, ue_id, dir, short_content, long_content;
  log_message (void);
  void sort_message(string);
};

log_message::log_message(void){
  hour = 0;
  mins = 0;
  secs = 0;
  mil_sec = 0;
  dir = "-";
  layer = "-";
  short_content = "";
  long_content = "";
}

void log_message::sort_message(string line_read){
  convert_to_time(line_read, hour, mins, secs, mil_sec, time);
  dir = line_read.substr (19,2);
  layer = line_read.substr(12,6);
  ue_id = line_read.substr(22,4);
  short_content = line_read.substr (26, line_read.length()-27);
  print_message_log(hour, mins, secs, mil_sec, layer, dir, ue_id, short_content);
}

