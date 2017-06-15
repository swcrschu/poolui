
#pragma once
#include <Wt/WContainerWidget>
#include <map>
#include <string>

#include "web_ui.h"

class iSCSIConfigurator : public Wt::WContainerWidget {
public:
    iSCSIConfigurator (std::map<std::string, std::string>& _ktc, WebUIApplication* ap); 
    virtual ~iSCSIConfigurator ();

private:
    /* data */
  std::map<std::string, std::string> key_to_command;
  WebUIApplication* application = nullptr;

};
