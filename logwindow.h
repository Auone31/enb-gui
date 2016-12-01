#ifndef GTKMM_LOGWINDOW_H
#define GTKMM_LOGWINDOW_H

#include <gtkmm.h>
#include "logworker.h"

class LogWindow : public Gtk::Window
{
public:
  LogWindow();
  virtual ~LogWindow();
  void get_data(std::string a, std::string b, std::string c, std::string d, std::string e);
  void notify();
private:

  //void fill_buffers();

  
  //Signal handlers:
  void on_button_all();
  void on_button_mac();
  void on_button_rrc();
  void on_button_nas();
  void on_button_quit();
  void on_stop_button_clicked();
  void on_notification_from_worker_thread();
  void update_start_stop_buttons();
  void update_widgets();

    class Log_Columns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    Log_Columns()
    { add(Time); add(Layer); add(Direction); add(UE_ID); add(Info);}

    Gtk::TreeModelColumn<std::string> Time;
    Gtk::TreeModelColumn<std::string> Layer;
    Gtk::TreeModelColumn<std::string> Direction;
    Gtk::TreeModelColumn<std::string> UE_ID;
    Gtk::TreeModelColumn<std::string> Info;
  };


  //Child widgets:
  Gtk::Box m_VBox;
  Log_Columns l_columns;

  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;

  Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer1, m_refTextBuffer2;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

  Gtk::VButtonBox m_ButtonBox;
  Gtk::Button m_Button_All, m_Button_MAC, m_Button_RRC, m_Button_NAS, Stop_Button, Quit_Button;
  
  Glib::Dispatcher m_Dispatcher;
  LogWorker m_Worker;
  std::thread* m_WorkerThread;


};

#endif