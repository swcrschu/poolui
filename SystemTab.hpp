
#pragma once
#include <Wt/WContainerWidget>
#include <map>
#include <string>

class SystemTab : public Wt::WContainerWidget{
public:
    SystemTab (std::map<std::string,std::string>& _ktc, Wt::WApplication* ap); 
    virtual ~SystemTab ();

private:
    /* data */
    Wt::WContainerWidget* content_System();
    std::map<std::string,std::string> key_to_command;
    Wt::WApplication* application = nullptr;
};
