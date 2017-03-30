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

#include "CommandExecutingButton.hpp"
#include "ThinclientTab.hpp"
#include "ssh.h"

using namespace Wt;
using namespace std;

// ThinclientTab ia a WContainerWidget
ThinclientTab::ThinclientTab(map<string, string> &_key_to_command,
			     WApplication *_ap) {
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
  subMenu->addItem("Target erstellen", content_ThinClient());
  subMenu->addItem("Target bearbeiten", new Wt::WText("test"));

  subMenuContainer->addWidget(subMenu);

  hbox->addWidget(subMenuContainer);
  hbox->addWidget(subStacked, 1);
}

ThinclientTab::~ThinclientTab() {}
/// hier kommt die Maus doxygen Kommandos
// Content ThinClient
WContainerWidget *ThinclientTab::content_ThinClient() {
  auto contentTab = new Wt::WContainerWidget();

  // XML one for all template
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
  //  group->setSelectedButtonIndex(0);
  tTarget->bindString("label", "Betriebssystem: ");
  tTarget->bindWidget("os", buttonLinux);
  tTarget->bindWidget("os2", buttonWindows);
  tTarget->bindWidget("dual", buttonDual);

  // iqnDate
  auto wlDate = new Wt::WLineEdit();
  wlDate->setTextSize(15);
  wlDate->setInputMask("9999-99;_");
  tTarget->bindString("iqn", "IQN-Datum (yyyy-mm):");
  tTarget->bindWidget("wlDate", wlDate);

  // MAC input Mask
  // character
  // https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WLineEdit.html#acfffadaf2a0409a79dbb3f69c5ec0666
  auto wlMac = new Wt::WLineEdit();
  auto infoMac = new WText("vollst&auml;ndig angeben");
  wlMac->setTextSize(15);
  wlMac->setInputMask("HH:HH:HH:HH:HH:HH;_");
  tTarget->bindString("mac", "Mac-Adresse:");
  tTarget->bindWidget("wlMac", wlMac);
  tTarget->bindWidget("ldapsearch", ldapsearch);
  tTarget->bindWidget("infoMac", infoMac);
  // Serverip

  auto wlIP = new Wt::WLineEdit();
  wlIP->setTextSize(15);
  wlIP->setInputMask("009.009.009.009;_");
  tTarget->bindString("ip", "Server-IP:");
  tTarget->bindWidget("wlIP", wlIP);

  // thinClient Hardware
  auto wlHardware = new Wt::WLineEdit();
  auto infoHardware = new WText("max: 12 Zeichen");
  wlHardware->setTextSize(15);
  wlHardware->setInputMask("xxxxxxxxxxxx;_");
  tTarget->bindString("hardware", "ThinClient System: ");
  tTarget->bindWidget("wlHardware", wlHardware);
  tTarget->bindWidget("infoHardware", infoHardware);

  // user password
  // TODO Random Button and Validierung
  //
  auto wlPass = new Wt::WLineEdit();
  auto wlUser = new Wt::WLineEdit();
  auto infoAuth = new WText("min: 8 Zeichen");
  wlUser->setTextSize(15);
  wlUser->setInputMask("NNNNNnnnnnnn;_");
  wlPass->setTextSize(15);
  wlPass->setInputMask("XXXXXXXXxxxx;_");
  tTarget->bindString("user", "User / Passwort: ");
  tTarget->bindWidget("wlUser", wlUser);
  tTarget->bindWidget("wlPass", wlPass);

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
    std::string answerImage = execute_command_with_ssh(
	"schuldtc", "141.53.7.32", key_to_command["image"].c_str());

    std::string line2;
    stringstream answer_stream2(answerImage);
    while (getline(answer_stream2, line2)) {
      if (line2 == "") continue;
      cbImage->addItem(line2);
      // disable the New button
      pbLunNew->setEnabled(false);
      pbLunNew->setStyleClass("btn btn-warning");
    }

  }));

#if 1  // Destination
  auto sbDestination = new WSelectionBox();
  tTarget->bindWidget("sbDestination", sbDestination);
  auto pbMount = new WPushButton("Zielpfad");
  tTarget->bindWidget("pbMount", pbMount);

  pbMount->clicked().connect(std::bind([=]() {

    sbDestination->clear();
    std::string answer = execute_command_with_ssh(
	"schuldtc", "141.53.7.32", key_to_command["mount"].c_str());

    std::string line;
    stringstream answer_stream(answer);
    while (getline(answer_stream, line)) {
      if (line == "") continue;
      sbDestination->addItem(line);
    }

  }));

  tTarget->bindString("zielpfad", "Zielpfad: ");
#endif
  // check and send to controller
  pbCheck =
      new CommandExecutingButton("Check", label, key_to_command["controller"],
				 ta_command_output, application);

  pbCheck->setStyleClass("btn-primary");
  tTarget->bindString("check", "Check: ");
  tTarget->bindWidget("pbCheck", pbCheck);

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
