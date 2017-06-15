
#pragma once
#include <Wt/WContainerWidget>
#include <map>
#include <string>

class ThinclientAdd : public Wt::WContainerWidget {
public:
    ThinclientAdd (std::map<std::string, std::string>& _ktc, WebUIApplication* ap); 
    virtual ~ThinclientAdd ();

private:
    /* data */
  std::map<std::string, std::string> key_to_command;
  WebUIApplication* application = nullptr;
#if 0
  Wt::WLineEdit* hostname;
  Wt::WLineEdit* mac;

  std::string thinclient_name_command;
  void update_text_box() {
       // output update helper
    thinclient_name_command =
	mac->text().toUTF8() + " " + hostname->text().toUTF8();

    std::string command_string = thinclient_name_command;
    pbThinclientadd->setCommand(key_to_command["thinclientadd"] + command_string.c_str());
  }

#endif


};
