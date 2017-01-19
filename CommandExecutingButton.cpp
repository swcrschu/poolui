#include "CommandExecutingButton.hpp"
#include "utility.hpp"

using namespace Wt;
//global CommandLine to TextAarea
void get_command_output(std::string command,
					  WTextArea *text_area) {
  // for owning a mutex or several mutexes for the duration of a scoped block
  std::string text;
  FILE *in;
  char buff[512];
  if (!(in = popen(command.c_str(), "r"))) {
    return;
  }
  while (fgets(buff, sizeof(buff), in) != NULL) {
    std::string line(&buff[0]);
    text += line;
  }
  pclose(in);
  std::cout << "text_area ptr " << text_area << std::endl;
  text_area->setText(text);
  return;
}


/*Intialisierung CommandExecutingButton  */
CommandExecutingButton::CommandExecutingButton(const Wt::WString buttonText,
			   WLabel* label,
			   std::string _command,
			   Wt::WTextArea* _ta_command_output,
			   Wt::WApplication* _application,
			   Wt::WContainerWidget *parent)
    : Wt::WPushButton(parent) {
  setText(buttonText);
  command = _command;
  application = _application;
  mouseMoved().connect(std::bind([=]() { label->setText(command); }));
  mouseWentOut().connect(std::bind([=]() { label->setText(""); }));

  this->clicked().connect(std::bind([=]() {
    fire_and_forget([=]() {
      get_command_output(command,_ta_command_output);
    }, application);
  }));
}

void CommandExecutingButton::MacheWasNeues() {}
// versuch den Befehl bei mouseover im Widget WLabel auszugeben
void CommandExecutingButton::command_preview(Wt::WString _command, Wt::WLabel* wlabel_command){
 wlabel_command->setText(_command);
 return;
}


