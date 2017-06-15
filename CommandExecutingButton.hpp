#include <Wt/WContainerWidget>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WTextArea>

#include "web_ui.h"

/*Class CommandExecutingButton inheritance from WPushButton */
class CommandExecutingButton : public Wt::WPushButton {
 public:
  CommandExecutingButton(const Wt::WString buttonText, Wt::WLabel* label,
			 std::string _command,
			 Wt::WTextArea* _ta_command_output,
			 WebUIApplication* _application,
			 Wt::WContainerWidget* parent = nullptr);
  // ldapsearch add the mac to ldap command
  //setter methoden
  void setCommand(std::string _command);


 private:
  Wt::WString* buttonText;
  std::string command;
  WebUIApplication* application = nullptr;
  void command_preview(Wt::WString, Wt::WLabel*);
};
