/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

<<<<<<< HEAD
#include <Wt/WAnchor>
#include <Wt/WApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WContainerWidget>
#include <Wt/WHBoxLayout>
#include <Wt/WLineEdit>
#include <Wt/WLink>
#include <Wt/WMenu>
#include <Wt/WTabWidget>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
=======

#include "web_ui.h"
#include <map>
#include <Wt/WBootstrapTheme>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WLineEdit>
#include <Wt/WLink>
#include <Wt/WText>
#include <Wt/WTextArea>
>>>>>>> fcde08f68468151a2ff40e163e01b38ee4554ef1
#include <Wt/WPushButton>
#include <Wt/WServer>
<<<<<<< HEAD
#include <Wt/WStackedWidget>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <future>
#include <algorithm>
=======
#include <Wt/WHBoxLayout>
>>>>>>> fcde08f68468151a2ff40e163e01b38ee4554ef1
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

// Menue
  Wt::WWidget *content_tab1();
  Wt::WWidget *content_tab2();
  Wt::WWidget *content_tab3();
  Wt::WWidget *content_tab4();
//Comand-Output  
  WTextArea *ta_command_output;
// key value map command file key_to_command
  std::map<string, string> key_to_command;

// read command output line
  void get_command_output(std::string, WTextArea *);
  void read_input_file(std::string filename);    
// use for update_lock and triggerUpdate (Button)
  void fire_and_forget( std::function<void()> f ); 
};

WebUIApplication::WebUIApplication(const WEnvironment &env)
    : WApplication(env) {
      setTitle("poolui");

// css
  Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(this);
  bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
  bootstrapTheme->setResponsive(true);
  setTheme(bootstrapTheme);
  useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css"); 

// needed to load the templates for the objects and make the bootstrap theme  work
  this->messageResourceBundle().use(this->appRoot() + "text");

// to enable threading for the main event loop
  this->enableUpdates(true);

// Config file key_to_command read
  read_input_file("key_to_command");

// menuContainer + menu
  Wt::WContainerWidget *menuContainer = new Wt::WContainerWidget();
  root()->addWidget(menuContainer);

// at the moment not use, because Tabs doesn't support responsive design
//  Wt::WTabWidget *menuTabConatiner = new Wt::WTabWidget();
//  root()->addWidget(menuTabConatiner);
//  menuTabConatiner->addTab(content_tab1(),"rollout", Wt::WTabWidget::PreLoading);
//  menuTabConatiner->addTab(content_tab2(),"administration", Wt::WTabWidget::PreLoading)->setStyleClass("trhead");
//  menuTabConatiner->addTab(content_tab3(),"monitoring");
//  menuTabConatiner->addTab(content_tab4(),"edit");

 Wt::WNavigationBar *navigation = new Wt::WNavigationBar(menuContainer);
 navigation->setTitle("URZ", "http://www.rz.uni-greifswald.de");
 navigation->setResponsive(true);


// output WText menuItem
  Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();
  root()->addWidget(contentsStack);
  contentsStack->addStyleClass("contents");

// Setup a Left-aligned
  Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack, menuContainer);
  navigation->addMenu(leftMenu);

  leftMenu->addItem("rollout", content_tab1());
  leftMenu->addItem("administration",content_tab2());
  leftMenu->addItem("monitoring", content_tab3());
  leftMenu->addItem("edit", content_tab4());

}
//content for a single menu
Wt::WWidget* WebUIApplication::content_tab1(){
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget(); 

  Wt::WText *text = new Wt::WText("Hier entsteht der Inhalt für saemtliche Rollout Transaktionen");
  contentTab->addWidget(text);  

  return contentTab;
}

Wt::WWidget* WebUIApplication::content_tab2(){
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();
//only for bootstrap grid      
  Wt::WContainerWidget *container = new Wt::WContainerWidget();
  container->addStyleClass("container-fluid");
  contentTab->addWidget(container);
  Wt::WContainerWidget *row = new Wt::WContainerWidget();
  row->addStyleClass("row");
  container->addWidget(row);
// button
  Wt::WPushButton *pb1 = new Wt::WPushButton("uptime");
 row->addWidget(pb1);
  
  Wt::WPushButton *pb2 = new Wt::WPushButton("ip-add");
 row->addWidget(pb2);


// action button
  pb1->clicked().connect(std::bind([=]() {
      fire_and_forget(
	  [=](){
	  get_command_output(key_to_command["uptime"], ta_command_output);
	  }
	);
  }));
  
  pb2->clicked().connect(std::bind([=]() {
    fire_and_forget(
      [=]() {
      get_command_output(key_to_command["ip-add"], ta_command_output);
      }
     );
  }));
  

      ta_command_output = new WTextArea();
      contentTab->addWidget(ta_command_output); 
      return contentTab;
}

Wt::WWidget* WebUIApplication::content_tab3(){
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();
      
      Wt::WText *text = new Wt::WText("Hier entsteht der Inhalt für die allgemeine Administration");
      contentTab->addWidget(text);

    
      return contentTab;
}

Wt::WWidget* WebUIApplication::content_tab4(){
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();
      
      Wt::WText *text = new Wt::WText("Hier entsteht der Inhalt für die allgemeine Administration");
      contentTab->addWidget(text);

    
      return contentTab;
}

// To protect common data against simultaneous access from multiple threads
std::mutex g_GetCommandOutputMutex;


// CommandLine to TextAarea
void WebUIApplication::get_command_output(std::string command,
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
  }
  pclose(in);
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
    std::cout << element.first << " " << element.second << endl;
  }
      
  }

//use for update_lock triggerUpdate (Button)
void WebUIApplication::fire_and_forget( std::function<void()> f ){
      std::thread([=](){
// getting update lock
      auto update_lock = this->getUpdateLock();
          f();
              this->triggerUpdate();
                 }).detach();
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
