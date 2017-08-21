#include <Wt/WBreak>
#include <Wt/WButtonGroup>
#include <Wt/WComboBox>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WInPlaceEdit>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WMenu>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WRadioButton>
#include <Wt/WSelectionBox>
#include <Wt/WSpinBox>
#include <Wt/WStackedWidget>
#include <Wt/WString>
#include <Wt/WTemplate>
#include <Wt/WTextArea>
#include <Wt/WVBoxLayout>

#include <algorithm>
#include "CommandExecutingButton.hpp"
#include "ThinclientTab.hpp"
#include "DHCPConfigurator.hpp"
#include "iSCSIConfigurator.hpp"
#include "iSCSIInfo.hpp"
#include "ThinclientAdd.hpp"
#include "utility.hpp"
#include "web_ui.h"


using namespace Wt;
using namespace std;


// preparation ssh output
std::vector<std::string> to_lines(std::string text_block) {
  std::vector<std::string> ret;
  stringstream stream(text_block);
  string line;
  while (getline(stream, line)) {
    line = filter_character(line);
    if (line == "") continue;
    if (line == " ") continue;
    ret.push_back(line);
  }
  return ret;
}
// ThinclientTab ia a WContainerWidget
ThinclientTab::ThinclientTab(map<string, string> &_key_to_command,
			     WebUIApplication *_ap) {
  application = _ap;
  key_to_command = _key_to_command;
  // BoxLayout horizontal two columns
  Wt::WHBoxLayout *hbox = new Wt::WHBoxLayout();
  this->setLayout(hbox);
  // subMenue column 1
  Wt::WContainerWidget *subMenuContainer = new Wt::WContainerWidget();
  // create a stac2
  Wt::WStackedWidget *subStacked = new Wt::WStackedWidget();
  Wt::WMenu *subMenu =
      new Wt::WMenu(subStacked, Wt::Vertical, subMenuContainer);

  subMenu->setStyleClass("nav nav-pills nav-stacked");
  subMenu->setWidth(200);

  // subMenu->addItem(new Wt::WMenuItem("Serververwaltung", pServerVerwaltung));
  subMenu->addItem("iSCSI Info", new iSCSIInfo(key_to_command,application));
  subMenu->addItem("Target erstellen", content_ThinClient());
  subMenu->addItem("Target loeschen¶", new iSCSIConfigurator(key_to_command,application));
  subMenu->addItem("Thin Client hinzufuegen", new ThinclientAdd(key_to_command,application));

  subMenuContainer->addWidget(subMenu);

  hbox->addWidget(subMenuContainer);
  hbox->addWidget(subStacked, 1);
}

ThinclientTab::~ThinclientTab() {}


// Content ThinClient
WContainerWidget *ThinclientTab::content_ThinClient() {
  auto contentTab = new Wt::WContainerWidget();

  // XML template target
  auto tTarget = new Wt::WTemplate(Wt::WString::tr("Target-template"));
  tTarget->addFunction("id", &Wt::WTemplate::Functions::id);

  // cmd output mouseover
  auto label = new WLabel();
  // result cmd output textarea
  auto ta_command_output = new WTextArea();
  tTarget->bindWidget("ta_command_output", ta_command_output);

  ldapsearch =
      new CommandExecutingButton("ldapsearch", label, key_to_command["ldap"],
				 ta_command_output, application);

  // Panel
  auto contentPanel = new Wt::WContainerWidget();
  auto ptargetCreate = new Wt::WPanel();
  ptargetCreate->setStyleClass("panel-primary");
  ptargetCreate->setWidth(1024);
  ptargetCreate->addStyleClass("centered-example");
  ptargetCreate->setTitle("Target erstellen");
  // OS
  auto containerOS = new Wt::WContainerWidget();
  auto buttonLinux = new Wt::WRadioButton("Linux", containerOS);
  auto buttonWindows = new Wt::WRadioButton("Windows", containerOS);
  auto buttonDual = new Wt::WRadioButton("Dualboot", containerOS);

  auto group = new Wt::WButtonGroup(containerOS);
  group->addButton(buttonLinux);
  group->addButton(buttonWindows);
  group->addButton(buttonDual);
  group->setSelectedButtonIndex(0);
  tTarget->bindString("label", "Betriebssystem: ");
  tTarget->bindWidget("os", buttonLinux);
  tTarget->bindWidget("os2", buttonWindows);
  tTarget->bindWidget("dual", buttonDual);

  // iqnDate
  auto wlDate = new Wt::WLineEdit("2015-10");
  wlDate->setTextSize(15);
  wlDate->setInputMask("9999-99;_");
  tTarget->bindString("iqn", "IQN-Datum (yyyy-mm):");
  tTarget->bindWidget("wlDate", wlDate);

  // MAC input Mask
  // character
  // https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#acfffadaf2a0409a79dbb3f69c5ec0666
  auto wlMac = new Wt::WLineEdit("F4:4D:30:60.E9:62");
  auto infoMac = new WText("vollst&auml;ndig angeben");
  wlMac->setTextSize(15);
  wlMac->setInputMask("HH:HH:HH:HH:HH:HH;_");
  tTarget->bindString("mac", "Mac-Adresse:");
  tTarget->bindWidget("wlMac", wlMac);
  tTarget->bindWidget("ldapsearch", ldapsearch);
  tTarget->bindWidget("infoMac", infoMac);
  // Serverip

  auto wlIP = new Wt::WLineEdit("10.10.0.1");
  wlIP->setTextSize(15);
  wlIP->setInputMask("009.009.009.009;_");
  tTarget->bindString("ip", "Server-IP:");
  tTarget->bindWidget("wlIP", wlIP);

  // thinClient Hardware
  auto wlHardware = new Wt::WLineEdit("nuci5");
  auto infoHardware = new WText("max: 12 Zeichen");
  wlHardware->setTextSize(15);
  wlHardware->setInputMask("xxxxxxxxxxxx;_");
  tTarget->bindString("hardware", "ThinClient System: ");
  tTarget->bindWidget("wlHardware", wlHardware);
  tTarget->bindWidget("infoHardware", infoHardware);

  // user password
  // TODO Random Button and Validierung
  //
  auto wlUser = new Wt::WLineEdit("uhrz03");
  auto wlPass = new Wt::WLineEdit("n3v9l7xspace");
  auto infoAuth = new WText("8-12 Zeichen");
  wlUser->setTextSize(15);
  wlUser->setInputMask("NNNNNnnnnnnn;_");
  wlPass->setTextSize(15);
  wlPass->setInputMask("XXXXXXXXxxxx;_");
  tTarget->bindString("user", "User / Passwort: ");
  tTarget->bindWidget("wlUser", wlUser);
  tTarget->bindWidget("wlPass", wlPass);
  tTarget->bindWidget("infoAuth", infoAuth);

  // LUN
  auto pbLunNew = new Wt::WPushButton("Neu");
  tTarget->bindString("lun", "Image:");
  tTarget->bindWidget("pbLunNew", pbLunNew);
  // Volume Size
  auto sbVolumeSize = new Wt::WSpinBox();
  auto infoVolumeSize = new Wt::WText("in GByte max: 100GB");
  sbVolumeSize->setRange(0, 100);
  sbVolumeSize->setValue(0);
  sbVolumeSize->setSingleStep(1.0);

  // empty string because we see the var name
  tTarget->bindString("volume", " ");
  tTarget->bindString("sbVolumeSize", "");
  tTarget->bindString("infoVolumeSize", "");

  auto cbImage = new WComboBox();
  tTarget->bindString("image", "Image");
  tTarget->bindWidget("cbImage", cbImage);
  cbImage->addItem("0");
  auto pbImageSearch = new WPushButton("Suche");
  tTarget->bindString("suche", "Suche");
  tTarget->bindWidget("pbImageSearch", pbImageSearch);

  pbLunNew->clicked().connect(std::bind([=]() {

    // display SpinBox VolumeSize
    tTarget->bindString("volume", "Volume Size: ");
    tTarget->bindWidget("sbVolumeSize", sbVolumeSize);
    tTarget->bindWidget("infoVolumeSize", infoVolumeSize);
    pbImageSearch->setEnabled(false);
    pbImageSearch->setStyleClass("btn btn-warning");
  }));

  // Image Search
  pbImageSearch->clicked().connect(std::bind([=]() {
    cbImage->clear();

    auto& client = application->get_ssh_client();
    std::string answer = client.execute_command(key_to_command["image"]);

    auto lines = to_lines(answer);
    for (auto &line : lines) {
      cbImage->addItem(line);
      // disable the New button
    }

    pbLunNew->setEnabled(false);
    pbLunNew->setStyleClass("btn btn-warning");

  }));

#if 1  // Destination
  auto sbDestination = new WSelectionBox();
  //temp
  sbDestination->addItem("/mnt/images");
  tTarget->bindWidget("sbDestination", sbDestination);
  auto pbMount = new WPushButton("Zielpfad");
  tTarget->bindWidget("pbMount", pbMount);

  pbMount->clicked().connect(std::bind([=]() {
    sbDestination->clear();

    auto& client = application->get_ssh_client();
    std::string answer = client.execute_command(key_to_command["mount"]);

    auto lines = to_lines(answer);
    for (auto &line : lines) {
      sbDestination->addItem(line);
    }

  }));

  tTarget->bindString("zielpfad", "Zielpfad: ");
#endif
  // check and send to controller
  pbCheck =
      new CommandExecutingButton("Check", label, key_to_command["controller"],
				 ta_command_output, application);

 
  pbCreate = 
    new CommandExecutingButton("iSCSI", label,key_to_command["controller"],
				  ta_command_output, application);
  pbDHCP = 
    new CommandExecutingButton("DHCP", label,key_to_command["controller"],
				  ta_command_output, application);

  pbCheck->setStyleClass("btn-primary");
  tTarget->bindString("check", "Check: ");
  tTarget->bindWidget("pbCheck", pbCheck);
  
  pbCreate->setStyleClass("btn-warning");
  pbDHCP->setStyleClass("btn-warning");
  tTarget->bindString("create", "Create: ");
  tTarget->bindWidget("pbCreate", pbCreate);
  tTarget->bindWidget("pbDHCP", pbDHCP);


   // update_helper
  auto wtTargetNameBox = new WText("auto Text");
  // instance
  // input
  group_pbOS = group;
  date = wlDate;
  mac = wlMac;
  ip = wlIP;
  hardware = wlHardware;
  user = wlUser;
  passwort = wlPass;
  volume = sbVolumeSize;
  image = cbImage;
  dest = sbDestination;
  // output
  target_name_box = wtTargetNameBox;

  // Event update target_name_box
  group->checkedChanged().connect(
      std::bind([=](Wt::WRadioButton *selection) { update_text_box(); },
		std::placeholders::_1));

  wlDate->changed().connect(std::bind([=]() { update_text_box(); }));

  wlMac->changed().connect(std::bind([=]() { update_text_box(); }));

  wlIP->changed().connect(std::bind([=]() { update_text_box(); }));

  wlHardware->changed().connect(std::bind([=]() { update_text_box(); }));

  wlUser->changed().connect(std::bind([=]() { update_text_box(); }));

  wlPass->changed().connect(std::bind([=]() { update_text_box(); }));

  sbVolumeSize->valueChanged().connect(std::bind([=]() { update_text_box(); }));

  cbImage->activated().connect(std::bind([=]() { update_text_box(); }));

  sbDestination->activated().connect(std::bind([=]() { update_text_box(); }));

  contentPanel->addWidget(tTarget);
  ptargetCreate->setCentralWidget(contentPanel);
  contentPanel->addWidget(wtTargetNameBox);
  contentTab->addWidget(ptargetCreate);
  return contentTab;
}
