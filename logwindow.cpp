#include "logwindow.h"
#include "logworker.h"
#include "logworker.cpp"
#include <string>

LogWindow::LogWindow()
: m_VBox(Gtk::ORIENTATION_HORIZONTAL),
  m_TreeBox(Gtk::ORIENTATION_HORIZONTAL),
  m_TextBox(Gtk::ORIENTATION_HORIZONTAL),
  Frame_TreeView(),
  Frame_TextView("Details"),
  m_Button_Start("Start"),
  m_Button_MAC("MAC"),
  m_Button_RRC("RRC"),
  m_Button_PR("Pause"),
  Stop_Button("Stop"),
  Quit_Button("Quit", true),
  m_Dispatcher(),
  m_Worker(),
  m_WorkerThread(nullptr)
{
  set_title("Nutaq-Amarisoft");
  set_border_width(5);
  set_default_size(2000, 1000);

  add(m_VBox);
  m_VBox.pack_start(Frame_TreeView, Gtk::PACK_EXPAND_WIDGET, 10);
  m_TreeBox.set_border_width(10);
  Frame_TreeView.add(m_TreeBox);
  //Add the TreeView, inside a ScrolledWindow, with the button underneath:
  m_ScrolledWindow.add(m_TreeView);

  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_TreeBox.pack_end(m_ScrolledWindow);

  //Add buttons:
  m_TreeBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_Start, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_PR, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(Stop_Button, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(Quit_Button, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_MAC, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_RRC, Gtk::PACK_SHRINK);
  m_ButtonBox.set_border_width(20);
  m_ButtonBox.set_spacing(10);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_START);

  


  //Connect signals:
  m_Button_Start.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_start) );
  m_Button_MAC.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_quit) );
  m_Button_RRC.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_quit) );
  m_Button_PR.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_PR_button_clicked) );
  Stop_Button.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_stop_button_clicked) );
  Quit_Button.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_quit) );

  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(l_columns);
  m_TreeView.set_model(m_refTreeModel);
   // Connect the handler to the dispatcher.
  m_Dispatcher.connect(sigc::mem_fun(*this, &LogWindow::on_notification_from_worker_thread));
  
  //Add the TreeView's view columns
  //This number will be shown with the default numeric formatting.
  m_TreeView.append_column("Time", l_columns.Time);
  m_TreeView.append_column("Layer", l_columns.Layer);
  m_TreeView.append_column("Direction", l_columns.Direction);
  m_TreeView.append_column("UE ID", l_columns.UE_ID);
  m_TreeView.append_column("INFO", l_columns.Info);

  m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this,
              &LogWindow::on_treeview_row_activated) );
  
  m_VBox.pack_end(Frame_TextView, Gtk::PACK_EXPAND_WIDGET, 5);
  m_TextBox.set_border_width(10);
  Frame_TextView.add(m_TextBox);
  text_ScrolledWindow.add(m_TextView);
  text_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  m_TextBox.pack_start(text_ScrolledWindow);

  show_all_children();
}

LogWindow::~LogWindow()
{
}

void LogWindow::on_button_quit()
{
  if (m_WorkerThread)
  {
    // Order the worker thread to stop and wait for it to stop.
    m_Worker.stop_work();
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
      delete m_WorkerThread;
      m_WorkerThread = nullptr;
  }
  hide();
}

void LogWindow::update_start_stop_buttons()
{
  const bool thread_is_running = m_WorkerThread != nullptr;

  m_Button_Start.set_sensitive(!thread_is_running);
  Stop_Button.set_sensitive(thread_is_running);
}

void LogWindow::notify()
{
  m_Dispatcher.emit();
}

void LogWindow::update_widgets()
{
  std::string a, b, c, d, e;
  int f;
  m_Worker.get_data(&a, &b, &c, &d, &e, &f);
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());

  row[l_columns.Time] = a;
  row[l_columns.Layer] = b;
  row[l_columns.Direction] = c;
  row[l_columns.UE_ID] = d;
  row[l_columns.Info] = e;
  row[l_columns.Row_Number] = f;

  m_Worker.notify_thread();
}

void LogWindow::on_notification_from_worker_thread()
{
  if (m_WorkerThread && m_Worker.has_stopped() || m_WorkerThread && m_Worker.has_paused())
  {
    // Work is done.
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
    delete m_WorkerThread;
    m_WorkerThread = nullptr;
    update_start_stop_buttons();
  }
  else
  {
    update_widgets();
  }
}

void LogWindow::on_button_start()
{
  if (m_WorkerThread)
  {
    std::cout << "Can't start a worker thread while another one is running." << std::endl;
  }
  else
  {
    // Start a new worker thread.
    m_WorkerThread = new std::thread(
      [this]
      {
        m_Worker.do_work(this);
      });
  }
  update_start_stop_buttons();
}

void LogWindow::on_PR_button_clicked()
{
  if (!m_WorkerThread && m_Button_PR.get_label() == "Pause")
  {
    std::cout << "Log not running." << std::endl;
  }
  else if (m_Button_PR.get_label() == "Pause")
  {
    m_Worker.pause_work();
    m_Button_PR.set_label("Resume");
  }
  else
  {
    m_Worker.notify_thread();
    m_Button_PR.set_label("Pause");
  }
}

void LogWindow::on_stop_button_clicked()
{
  if (!m_WorkerThread)
  {
    std::cout << "Can't stop a worker thread. None is running." << std::endl;
  }
  else
  {
   // Order the worker thread to stop.
    m_Worker.stop_work();
  }
}

void LogWindow::on_treeview_row_activated(const Gtk::TreeModel::Path& path,
        Gtk::TreeViewColumn* /*column*/)
{
  refTreeSelection = m_TreeView.get_selection();
  Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
  {
    Gtk::TreeModel::Row row_sel = *iter;
    std::string strText = row_sel[l_columns.Time];
    int row_n = row_sel[l_columns.Row_Number];
    std::cout << "Row activated: ID=" << row_n << std::endl;
    m_refTextBuffer = Gtk::TextBuffer::create();
    m_refTextBuffer->set_text(std::to_string(row_n));
    m_TextView.set_buffer(m_refTextBuffer);
  }
}

