using namespace std;

void print_message_log(int h, int m, int s, int ms, string layer, string dir, string ue_id, string message)
{
  cout << h << ':';
  cout << m << ':';
  cout << s << '.';
  cout << ms << ' ';
  cout << layer << ' ';
  cout << dir << ' ';
  cout << ue_id << message << endl;
}

void print_pdu_log(int h, int m, int s, int ms, string layer, string dir, string ue_id, string sc)
{
  cout << h << ':';
  cout << m << ':';
  cout << s << '.';
  cout << ms << ' ';
  cout << layer << ' ';
  cout << dir << ' ' << ue_id << sc << endl;
}