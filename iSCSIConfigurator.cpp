
#include <Wt/WPanel>
#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WTemplate>

#include <algorithm>
#include "iSCSIConfigurator.hpp"
#include "CommandExecutingButton.hpp"
#include "utility.hpp"

using namespace std;
using namespace Wt;

 
struct TargetEntry {
  std::string iqn;
  std::string lun;
  std::string lunpath;
  std::string username;
  std::string password;
};


WPanel* new_target( TargetEntry& entry, int target_id , std::string folder_str, WebUIApplication* application ){
  
  //needed to delete the right folder 
  std::string id = to_string(target_id);
  std::string folder_string = folder_str;
 
  std::cout << folder_string << std::endl; 


  auto panel = new WPanel();
  panel->setTitle(folder_string); 
  panel->setStyleClass("panel-primary"); 
  // XML template dhcp
  auto tDHCP = new Wt::WTemplate(Wt::WString::tr("iscsi-template"));
  tDHCP->addFunction("id", &Wt::WTemplate::Functions::id);
 
  auto container = new WContainerWidget();
  auto wtIqn = new WText(entry.iqn);
  auto wtLun = new WText(entry.lun);
  auto wtLunpath = new WText(entry.lunpath);
  auto wtUsername = new WText(entry.username);
  auto wtPassword = new WText(entry.password);
 
  WPushButton* pbTargetdel = nullptr;
  pbTargetdel = new WPushButton("Delete");
  pbTargetdel->setStyleClass("btn-danger");
  

  tDHCP->bindString("target","Target:");
  tDHCP->bindWidget("wtIqn", wtIqn);
  tDHCP->bindString("lun", "Lun:");
  tDHCP->bindWidget("wtLun",wtLun);
  tDHCP->bindString("lunpath", "Path:");
  tDHCP->bindWidget("wtLunpath", wtLunpath);
  tDHCP->bindString("auth", "User:");
  tDHCP->bindWidget("wtUsername", wtUsername);
  tDHCP->bindString("password", "Password:");
  tDHCP->bindWidget("wtPassword", wtPassword);
  tDHCP->bindWidget("pbTargetdel", pbTargetdel);

  container->addWidget(tDHCP);  
  panel->setCentralWidget(container);

  // button. // TODO
  pbTargetdel->clicked().connect(std::bind([=]() {
    std::string command_string; 
    command_string = "bash setup/deleteISCSITARGETS.sh "s + folder_string; 
    
    std::cout << command_string << std::endl;

    auto& client = application->get_ssh_client();
    client.execute_command(command_string); 				 

	}));
  

  return panel;
}

TargetEntry parse_target_entry( std::string target_entry )
{
//  cout << __PRETTY_FUNCTION__ << endl;
  stringstream in( target_entry );
  std::string line;

  TargetEntry entry;

  while( getline( in, line ) ){
    stringstream in_line( line ) ;
    std::string word;
    in_line >> word;
//    cout << line << endl;
//    cout << "word " << word << endl;

    if ( word == "" || word[0] == '#' ) {
      continue;
    }

    if ( word == "Target" ) {
      std::string target;
      in_line >> target;
      entry.iqn = target;
      continue;
    }

    if ( word == "LUN" ) {
      std::string _lun;
      std::string _lunpath;
      in_line >> _lun >> _lunpath;
      entry.lun = _lun;
      entry.lunpath = _lunpath;
    }

    if ( word == "InComingUser" ) {
      std::string _username;
      std::string _password;
      in_line >> _username >> _password;
      entry.username = _username;
      entry.password = _password;
    }

  }
  return entry;
}

iSCSIConfigurator::iSCSIConfigurator(map<string, string> &_key_to_command,
			     WebUIApplication *_ap) {
  application = _ap;
  key_to_command = _key_to_command;
  std::cout << "iSCSI class" << std::endl;
#if 1
  WContainerWidget* new_target(TargetEntry& entry, int target_id, std::string folder_str, WebUIApplication* application);
 
//  auto testText = new WText("buttons for iSCSI control ");
//  this->addWidget(testText);
  
  
  int number_of_targets;
  auto& client = application->get_ssh_client();
  std::string number_of_targets_str = client.execute_command("cd setup/targets/ && ls | wc -l" );
  // TODO error handling
  number_of_targets = stoi(number_of_targets_str);
  
  //needed to remove the right folder
  for (int i = 0; i < number_of_targets; ++i){
    std::string folder_str = client.execute_command(
	"cd setup/targets/ && ls | awk -F _ '{ print $1 }' | sed -n "s + to_string(i+1) +"p").c_str();
    //filter carraige return \r utility.hpp
    folder_str = filter_newline(folder_str);
    
	
    std::string target_entry = client.execute_command(
	  "cd setup/targets && find "s + folder_str + "_target -type f | xargs cat").c_str();
    auto entry = parse_target_entry( target_entry );


    this->addWidget( new_target(entry, i+1, folder_str, application) ) ;
  }
#endif
}

iSCSIConfigurator::~iSCSIConfigurator() {}


