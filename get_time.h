void convert_to_time(std::string line_read, int& a, int& b, int& c, int& d, std::string & e)
{
  a = stoi (line_read);
  b = stoi (line_read.substr(3,2));
  c = stoi (line_read.substr(6,2));
  d = stoi (line_read.substr(9,3));
  e = line_read.substr(0,12);

}