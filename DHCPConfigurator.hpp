
#pragma once
#include <Wt/WContainerWidget>
#include <map>
#include <string>
#include "web_ui.h"

class DHCPConfigurator : public Wt::WContainerWidget {
public:
    DHCPConfigurator (std::map<std::string, std::string>& _ktc, WebUIApplication* ap); 
    virtual ~DHCPConfigurator ();

private:
    /* data */
  std::map<std::string, std::string> key_to_command;
  WebUIApplication* application = nullptr;
};
