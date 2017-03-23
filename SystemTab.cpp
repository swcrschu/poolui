#include <Wt/WPushButton>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WMenu>
#include <Wt/WPanel>
#include <Wt/WStackedWidget>

#include "CommandExecutingButton.hpp"
#include "SystemTab.hpp"

using namespace Wt;
using namespace std;

SystemTab::SystemTab( map<string,string>& _key_to_command, WApplication* _ap) {
  application = _ap;
  key_to_command = _key_to_command;
//BoxLayout horizontal
  auto hbox = new Wt::WHBoxLayout();
  this->setLayout(hbox);
  auto subMenuContainer = new Wt::WContainerWidget();
  auto subStacked = new Wt::WStackedWidget();
  auto subMenu = new Wt::WMenu(subStacked,Wt::Vertical,subMenuContainer);
  subMenu->setStyleClass("nav nav-pills nav-stacked");
  subMenu->setWidth(200);
  subMenu->addItem("Serververwaltung", content_System() );

  subMenuContainer->addWidget(subMenu);

  hbox->addWidget(subMenuContainer); 
  hbox->addWidget(subStacked,1);
}

SystemTab::~SystemTab(){
  
}


WContainerWidget* SystemTab::content_System() {
  auto contentTab = new Wt::WContainerWidget();

//Serveradmin central panel
  auto pServerVerwaltung = new Wt::WPanel();
  pServerVerwaltung->setWidth(300);
  pServerVerwaltung->addStyleClass("centered-example");
  pServerVerwaltung->setTitle("Dienste");
  pServerVerwaltung->setCentralWidget(new Wt::WText("Methoden zum Abfragen der Dienste"));
  contentTab->addWidget(pServerVerwaltung); 
// cmd output mouseover
  auto label = new WLabel();
  // result cmd output textarea
  auto ta_command_output = new WTextArea();
 
#if 0
  Wt::WImage *image = new Wt::WImage(Wt::WLink("ducky.jpg"));
  image->setStyleClass("img-responsive");
  contentTab->addWidget(image);
  
#endif

//CommandExecutingButton
  auto pb1 = new CommandExecutingButton(
      "ip-add", label, key_to_command["ip-add"], ta_command_output, application);
  contentTab->addWidget(pb1);

  auto pb2 = new CommandExecutingButton(
      "uptime", label, key_to_command["uptime"], ta_command_output, application);
  contentTab->addWidget(pb2);
  
  auto pb3 = new CommandExecutingButton(
      "session", label, key_to_command["session"], ta_command_output, application);
  contentTab->addWidget(pb3);
  
  auto pb4 = new CommandExecutingButton(
      "volume", label, key_to_command["volume"], ta_command_output, application);
   auto pb5 = new CommandExecutingButton(
      "ldapsearch", label, key_to_command["ldap"], ta_command_output, application);
  contentTab->addWidget(pb4);
  contentTab->addWidget(pb5);
  contentTab->addWidget(label);
  contentTab->addWidget(ta_command_output);

  return contentTab;
}
  
