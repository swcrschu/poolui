
#pragma once
#include <Wt/WButtonGroup>
#include <Wt/WContainerWidget>
#include <Wt/WSelectionBox>
#include <Wt/WSpinBox>
#include <Wt/WString>
#include <Wt/WText>

#include <map>
#include <string>

class ThinclientTab : public Wt::WContainerWidget {
 public:
  ThinclientTab(std::map<std::string, std::string>& _ktc, Wt::WApplication* ap);
  virtual ~ThinclientTab();

 private:
  /* data */
  Wt::WContainerWidget* content_ThinClient();
  std::map<std::string, std::string> key_to_command;
  Wt::WApplication* application = nullptr;

  CommandExecutingButton* pbCheck;

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
  CommandExecutingButton* ldapsearch;
  Wt::WText* target_name_box;

  void update_text_box() {
    // get button label from checkedButton
    std::string button_label = "";
    if (group_pbOS->checkedButton() != nullptr) {
      auto button = group_pbOS->checkedButton();
      button_label = button->text().toUTF8();
    }
    // output update helper
    target_name_box->setText("iqn." + date->text() + ".de.uni-greifswald:tci." +
			     ip->text() + ":" + hardware->text() + ":" +
			     button_label + "." + mac->text() + ".img" + " " +
			     user->text() + " " + passwort->text() + " " +
			     volume->valueText() + " " + image->valueText() +
			     " " + dest->valueText());

    // ldapsearch output + mac filter + in one Line
    ldapsearch->setCommand((key_to_command["ldap"] + mac->text()).toUTF8() +
			   " | perl -p00e 's/\r?\n //g'");

    // comand to Controller
    // TODO own class?
    pbCheck->setCommand(
	(key_to_command["controller"] + target_name_box->text().toUTF8()));
  }
};
