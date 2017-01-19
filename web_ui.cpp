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
#include <Wt/WLabel>
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
#include <Wt/WString>
#include <Wt/WTabWidget>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WImage>
#include <algorithm>
#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "web_ui.h"
#include "CommandExecutingButton.hpp"
#include "utility.hpp"

using namespace Wt;
using namespace std;
// c++0x only, for std::bind

class WebUIApplication : public WApplication {
 public:
  WebUIApplication(const WEnvironment &env);

 private:
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
  /*
  Wt::WContainerWidget mainContainer = new Wt::WContainerWidget();
  mainContainer->setHeight(800);
  maincontainer->setStyleClass("yellow-box");

  Wt::WBorderLayout *layout = new Wt::WBorderLayout();
  mainContainer->setLayout(layout);
  layout->addWidget(new Wt::WText("West-side is best"),
  Wt::WBorderLayout::West);
  layout->addWidget(new Wt::WText("East-side is best"),
  Wt::WBorderLayout::East);
  layout->addWidget(new Wt::WText("Center"), Wt::WBorderLayout::Center);
  mainContainer->setLayout(layout, Wt::AlignTop | Wt::AlignJustify);
  */
  Wt::WContainerWidget *mainContainer = new Wt::WContainerWidget();
  root()->addWidget(mainContainer);
  mainContainer->setStyleClass("alert-info");

  Wt::WBorderLayout *layout = new Wt::WBorderLayout();
  mainContainer->setLayout(layout);

  // Menu
  // menuContainer + menu
  Wt::WContainerWidget *menuContainer = new Wt::WContainerWidget();
  Wt::WNavigationBar *navigation = new Wt::WNavigationBar(menuContainer);
  navigation->setTitle("URZ", "http://www.rz.uni-greifswald.de");
  navigation->setResponsive(true);

  // output WText menuItem
  Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();
  // root()->addWidget(contentsStack);
  // contentsStack->addStyleClass("contents");

  // Setup a Left-aligned
  Wt::WMenu *leftMenu = new Wt::WMenu(contentsStack, menuContainer);
  navigation->addMenu(leftMenu);

  leftMenu->addItem("rollout", content_tab1());
  leftMenu->addItem("administration", content_tab2());
  leftMenu->addItem("monitoring", content_tab3());
  leftMenu->addItem("edit", content_tab4());

  // Borderlayout

  layout->addWidget(navigation, Wt::WBorderLayout::North);
  layout->addWidget(new Wt::WText("West"), Wt::WBorderLayout::West);
  layout->addWidget(new Wt::WText("East"), Wt::WBorderLayout::East);
  // item->setStyleClass("alert-warning");
  layout->addWidget(new Wt::WText("South"), Wt::WBorderLayout::South);
  layout->addWidget(contentsStack, Wt::WBorderLayout::Center);
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

  label = new WLabel();
  //cmd output mouseover

//result cmd output textarea
  ta_command_output = new WTextArea();
  
//  Wt::WImage *image = new Wt::WImage(Wt::WLink("ducky.jpg"));
//  image->setStyleClass("img-responsive");                                   
//  contentTab->addWidget(image); 
  
  // neuer CommandExecutingButton
  CommandExecutingButton *pb1 = new CommandExecutingButton("ip-add", label,key_to_command["ip-add"],ta_command_output, this);
  contentTab->addWidget(pb1);
  

  CommandExecutingButton *pb2 = new CommandExecutingButton("ip-add", label, key_to_command["uptime"], ta_command_output, this);
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
