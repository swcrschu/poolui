/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WAnchor>
#include <Wt/WApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WLineEdit>
#include <Wt/WLink>
#include <Wt/WMenu>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WPushButton>
#include <Wt/WServer>
#include <Wt/WStackedWidget>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <future>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "web_ui.h"

//#include "ImagesWidget.h"

using namespace Wt;
using namespace std;
// c++0x only, for std::bind
// #include <functional>

class WebUIApplication : public WApplication {
 public:
  WebUIApplication(const WEnvironment &env);

 private:
  // define class

  WLineEdit *nameEdit_;
  WTextArea *ta_command_output;
  std::map<string, string> key_to_command;  // key value map command file key_to_command
  std::vector<string> keys ;

  void GetCommandoutput(std::string, WTextArea *);  // read command output line
  void read_input_file(std::string filename);       //
};

WebUIApplication::WebUIApplication(const WEnvironment &env)
    : WApplication(env) {
  setTitle("poolui");  // application title

  // css
  Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(this);
  bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
  bootstrapTheme->setResponsive(true);
  setTheme(bootstrapTheme);
  useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");

  // needed to load the templates for the objects and make the bootstrap theme
  // work
  this->messageResourceBundle().use(this->appRoot() + "text");

  // to enable threading for the main event loop
  this->enableUpdates(true);

  // Config file key_to_command read
  read_input_file("key_to_command");
  // menuContainer + menu
  Wt::WContainerWidget *menuContainer = new Wt::WContainerWidget();
  root()->addWidget(menuContainer);

  Wt::WNavigationBar *navigation = new Wt::WNavigationBar(menuContainer);
  navigation->setTitle("Link", "http://www.google.com/search?q=corpy+inc");
  navigation->setResponsive(true);

  // output WText menuItem
  Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget(menuContainer);
  contentsStack->addStyleClass("contents");

  // Setup a Left-aligned
  Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack, menuContainer);
  navigation->addMenu(leftMenu);

  leftMenu->addItem("Home", new Wt::WText("alternative Text"));
  leftMenu->addItem("link2", new Wt::WText("link2"));
  leftMenu->addItem("link3", new Wt::WText("link3"));

  // contentContainer + content
  Wt::WContainerWidget *contenContainer = new Wt::WContainerWidget();
  root()->addWidget(contenContainer);

  // button
  Wt::WPushButton *pb1 = new Wt::WPushButton("hier soll nur der key aus dem vector rein" );
  root()->addWidget(pb1);
  
  Wt::WPushButton *pb2 = new Wt::WPushButton(key_to_command["ip-add"]);

  ta_command_output = new WTextArea();
  root()->addWidget(ta_command_output);

  std::cout << "text_area ptr " << ta_command_output << std::endl;

  // action button key + place textArea commad_output
  pb1->clicked().connect(std::bind([=]() {

    std::thread([=]() {
      // getting update lock
      auto update_lock = this->getUpdateLock();
      // got update lock
      //    std::cout << "from thread: " << std::this_thread::get_id() << endl;
      GetCommandoutput(key_to_command["uptime"], ta_command_output);
      // push the changes to the browser
      this->triggerUpdate();
    }).detach();

  }));
  pb2->clicked().connect(std::bind([=]() {
    std::thread([=]() {
      auto update_lock = this->getUpdateLock();
      GetCommandoutput(key_to_command["ip-add"], ta_command_output);
      this->triggerUpdate();
    }).detach();

  }

				   ));
}

// To protect common data against simultaneous access from multiple threads
std::mutex g_GetCommandOutputMutex;

// CommandLine to TextAarea
void WebUIApplication::GetCommandoutput(std::string command,
					WTextArea *text_area) {
  // for owning a mutex or several mutexes for the duration of a scoped block
  std::lock_guard<std::mutex> guard(g_GetCommandOutputMutex);
  std::string text;
  FILE *in;
  char buff[512];
  if (!(in = popen(command.c_str(), "r"))) {
    return;
  }
  while (fgets(buff, sizeof(buff), in) != NULL) {
    std::string line(&buff[0]);
    text += line;

    //	cout << buff ;
  }
  pclose(in);
//test output vector keys    
  for (string n : keys){
      std::cout << n << std::endl;
  }
  std::cout << "text_area ptr " << text_area << std::endl;
  text_area->setText(text);
  return;
}

// methode config file read
void WebUIApplication::read_input_file(std::string filename) {
  ifstream in(filename);
  assert(in.good());

  std::string line;
  while (getline(in, line)) {
    stringstream line_stream(line);
    std::string key, value_part;
    std::string combined_value;
    line_stream >> key;
    // ignore # and blank lines
    if (key[0] == '#' || key == "") {
      continue;
    }
    while (line_stream >> value_part) {
      combined_value += value_part + " ";
    }
    key_to_command[key] = combined_value;
  }
  // test output line_stream
  // store only the key's for buttontext
  for (auto &&element : key_to_command) {
    keys.push_back(element.first);
  }
      
  }

WApplication *createApplication(const WEnvironment &env) {
  return new WebUIApplication(env);
}
int WebUI::run(int argc, char **argv) {
  /*
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */

  return WRun(argc, argv, &createApplication);
}

WebUI::~WebUI() {}
