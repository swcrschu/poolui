#include <Wt/WPushButton>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WLabel>
#include <Wt/WMenu>
#include <Wt/WPanel>
#include <Wt/WStackedWidget>

#include "CommandExecutingButton.hpp"
#include "MonitoringTab.hpp"

using namespace Wt;
using namespace std;

MonitoringTab::MonitoringTab( map<string,string>& _key_to_command, WApplication* _ap) {
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
  subMenu->addItem("Monitoring", content_Monitoring() );

  subMenuContainer->addWidget(subMenu);

  hbox->addWidget(subMenuContainer); 
  hbox->addWidget(subStacked,1);
}

MonitoringTab::~MonitoringTab(){
  
}


WContainerWidget* MonitoringTab::content_Monitoring() {
  auto contentTab = new Wt::WContainerWidget();
  auto text = new Wt::WText("monitoring");
  contentTab->addWidget(text);


  return contentTab;

}


