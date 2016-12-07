#ifndef GTKMM_LOGWINDOW_H
#define GTKMM_LOGWINDOW_H

#include <gtkmm.h>
#include "logworker.h"

class LogWindow : public Gtk::Window
{
public:
  LogWindow();
  virtual ~LogWindow();
  void notify();
private:

  //void fill_buffers();

  
  //Signal handlers:
  void on_button_start();
  void on_button_mac();
  void on_button_rrc();
  void on_PR_button_clicked();
  void on_button_quit();
  void on_stop_button_clicked();
  void on_notification_from_worker_thread();
  void update_start_stop_buttons();
  void update_widgets();
  void on_treeview_row_activated(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

  class Log_Columns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    Log_Columns()
    { add(Row_Number); add(Time); add(Layer); add(Direction); add(UE_ID); add(Info);}

    Gtk::TreeModelColumn<int> Row_Number;
    Gtk::TreeModelColumn<std::string> Time;
    Gtk::TreeModelColumn<std::string> Layer;
    Gtk::TreeModelColumn<std::string> Direction;
    Gtk::TreeModelColumn<std::string> UE_ID;
    Gtk::TreeModelColumn<std::string> Info;
  };


  //Child widgets:
  Gtk::Box m_VBox, m_TreeBox, m_TextBox;
  //Gtk::Paned m_VBox;
  Log_Columns l_columns;

  Gtk::ScrolledWindow m_ScrolledWindow, text_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Gtk::TextView m_TextView;

  Glib::RefPtr<Gtk::TextBuffer> m_refTextBuffer;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Glib::RefPtr<Gtk::TreeSelection> refTreeSelection;

  Gtk::VButtonBox m_ButtonBox;
  Gtk::Button m_Button_Start, m_Button_MAC, m_Button_RRC, m_Button_PR, Stop_Button, Quit_Button;
  
  Gtk::Frame Frame_TreeView, Frame_TextView;
  Glib::Dispatcher m_Dispatcher;
  LogWorker m_Worker;
  std::thread* m_WorkerThread;


};

#endif