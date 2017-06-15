
#pragma once
#include <Wt/WContainerWidget>
#include <map>
#include <string>
#include "web_ui.h"


class SystemTab : public Wt::WContainerWidget{
public:
    SystemTab (std::map<std::string,std::string>& _ktc, WebUIApplication* ap); 
    virtual ~SystemTab ();

private:
    /* data */
    Wt::WContainerWidget* content_System();
    std::map<std::string,std::string> key_to_command;
    WebUIApplication * application = nullptr;
};
