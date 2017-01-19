#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WTextArea>

/*Class CommandExecutingButton inheritance from WPushButton */
class CommandExecutingButton : public Wt::WPushButton {
 public:
  CommandExecutingButton(const Wt::WString buttonText,
      Wt::WLabel* label,
      std::string _command,
      Wt::WTextArea* _ta_command_output,
      Wt::WApplication* _application,
      Wt::WContainerWidget *parent = nullptr);
  void MacheWasNeues();

 private:
  Wt::WString *buttonText;
  std::string command;
  Wt::WApplication* application;
  void command_preview(Wt::WString,Wt::WLabel *);
};


