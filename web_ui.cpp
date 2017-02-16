/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include <Wt/WAnchor>
#include <Wt/WApplication>
#include <Wt/WBootstrapTheme>
#include <Wt/WBorderLayout>
#include <Wt/WContainerWidget>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WImage>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WLink>
#include <Wt/WMenu>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPanel>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WPushButton>
#include <Wt/WServer>
#include <Wt/WStackedWidget>
#include <Wt/WString>
#include <Wt/WTabWidget>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <algorithm>
#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "CommandExecutingButton.hpp"
#include "utility.hpp"
#include "web_ui.h"

#include "ssh.h"

using namespace Wt;
using namespace std;

// c++0x only, for std::bind

class WebUIApplication : public WApplication {
 public:
  WebUIApplication(const WEnvironment &env);

 private:
  // LayoutContainer
  Wt::WWidget *content_west();
  Wt::WWidget *content_east();
  Wt::WWidget *content_south();

  // Menue
  Wt::WWidget *content_tab1();
  Wt::WWidget *content_tab2();
  Wt::WWidget *content_tab3();
  Wt::WWidget *content_tab4();
  // command-Output mouseover
  Wt::WLabel *label;
  // Comand-Output Area
  Wt::WTextArea *ta_command_output;

  //  key value map command file key_to_command
  std::map<string, string> key_to_command;
  void read_input_file(std::string filename);
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

  // needed to load the templates for the objects and make the bootstrap theme
  // work
  this->messageResourceBundle().use(this->appRoot() + "text");

  // to enable threading for the main event loop
  this->enableUpdates(true);

  // Config file key_to_command read
  read_input_file("key_to_command");
  
  Wt::WContainerWidget *mainContainer = new Wt::WContainerWidget();
  root()->addWidget(mainContainer);
  //  mainContainer->setStyleClass("alert-info");



  // Menu
  // menuContainer + menu
  Wt::WContainerWidget *menuContainer = new Wt::WContainerWidget();
  Wt::WNavigationBar *navigation = new Wt::WNavigationBar(menuContainer);
  navigation->setTitle("URZ", "http://www.rz.uni-greifswald.de");
  navigation->setResponsive(true);

  // output WText menuItem
  Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();
  
  // Setup a Left-aligned
  Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack, menuContainer);
  navigation->addMenu(leftMenu);

  leftMenu->addItem("rollout", content_tab1());
  leftMenu->addItem("administration", content_tab2());
  leftMenu->addItem("monitoring", content_tab3());
  leftMenu->addItem("edit", content_tab4());


  // Borderlayout
  Wt::WBorderLayout *layout = new Wt::WBorderLayout();
  mainContainer->setLayout(layout);
  layout->addWidget(navigation, Wt::WBorderLayout::North);
  layout->addWidget(content_west(), Wt::WBorderLayout::West);
  layout->addWidget(new Wt::WText("East"), Wt::WBorderLayout::East);
  // item->setStyleClass("alert-warning");
  layout->addWidget(content_south(), Wt::WBorderLayout::South);
  layout->addWidget(contentsStack, Wt::WBorderLayout::Center);
}


Wt::WWidget *WebUIApplication::content_west() {
  Wt::WContainerWidget *ContentContainer = new Wt::WContainerWidget();
  Wt::WText *text = new Wt::WText("container west");
  ContentContainer->addWidget(text);
  return ContentContainer;
}
Wt::WWidget *WebUIApplication::content_east() {
  Wt::WContainerWidget *ContentContainer = new Wt::WContainerWidget();
  Wt::WText *text = new Wt::WText("east");
  ContentContainer->addWidget(text);
  return ContentContainer;
}

Wt::WWidget *WebUIApplication::content_south() {
  Wt::WContainerWidget *ContentContainer = new Wt::WContainerWidget();
  Wt::WText *text = new Wt::WText("bottom");
  ContentContainer->addWidget(text);
  return ContentContainer;
}

// content for a single menu
Wt::WWidget *WebUIApplication::content_tab1() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();

  Wt::WText *text = new Wt::WText(
      "Hier entsteht der Inhalt für saemtliche Rollout Transaktionen");
  contentTab->addWidget(text);

  return contentTab;
}

Wt::WWidget *WebUIApplication::content_tab2() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();
  Wt::WPanel *panel = new Wt::WPanel();
  //panel->resize(400,400);
  panel->setWidth(300);
  panel->addStyleClass("centered-example");
  panel->setTitle("Dienste");
  panel->setCentralWidget(new Wt::WText("Methoden zum Abfragen der Dienste"));
  contentTab->addWidget(panel); 
  // cmd output mouseover
  label = new WLabel();
  // result cmd output textarea
  ta_command_output = new WTextArea();

  //  Wt::WImage *image = new Wt::WImage(Wt::WLink("ducky.jpg"));
  //  image->setStyleClass("img-responsive");
  //  contentTab->addWidget(image);
  
  // neuer CommandExecutingButton
  CommandExecutingButton *pb1 = new CommandExecutingButton(
      "ip-add", label, key_to_command["ip-add"], ta_command_output, this);
  contentTab->addWidget(pb1);

  CommandExecutingButton *pb2 = new CommandExecutingButton(
      "uptime", label, key_to_command["uptime"], ta_command_output, this);
  contentTab->addWidget(pb2);
  contentTab->addWidget(label);
  contentTab->addWidget(ta_command_output);

  return contentTab;
}

Wt::WWidget *WebUIApplication::content_tab3() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();

  Wt::WText *text = new Wt::WText("monitoring");
  contentTab->addWidget(text);

  return contentTab;
}

Wt::WWidget *WebUIApplication::content_tab4() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();

  Wt::WText *text = new Wt::WText("file editor");
  contentTab->addWidget(text);

  return contentTab;
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
  //  for (auto &&element : key_to_command) {
  //  std::cout << element.first << " " << element.second << endl;
  //}
}

WApplication *createApplication(const WEnvironment &env) {
  //authenticate_with_ssh( "incubus", "muh", "141.53.32.45" );
 // std::string answer = execute_command_with_ssh( "incubus", "141.53.32.45", "ls ~" );

//  std::cout << "got answer : " << answer << std::endl;

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
