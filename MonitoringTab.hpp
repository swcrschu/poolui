
#pragma once
#include <Wt/WContainerWidget>
#include <map>
#include <string>

class MonitoringTab : public Wt::WContainerWidget{
public:
    MonitoringTab (std::map<std::string,std::string>& _ktc, Wt::WApplication* ap); 
    virtual ~MonitoringTab ();

private:
    /* data */
    Wt::WContainerWidget* content_Monitoring();
    std::map<std::string,std::string> key_to_command;
    Wt::WApplication* application = nullptr;
};
