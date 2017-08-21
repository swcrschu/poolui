
#include <Wt/WBreak>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WMenu>
#include <Wt/WPanel>
#include <Wt/WStackedWidget>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WVBoxLayout>
#include <Wt/WLineEdit>

#include <algorithm>
#include "CommandExecutingButton.hpp"
#include "ThinclientAdd.hpp"
#include "utility.hpp"

using namespace std;
using namespace Wt;

#if 1

struct ThinClient {
 std::string mac;
 std::string hostname;
  };
#endif

WPanel* Thinclientinfo(ThinClient& entry, int session_id, WebUIApplication* application) {
   
  //infoPanel add ThinClient
  auto thinPanel = new Wt::WPanel();
  thinPanel->setWidth(500);
  auto tThinclient = new Wt::WTemplate(Wt::WString::tr("thinclients-panel"));
  auto containerThinclient = new WContainerWidget();
  auto wtMac = new WText(entry.mac);
  auto wtHostname = new WText(entry.hostname);
  auto pbThinclientcreate = new WPushButton("create");
  tThinclient->addFunction("id", &Wt::WTemplate::Functions::id);
  tThinclient->bindString("mac","MAC: ");
  tThinclient->bindWidget("wtMac", wtMac);
  tThinclient->bindString("hostname", "Hostname: ");
  tThinclient->bindWidget("wtHostname",wtHostname);
  tThinclient->bindWidget("pbThinclientcreate", pbThinclientcreate);
  containerThinclient->addWidget(tThinclient);
  thinPanel->setCentralWidget(containerThinclient);
  thinPanel->setTitle(entry.hostname);

  pbThinclientcreate->clicked().connect(std::bind([=]() {
    std::string command_string; 
    command_string = "bash setup/thinclients/autorollout.sh " +  entry.mac + " " +  entry.hostname; 
    
    std::cout << command_string << std::endl;

   auto& client = application->get_ssh_client();
   client.execute_command(command_string); 				 

	}));
  return thinPanel;
}

ThinClient parse_thinclient_entry(std::string thinclient_entry,
				 int number_of_clients) {

  ThinClient entry;
  int clients = number_of_clients;
  stringstream in(thinclient_entry);
  std::string line;
  
  while (getline(in, line)) {
    stringstream in_line(line);
    std::string mac;
    in_line >> mac;
    entry.mac = mac;
    std::string hostname;
    in_line >> hostname;
    entry.hostname = hostname;

   }

  return entry;
}

ThinclientAdd::ThinclientAdd(map<string, string>& _key_to_command, WebUIApplication* _ap) {
  application = _ap;
  key_to_command = _key_to_command;
  
  std::cout << "ThinclientAdd" << std::endl;
  WContainerWidget* Thinclientinfo(ThinClient & entry, int session_id, WebUIApplication* application);
  
  auto& client = application->get_ssh_client();
  std::string number_of_thinclients_str = client.execute_command(
      "cd setup/thinclients && cat inventar | wc -l");
  int number_of_thinclients = stoi(number_of_thinclients_str);
  auto containerThinclientadd = new Wt::WContainerWidget();
  auto panelThinclientAdd = new Wt::WPanel();
  panelThinclientAdd->setCentralWidget(containerThinclientadd);
  panelThinclientAdd->setTitle("Thin Client zum Inventar hinzufuegen");
  panelThinclientAdd->setStyleClass("panel-primary");
  panelThinclientAdd->setWidth(500);
  auto tTarget = new Wt::WTemplate(Wt::WString::tr("Thinclientadd-template"));
  tTarget->addFunction("id", &Wt::WTemplate::Functions::id);

  auto wlMac = new Wt::WLineEdit();
  wlMac->setTextSize(15);
  wlMac->setInputMask("HH:HH:HH:HH:HH:HH;_");
  tTarget->bindString("mac", "Mac:");
  tTarget->bindWidget("wlMac", wlMac);

  // thinClient Hardware
  auto wlHostname = new Wt::WLineEdit();
  wlHostname->setTextSize(15);
  wlHostname->setInputMask("xxxxxxxxxxxx;_");
  tTarget->bindString("hostname", "Hostname: ");
  tTarget->bindWidget("wlHostname", wlHostname);
#if 0  
  auto pbThinclientadd = new CommandExecutingButton("add", label, key_to_command["thinclientadd"],
				 ta_command_output, application);
#endif
  auto pbThinclientadd = new WPushButton("add");
  tTarget->bindWidget("pbThinclientadd",pbThinclientadd);

  containerThinclientadd->addWidget(tTarget);
  this->addWidget(panelThinclientAdd);
 
  pbThinclientadd->clicked().connect(std::bind([=]() {
	auto& client = application->get_ssh_client();
	std::string thinclientadd_str;
	thinclientadd_str = wlMac->text().toUTF8() + " " + wlHostname->text().toUTF8();
	std::string command_string = thinclientadd_str;
	std::cout << "mac + hostname" << command_string.c_str() << std::endl;
	
	client.execute_command(
	"cd setup/thinclients && echo '"s + command_string.c_str() + "' >> inventar");

  }));


  
  for (int i = 0; i < number_of_thinclients; ++i) {
    std::string thinclient_str = client.execute_command(
	("cd setup/thinclients && cat inventar | sed -n "s + to_string(i+1) + "p").c_str());

// filter carraige return \r utility.hpp
   thinclient_str = filter_newline(thinclient_str);

    auto entry = parse_thinclient_entry(thinclient_str, number_of_thinclients);

   
    //active session Panel
    this->addWidget(Thinclientinfo(entry, i + 1, application));
  }

   }

ThinclientAdd::~ThinclientAdd() {}
