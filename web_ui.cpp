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
#include <Wt/WButtonGroup>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WInPlaceEdit>
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

using namespace Wt;
using namespace std;

// c++0x only, for std::bind

class WebUIApplication : public WApplication {
 public:
  WebUIApplication(const WEnvironment &env);

 private:
// LayoutContainer
  Wt::WWidget *content_south();

// Menue
  Wt::WWidget *mTab2();
  Wt::WWidget *mTab3();

//content
  Wt::WWidget *content_ThinClient();
// command-Output mouseover
  Wt::WLabel *label;
// Comand-Output Area
  Wt::WTextArea *ta_command_output;
  
//panels 
  Wt::WPanel *ptargetCreate; 

//  key value map command file key_to_command
  std::map<string, string> key_to_command;
  void read_input_file(std::string filename);

  void get_command_save(std::string command);

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
  
  // Setup a Left-aligned top menue
  Wt::WMenu *topMenu = new Wt::WMenu(contentsStack, menuContainer);

  topMenu->addItem("System", new SystemTab(key_to_command,this));
  topMenu->addItem("ThinClient infrastructure", mTab2());
  topMenu->addItem("monitoring", mTab3());
//  topMenu->addItem("edit", content_tab4());
//  topMenu->addItem("Target erstellen", content_tab5());

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


//Menue ThinClient
Wt::WWidget *WebUIApplication::mTab2() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget(); 
//BoxLayout horizontal two columns
  Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
  contentTab->setLayout(hbox);
//subMenue column 1
  Wt::WContainerWidget *subMenuContainer = new Wt::WContainerWidget();
// create a stac2
  Wt::WStackedWidget *subStacked = new Wt::WStackedWidget();
  Wt::WMenu *subMenu = new Wt::WMenu(subStacked,Wt::Vertical,subMenuContainer);
 
 
  subMenu->setStyleClass("nav nav-pills nav-stacked");
  subMenu->setWidth(200);

//subMenu->addItem(new Wt::WMenuItem("Serververwaltung", pServerVerwaltung));
  subMenu->addItem("Target erstellen", content_ThinClient());
  subMenu->addItem("Target bearbeiten", new Wt::WText("test"));

  subMenuContainer->addWidget(subMenu);

  hbox->addWidget(subMenuContainer); 
  hbox->addWidget(subStacked,1);
  


 return contentTab;

}

//monitoring content
Wt::WWidget *WebUIApplication::mTab3() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();

  Wt::WText *text = new Wt::WText("monitoring");
  contentTab->addWidget(text);

  return contentTab;
}



//Content ThinClient
Wt::WWidget *WebUIApplication::content_ThinClient() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();
  Wt::WContainerWidget *contentPanel = new Wt::WContainerWidget();
//input panel Target create
  ptargetCreate = new Wt::WPanel();
  ptargetCreate->setWidth(600);
  ptargetCreate->addStyleClass("centered-example");
  ptargetCreate->setTitle("Target erstellen");

//radiobutton
  Wt::WContainerWidget *containerOS = new Wt::WContainerWidget();
  Wt::WLabel *labelOS = new Wt::WLabel("Betriebssystem: ");
  labelOS->addStyleClass("h5");
  containerOS->addWidget(labelOS);
//TODO isChecked() ?
  auto buttonLinux  = new Wt::WRadioButton("Linux", containerOS);
  auto buttonWindows  = new Wt::WRadioButton("Windows", containerOS);
  Wt::WButtonGroup *group = new Wt::WButtonGroup(containerOS);
  group->addButton(buttonLinux);
  group->addButton(buttonWindows);
  contentPanel->addWidget(containerOS);
  
//mac 
  Wt::WContainerWidget *containerMAC = new Wt::WContainerWidget();
  Wt::WLabel *labelMAC = new Wt::WLabel("MAC-Adresse: ");
  labelMAC->addStyleClass("h5");
  containerMAC->addWidget(labelMAC);
  Wt::WInPlaceEdit *mac = new Wt::WInPlaceEdit("", containerMAC);
//  mac->setWidth(500);
  mac->setButtonsEnabled(true);
  mac->setPlaceholderText("AA:BB: und soweiter");
//  containerMAC->addWidget(mac);  
  contentPanel->addWidget(containerMAC); 

//VLAN
//TODO VLAN Auslesen
  Wt::WContainerWidget *containerVLAN = new Wt::WContainerWidget();
  Wt::WLabel *labelVLAN = new Wt::WLabel("VLAN: ");
  labelVLAN->addStyleClass("h5");
  containerVLAN->addWidget(labelVLAN);
  Wt::WComboBox *cbVLAN = new Wt::WComboBox();
  cbVLAN->setWidth(200);
  cbVLAN->addItem("VLAN 7");
  cbVLAN->addItem("VLAN 6");
  containerVLAN->addWidget(cbVLAN);
  contentPanel->addWidget(containerVLAN);  
 
  ptargetCreate->setCentralWidget(contentPanel); 
  contentTab->addWidget(ptargetCreate); 

  auto target_name_box = new WText("test");
  
  contentPanel->addWidget(target_name_box);

  
  group->checkedChanged().connect(
      std::bind([=] (Wt::WRadioButton *selection) {
	  target_name_box->setText( selection->text() );
      }, 
      std::placeholders::_1
      )
  );



#if 0
  Wt::WText *out = new Wt::WText(container);

  group->checkedChanged().connect(std::bind([=] (Wt::WRadioButton *selection) {
  Wt::WString text;
  switch (group->id(selection)) {
    case 1: text = Wt::WString::fromUTF8("You checked button {0}.")
          .arg(group->checkedId());
    break;

    case 2: text = Wt::WString::fromUTF8("You selected button {1}.")
          .arg(group->checkedId());
    break;
    if (group->id(selection) == 4)
      text = Wt::WString::fromUTF8("That's what I expected!");

    out->setText(Wt::WString::fromUTF8("<p>") + text + "</p>");
    }))
  };

#endif
 
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
void WebUIApplication::get_command_save( std::string command ) {
  std::vector<string> answer(2);
//TODO siehe pushback answer
#if 1  
  for(size_t i=0; i < answer.size(); i++){
    answer[i] = execute_command_with_ssh( "schuldtc", "141.53.7.32", command.c_str() );
  }
#endif

  for(size_t i=0; i < answer.size(); i++){
    std::cout << answer[i] << std::endl;
  }

  return;
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
