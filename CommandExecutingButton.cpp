#include "CommandExecutingButton.hpp"
#include "utility.hpp"


using namespace Wt;
// global CommandLine to TextArea
void get_command_output(std::string command, WTextArea* text_area, WebUIApplication* application) {
  
  auto& client = application->get_ssh_client();
  std::string answer =
      client.execute_command(command.c_str());
  std::cout << "got answer : " << answer << std::endl;
  text_area->setText(answer);

  return;
}

/*Intialisierung CommandExecutingButton  */
CommandExecutingButton::CommandExecutingButton(
    const Wt::WString buttonText, WLabel* label, std::string _command,
    Wt::WTextArea* _ta_command_output, WebUIApplication* _application,
    Wt::WContainerWidget* parent)
    : Wt::WPushButton(parent) {
  setText(buttonText);
  command = _command;
  application = _application;
  mouseMoved().connect(std::bind([=]() { label->setText(command); }));
  mouseWentOut().connect(std::bind([=]() { label->setText(""); }));

  this->clicked().connect(std::bind([=]() {
    fire_and_forget([=]() { get_command_output(command, _ta_command_output,application); },
		    application);
  }));
}

// setter methoden
void CommandExecutingButton::setCommand(std::string _command) {
  command = _command;
}


// versuch den Befehl bei mouseover im Widget WLabel auszugeben
void CommandExecutingButton::command_preview(Wt::WString _command,
					     Wt::WLabel* wlabel_command) {
  wlabel_command->setText(_command);
  return;
}
