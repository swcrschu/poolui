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
#if 1  
//BoxLayout horizontal two columns
  Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
  this->setLayout(hbox);
//subMenue column 1
  Wt::WContainerWidget *subMenuContainer = new Wt::WContainerWidget();
// create a stac2
  Wt::WStackedWidget *subStacked = new Wt::WStackedWidget();
  Wt::WMenu *subMenu = new Wt::WMenu(subStacked,Wt::Vertical,subMenuContainer);
 
 
  subMenu->setStyleClass("nav nav-pills nav-stacked");
  subMenu->setWidth(200);

//subMenu->addItem(new Wt::WMenuItem("Serververwaltung", pServerVerwaltung));
  subMenu->addItem("Serververwaltung", content_System() );
//  subMenu->addItem("Target erstellen", ptargetCreate );
//  subMenu->addItem("Target bearbeiten");

  subMenuContainer->addWidget(subMenu);

  hbox->addWidget(subMenuContainer); 
  hbox->addWidget(subStacked,1);
#endif
}

SystemTab::~SystemTab(){
  
}


WContainerWidget* SystemTab::content_System() {
  Wt::WContainerWidget *contentTab = new Wt::WContainerWidget();

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
  CommandExecutingButton *pb1 = new CommandExecutingButton(
      "ip-add", label, key_to_command["ip-add"], ta_command_output, application);
  contentTab->addWidget(pb1);

  CommandExecutingButton *pb2 = new CommandExecutingButton(
      "uptime", label, key_to_command["uptime"], ta_command_output, application);
  contentTab->addWidget(pb2);
  
  CommandExecutingButton *pb3 = new CommandExecutingButton(
      "session", label, key_to_command["session"], ta_command_output, application);
  contentTab->addWidget(pb3);
  
  CommandExecutingButton *pb4 = new CommandExecutingButton(
      "volume", label, key_to_command["volume"], ta_command_output, application);
  contentTab->addWidget(pb4);

#if 0
  Wt::WPushButton *pb5 = new Wt::WPushButton("Aufruf get_command_save");
    pb5->clicked().connect(std::bind([=]() {
     get_command_save(key_to_command["session"]);   
    
  }));
  contentTab->addWidget(pb5);
#endif
  contentTab->addWidget(label);
  contentTab->addWidget(ta_command_output);

  return contentTab;
}
  
