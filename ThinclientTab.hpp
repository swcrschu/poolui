
#pragma once
#include <Wt/WButtonGroup>
#include <Wt/WComboBox>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WSelectionBox>
#include <Wt/WSpinBox>
#include <Wt/WString>
#include <Wt/WText>

#include <map>

class ThinclientTab : public Wt::WContainerWidget {
 public:
  ThinclientTab(std::map<std::string, std::string>& _ktc, WebUIApplication* ap);
  virtual ~ThinclientTab();

   private:
  /* data */
  Wt::WContainerWidget* content_ThinClient();
  std::map<std::string, std::string> key_to_command;
  WebUIApplication* application = nullptr;

  CommandExecutingButton* pbCheck;
  CommandExecutingButton* pbCreate;
  CommandExecutingButton* ldapsearch;
  CommandExecutingButton* pbDHCP;

#if 0
  //number of mounts to inital the destionation radio buttons
  std::int mount;
  mount = setCommand((key_to_command[mount] + " | wc -l"));
#endif

  // Input for update_helper
  Wt::WButtonGroup* group_pbOS;
  Wt::WLineEdit* date;
  Wt::WLineEdit* mac;
  Wt::WLineEdit* ip;
  Wt::WLineEdit* hardware;
  Wt::WLineEdit* user;
  Wt::WLineEdit* passwort;
  Wt::WSpinBox* volume;
  Wt::WComboBox* image;
  Wt::WSelectionBox* dest;
  // output
  Wt::WText* target_name_box;
  std::string target_name_command;
  std::string dhcp_string;

  void update_text_box() {
    // get button label from checkedButton
    std::string button_label = "";
    if (group_pbOS->checkedButton() != nullptr) {
      auto button = group_pbOS->checkedButton();
      button_label = button->text().toUTF8();
    }
    // output update helper
    target_name_command =
	"iqn." + date->text().toUTF8() + ".de.uni-greifswald:tci." +
	ip->text().toUTF8() + ":" + hardware->text().toUTF8() + ":" +
	button_label + "." + mac->text().toUTF8() + ".img" + " " +
	volume->valueText().toUTF8() + " " + image->currentText().toUTF8() +
	" " + dest->valueText().toUTF8() + " " + user->text().toUTF8() + " " +
	passwort->text().toUTF8();

    dhcp_string = hardware->text().toUTF8() + " " + mac->text().toUTF8();
    // ldapsearch output + mac filter + in one Line
    ldapsearch->setCommand((key_to_command["ldap"] + mac->text()).toUTF8() +
			   " | perl -p00e 's/\r?\n //g'");

    target_name_box->setText(target_name_command);
    
    std::string command_string = target_name_command;
    pbCheck->setCommand(key_to_command["controller"] + "check" + " " + command_string.c_str());
    pbCreate->setCommand(key_to_command["controller"] + "create" + " " + command_string.c_str());
    
    std::string dhcp_command = dhcp_string;
    pbDHCP->setCommand(key_to_command["controller"] + "dhcp" + " " + command_string.c_str() + " " + dhcp_command.c_str());
  }
};
