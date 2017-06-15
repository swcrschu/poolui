#include <Wt/WHBoxLayout>
#include <Wt/WApplication>
#include <Wt/WLabel>
#include <Wt/WMenu>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WVBoxLayout>
#include <algorithm>

#include "CommandExecutingButton.hpp"
#include "SystemTab.hpp"

using namespace Wt;
using namespace std;

SystemTab::SystemTab(map<string, string>& _key_to_command, WebUIApplication* _ap) {
  application = _ap;
  key_to_command = _key_to_command;
  // BoxLayout horizontal
  auto hbox = new Wt::WHBoxLayout();
  this->setLayout(hbox);
  auto subMenuContainer = new Wt::WContainerWidget();
  auto subStacked = new Wt::WStackedWidget();
  auto subMenu = new Wt::WMenu(subStacked, Wt::Vertical, subMenuContainer);
  subMenu->setStyleClass("nav nav-pills nav-stacked");
  subMenu->setWidth(200);
  subMenu->addItem("Serververwaltung", content_System());

  subMenuContainer->addWidget(subMenu);

  hbox->addWidget(subMenuContainer);
  hbox->addWidget(subStacked, 1);
}

SystemTab::~SystemTab() {}

WContainerWidget* SystemTab::content_System() {
  
  auto contentTab = new Wt::WContainerWidget();

  // Serveradmin central panel
  auto pServerVerwaltung = new Wt::WPanel();
  auto containerSystem = new Wt::WContainerWidget();
  pServerVerwaltung->setWidth(400);
  pServerVerwaltung->addStyleClass("centered-example panel-primary");
  pServerVerwaltung->setTitle("System");
  pServerVerwaltung->setCentralWidget(containerSystem);
  contentTab->addWidget(pServerVerwaltung);
 
  auto& client = application->get_ssh_client();
  auto dhcpd_pid_str = client.execute_command("sudo pidof dhcpd");

  auto pDHCPVerwaltung = new Wt::WPanel();
  auto containerDHCP = new Wt::WContainerWidget();
  pDHCPVerwaltung->setWidth(400);
  pDHCPVerwaltung->setTitle("DHCP Pid: " + dhcpd_pid_str);
  pDHCPVerwaltung->setCentralWidget(containerDHCP);
  contentTab->addWidget(pDHCPVerwaltung);

  if (dhcpd_pid_str == "") {
    pDHCPVerwaltung->addStyleClass("centered-example panel-danger");

  }

  else {
    pDHCPVerwaltung->addStyleClass("centered-example panel-success");
  }

  auto iscsi_pid_str =client.execute_command("sudo pidof ietd");


  auto pISCSIVerwaltung = new Wt::WPanel();
  auto containerISCSI = new Wt::WContainerWidget();
  pISCSIVerwaltung->setWidth(400);
  pISCSIVerwaltung->setTitle("iSCSI Pid: " + iscsi_pid_str);
  pISCSIVerwaltung->setCentralWidget(containerISCSI);
  contentTab->addWidget(pISCSIVerwaltung);
  
  
  
 if (iscsi_pid_str == "") { 
    pISCSIVerwaltung->addStyleClass("centered-example panel-danger");
 }

 else {  
    pISCSIVerwaltung->addStyleClass("centered-example panel-success");
  }
  
   auto tftp_pid_str =client.execute_command("sudo pidof in.tftpd");


  auto pTFTPVerwaltung = new Wt::WPanel();
  auto containerTFTP = new Wt::WContainerWidget();
  pTFTPVerwaltung->setWidth(400);
  pTFTPVerwaltung->setTitle("tFTP-Server Pid: " + tftp_pid_str);
  pTFTPVerwaltung->setCentralWidget(containerTFTP);
  contentTab->addWidget(pTFTPVerwaltung);
  
  
  
 if (tftp_pid_str == "") { 
    pTFTPVerwaltung->addStyleClass("centered-example panel-danger");
 }

 else {  
    pTFTPVerwaltung->addStyleClass("centered-example panel-success");
  }
  
  
  // cmd output mouseover
  auto label = new WLabel();
  // result cmd output textarea
  auto ta_command_output = new WTextArea();

#if 0
  Wt::WImage *image = new Wt::WImage(Wt::WLink("ducky.jpg"));
  image->setStyleClass("img-responsive");
  contentTab->addWidget(image);

#endif
  //application refresh()
  auto pbRefreshApp = new Wt::WPushButton("refresh app");

  pbRefreshApp->clicked().connect(std::bind([=]() {
 
  // 	Wt::WApplication::refresh();
  //TODO session refresh	
  // https://www.webtoolkit.eu/wt/doc/reference/html/overview.html#config_session
  // https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WApplication.html#a02b9d8aa1b6c2d0dc9edc1b9c63f86dc

	}));
  containerSystem->addWidget(pbRefreshApp);

  // CommandExecutingButton
  auto pb1 =
      new CommandExecutingButton("ip-add", label, key_to_command["ip-add"],
				 ta_command_output, application);
  containerSystem->addWidget(pb1);

  auto pb2 =
      new CommandExecutingButton("uptime", label, key_to_command["uptime"],
				 ta_command_output, application);
  containerSystem->addWidget(pb2);

  auto pb3 = new CommandExecutingButton("restart", label,
					key_to_command["iscsirestart"],
					ta_command_output, application);
  containerISCSI->addWidget(pb3);

  auto pb4 =
      new CommandExecutingButton("volume", label, key_to_command["volume"],
				 ta_command_output, application);
  //containerISCSI->addWidget(pb4);

  auto pb5 =
      new CommandExecutingButton("ldapsearch", label, key_to_command["ldap"],
				 ta_command_output, application);
  //  contentTab->addWidget(pb5);

  auto pb6 = new CommandExecutingButton("restart", label,
					key_to_command["dhcprestart"],
					ta_command_output, application);
  containerDHCP->addWidget(pb6);

  auto pb7 =
      new CommandExecutingButton("status", label, key_to_command["dhcpstatus"],
				 ta_command_output, application);
  containerDHCP->addWidget(pb7);

  auto pb8 = new CommandExecutingButton("create dhcpd.conf", label,
					key_to_command["dhcpregenerate"],
					ta_command_output, application);
  containerDHCP->addWidget(pb8);

  auto pb9 = new CommandExecutingButton("create ietd.conf", label,
					key_to_command["iscsiregenerate"],
					ta_command_output, application);
  containerISCSI->addWidget(pb9);

  auto pb10 =
      new CommandExecutingButton("session", label, key_to_command["session"],
				 ta_command_output, application);
//  containerISCSI->addWidget(pb10);

  auto pb11 =
      new CommandExecutingButton("updates", label, key_to_command["sysupdate"],
				 ta_command_output, application);
  //  containerSystem->addWidget(pb11);
  auto pb12 =
      new CommandExecutingButton("stop", label, key_to_command["dhcpstop"],
				 ta_command_output, application);
  containerDHCP->addWidget(pb12);

   auto pb13 =
      new CommandExecutingButton("stop", label, key_to_command["iscsistop"],
				 ta_command_output, application);
  containerISCSI->addWidget(pb13);

  contentTab->addWidget(label);
  contentTab->addWidget(ta_command_output);

  return contentTab;
}
