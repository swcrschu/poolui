/*
 * Copyright (C) 2008 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include <Wt/WAnchor>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WBootstrapTheme>
#include <Wt/WBorderLayout>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WImage>
#include <Wt/WLabel>
#include <Wt/WRadioButton>
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
#include <vector>
#include <cstddef>
#include <future>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include "CommandExecutingButton.hpp"
#include "utility.hpp"
#include "web_ui.h"

#include "ssh.h"
#include "SystemTab.hpp"
#include "ThinclientTab.hpp"
#include "MonitoringTab.hpp"

using namespace Wt;
using namespace std;

// c++0x only, for std::bind

class WebUIApplication : public WApplication {
 public:
  WebUIApplication(const WEnvironment &env);

 private:
// LayoutContainer
  Wt::WWidget *content_south();

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
  Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget();
  
// Setup a Left-aligned top menue
  Wt::WMenu *topMenu = new Wt::WMenu(contentsStack, menuContainer);
  topMenu->addItem("System", new SystemTab(key_to_command,this));
  topMenu->addItem("ThinClient infrastructure", new ThinclientTab(key_to_command,this));
  topMenu->addItem("monitoring",new MonitoringTab(key_to_command,this));
  navigation->addMenu(topMenu);

// Borderlayout
  Wt::WBorderLayout *layout = new Wt::WBorderLayout();
  mainContainer->setLayout(layout);
  layout->addWidget(navigation, Wt::WBorderLayout::North);
// item->setStyleClass("alert-warning");
  layout->addWidget(content_south(), Wt::WBorderLayout::South);
  layout->addWidget(contentsStack, Wt::WBorderLayout::Center);

}

Wt::WWidget *WebUIApplication::content_south() {
  Wt::WContainerWidget *ContentContainer = new Wt::WContainerWidget();
  Wt::WText *text = new Wt::WText("bottom");
  ContentContainer->addWidget(text);
  return ContentContainer;
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
}


WApplication *createApplication(const WEnvironment &env) {

#if 0 
  authenticate_with_ssh( "schuldtc", "schuldtc", "141.53.7.32" );
  std::string answer = execute_command_with_ssh( "schuldtc", "141.53.32.45", "ls ~" );

  std::cout << "got answer : " << answer << std::endl;
#endif

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
