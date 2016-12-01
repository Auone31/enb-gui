#include "logwindow.h"
#include "logworker.h"
#include "logworker.cpp"
#include <string>

LogWindow::LogWindow()
: m_VBox(Gtk::ORIENTATION_HORIZONTAL),
  m_Button_All("ALL"),
  m_Button_MAC("MAC"),
  m_Button_RRC("RRC"),
  m_Button_NAS("NAS"),
  Stop_Button("Stop"),
  Quit_Button("Quit", true),
  m_Dispatcher(),
  m_Worker(),
  m_WorkerThread(nullptr)
{
  set_title("Nutaq-Amarisoft");
  set_border_width(5);
  set_default_size(1000, 1000);

  add(m_VBox);

  //Add the TreeView, inside a ScrolledWindow, with the button underneath:
  m_ScrolledWindow.add(m_TreeView);

  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_VBox.pack_end(m_ScrolledWindow);

  //Add buttons:
  m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);


  m_ButtonBox.pack_start(m_Button_All, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_MAC, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_RRC, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(m_Button_NAS, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(Stop_Button, Gtk::PACK_SHRINK);
  m_ButtonBox.pack_start(Quit_Button, Gtk::PACK_SHRINK);
  m_ButtonBox.set_border_width(10);
  m_ButtonBox.set_spacing(10);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_START);

  


  //Connect signals:
  m_Button_All.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_all) );
  m_Button_MAC.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_quit) );
  m_Button_RRC.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_quit) );
  m_Button_NAS.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_quit) );
  Stop_Button.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_stop_button_clicked) );
  Quit_Button.signal_clicked().connect(sigc::mem_fun(*this,
              &LogWindow::on_button_quit) );

 

  //fill_buffers();
  //on_button_all();
  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(l_columns);
  m_TreeView.set_model(m_refTreeModel);
  refTreeSelection = m_TreeView.get_selection();

  

  //Fill the TreeView's model
   // Connect the handler to the dispatcher.
  m_Dispatcher.connect(sigc::mem_fun(*this, &LogWindow::on_notification_from_worker_thread));

  
  //Add the TreeView's view columns:
  //This number will be shown with the default numeric formatting.
  m_TreeView.append_column("Time", l_columns.Time);
  m_TreeView.append_column("Layer", l_columns.Layer);
  m_TreeView.append_column("Direction", l_columns.Direction);
  m_TreeView.append_column("UE ID", l_columns.UE_ID);
  m_TreeView.append_column("INFO", l_columns.Info);

  
  show_all_children();
}

/*void LogWindow::fill_buffers()
{
  m_refTextBuffer1 = Gtk::TextBuffer::create();
  m_refTextBuffer1->set_text(std::to_string(4));

  m_refTextBuffer2 = Gtk::TextBuffer::create();
  m_refTextBuffer2->set_text("This is some alternative text \n, from TextBuffer #2. This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.This is some alternative text, from TextBuffer #2.");

}*/

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
  }
  hide();
}

void LogWindow::update_start_stop_buttons()
{
  const bool thread_is_running = m_WorkerThread != nullptr;

  m_Button_All.set_sensitive(!thread_is_running);
  Stop_Button.set_sensitive(thread_is_running);
}

void LogWindow::notify()
{
  m_Dispatcher.emit();
}

void LogWindow::update_widgets()
{
  std::string a, b, c, d, e;
  m_Worker.get_data(&a, &b, &c, &d, &e);
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());

  row[l_columns.Time] = a;
  row[l_columns.Layer] = b;
  row[l_columns.Direction] = c;
  row[l_columns.UE_ID] = d;
  row[l_columns.Info] = e;
}

void LogWindow::on_notification_from_worker_thread()
{
  if (m_WorkerThread && m_Worker.has_stopped())
  {
    // Work is done.
    if (m_WorkerThread->joinable())
      m_WorkerThread->join();
    delete m_WorkerThread;
    m_WorkerThread = nullptr;
    update_start_stop_buttons();
  }
  update_widgets();
}

void LogWindow::on_button_all()
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
    Stop_Button.set_sensitive(false);
    m_Button_All.set_sensitive(true);
  }
}
/*void LogWindow::on_button_all()
{
  m_TextView.set_buffer(m_refTextBuffer1);
}

void LogWindow::on_button_mac()
{
  m_TextView.set_buffer(m_refTextBuffer2);
}

void LogWindow::on_button_rrc()
{
  m_TextView.set_buffer(m_refTextBuffer2);
}

void LogWindow::on_button_nas()
{
  m_TextView.set_buffer(m_refTextBuffer2);
}*/

/*void LogWindow::get_data(std::string a, std::string b, std::string c, std::string d, std::string e){
  //m_refTreeModel = Gtk::ListStore::create(l_columns);
  //m_TreeView.set_model(m_refTreeModel);
  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  row[l_columns.Time] = a;
  row[l_columns.Layer] = b;
  row[l_columns.Direction] = c;
  row[l_columns.UE_ID] = d;
  row[l_columns.Info] = e;
  //show_all_children();
}*/