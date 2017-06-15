#include <Wt/WPanel>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WTemplate>

#include <algorithm>
#include "DHCPConfigurator.hpp"
#include "CommandExecutingButton.hpp"

using namespace std;
using namespace Wt;


struct HostEntry {
  std::string hostname;
  std::string mac;
  std::string root_path;
  std::string username;
  std::string password;
};



WPanel* new_host( HostEntry& entry, int host_id, WebUIApplication* application ){
  auto panel = new WPanel();
  panel->setTitle(entry.hostname); 
  panel->setStyleClass("panel-primary");
  auto container = new WContainerWidget();
  auto wtHostname = new WText(entry.hostname);
  auto wtMac = new WText(entry.mac);
  auto wtrp = new WText(entry.root_path);
  auto wtUsername = new WText(entry.username);
  auto wtPassword = new WText(entry.password);
  
  // cmd output mouseover
  auto label = new WLabel();
  // result cmd output textarea
  auto ta_command_output = new WTextArea();
 
  panel->setCentralWidget(container);
  WPushButton* pbDelete = nullptr;
  pbDelete = new WPushButton("Delete");
  pbDelete->setStyleClass("btn-danger");  
  // XML template dhcp
  auto tDHCP = new Wt::WTemplate(Wt::WString::tr("DHCP-template"));
  tDHCP->addFunction("id", &Wt::WTemplate::Functions::id);
  
  tDHCP->bindString("hostname","Hostname:");
  tDHCP->bindWidget("hostname", wtHostname);
  tDHCP->bindString("mac", "MAC-Adresse:");
  tDHCP->bindWidget("macadress",wtMac);
  tDHCP->bindString("rpath", "root-path:");
  tDHCP->bindWidget("root-path", wtrp);
  tDHCP->bindString("user", "Username:");
  tDHCP->bindWidget("username", wtUsername);
  tDHCP->bindString("pass", "Password");
  tDHCP->bindWidget("password", wtPassword);
  tDHCP->bindWidget("pbDelete", pbDelete);

  container->addWidget(tDHCP);  

  // button. // TODO
  pbDelete->clicked().connect(std::bind([=]() {
    std::string command_string; 
    command_string = "bash setup/deleteDHCP.sh " +  entry.hostname + " " + entry.mac ; 
    
    std::cout << command_string << std::endl;

   auto& client = application->get_ssh_client();
   client.execute_command(
      command_string); 				 

	}));

  return panel;
}



HostEntry parse_host_entry( std::string host_entry )
{
//  cout << __PRETTY_FUNCTION__ << endl;
  stringstream in( host_entry );
  std::string line;

  HostEntry entry;

  while( getline( in, line ) ){
    stringstream in_line( line ) ;
    std::string word;
    in_line >> word;
//    cout << line << endl;
//    cout << "word " << word << endl;

    if ( word == "" || word[0] == '#' ) {
      continue;
    }


    if ( word == "host" ) {
      std::string host_name;
      in_line >> host_name;
      entry.hostname = host_name;
      continue;
    }

    if ( word == "hardware" ) {
      std::string dont_care;
      std::string mac;
      in_line >> dont_care >> mac;
      entry.mac = mac;
    }

    if ( word == "option") {
      std::string _option_w2;
      std::string _rootpath;
      std::string _username;
      std::string _password;
     // read the second word
      in_line >> _option_w2;
      std::cout << "word2 " <<  _option_w2 << std::endl;

        if ( _option_w2 == "root-path")
	{
	// read the third word and return  
	  in_line >> _rootpath; 
	  entry.root_path = _rootpath; 
	}

	if ( _option_w2 == "gpxe.username")
	{
	  in_line >> _username; 
	  entry.username = _username; 
	}

	if ( _option_w2 == "gpxe.password")
	{
	  in_line >> _password; 
	  entry.password = _password; 
	}

    }



  }
  return entry;
}


DHCPConfigurator::DHCPConfigurator(map<string, string> &_key_to_command,
			     WebUIApplication *_ap) {
  application = _ap;
  key_to_command = _key_to_command;
  WContainerWidget* new_host(HostEntry& entry, int host_id, WebUIApplication* application);
 
  int number_of_hosts;
  auto& client = application->get_ssh_client();
  std::string number_of_hosts_str = client.execute_command("cd setup/dhcp/generatefiles/02_hosts && ls | wc -l" );
  // TODO error handling
  number_of_hosts = stoi(number_of_hosts_str);

  for (int i = 0; i < number_of_hosts; ++i){
    std::string host_entry = client.execute_command(
	  "cd setup/dhcp/generatefiles/02_hosts && ls | sed -n "s + to_string(i+1) + "p | xargs cat").c_str();
    auto entry = parse_host_entry( host_entry );

    this->addWidget( new_host(entry, i+1, application) ) ;
  }
}

DHCPConfigurator::~DHCPConfigurator() {}


