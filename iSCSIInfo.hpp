
#pragma once
#include <Wt/WContainerWidget>
#include <map>
#include <string>

#include "web_ui.h"

class iSCSIInfo : public Wt::WContainerWidget {
public:
    iSCSIInfo (std::map<std::string, std::string>& _ktc, WebUIApplication* ap); 
    virtual ~iSCSIInfo ();

private:
    /* data */
  std::map<std::string, std::string> key_to_command;
  WebUIApplication* application = nullptr;

};
