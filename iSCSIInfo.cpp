
#include <Wt/WBreak>
#include <Wt/WHBoxLayout>
#include <Wt/WLabel>
#include <Wt/WMenu>
#include <Wt/WPanel>
#include <Wt/WStackedWidget>
#include <Wt/WTemplate>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

#include <algorithm>
#include "CommandExecutingButton.hpp"
#include "iSCSIInfo.hpp"
#include "utility.hpp"

using namespace std;
using namespace Wt;

struct SessionEntry {
  std::string tid;
  std::string iqn;
  std::string sid;
  std::string initiator;
  std::string cid;
  std::string ip;
  std::string state;
};

WPanel* sessioninfo(SessionEntry& entry, int session_id, WebUIApplication* application) {
  auto container = new WContainerWidget();
  auto panel = new WPanel();
  panel->setStyleClass("panel-success");
  auto wtIqn = new WText(entry.iqn);
  auto wtSid = new WText(entry.sid);
  auto wtInitiator = new WText(entry.initiator);
  auto wtCid = new WText(entry.cid);
  auto wtIp = new WText(entry.ip);
  auto wtState = new WText(entry.state);
  container->addWidget(wtIqn);
  container->addWidget(new WBreak());
  //  container->addWidget(wtSid);
  //  container->addWidget(new WBreak());
  container->addWidget(wtInitiator);
  container->addWidget(new WBreak());
  //  container->addWidget(wtCid);
  //  container->addWidget(new WBreak());
  container->addWidget(wtIp);
  container->addWidget(new WBreak());
  container->addWidget(wtState);
  // needed to delete the right folder
  std::string id = to_string(session_id);
  std::string title_str = entry.tid + " " + entry.state;
  panel->setTitle(title_str);
  panel->setCentralWidget(container);
  auto pbShutdown = new WPushButton("Shutdown");
  container->addWidget(new WBreak());
  container->addWidget(pbShutdown);
  // button. // TODO
  pbShutdown->clicked().connect(std::bind([=]() {
    std::string command_string;
    command_string = "bash setup/clientshutdown.sh " + entry.ip;

    std::cout << command_string << std::endl;

    auto& client = application->get_ssh_client();
    client.execute_command(command_string);

  }));
  return panel;
}

SessionEntry parse_session_entry(std::string session_entry,
				 int number_of_targets) {
  SessionEntry entry;
  int targets = number_of_targets;
  stringstream in(session_entry);
  std::string line;

  while (getline(in, line)) {
    stringstream in_line(line);
    std::string word;
    in_line >> word;

    // loop is needed to find the exactly tid:1 or tid:2
    for (int i = targets; i > 0; i--) {
      if (word == "tid:"s + to_string(i)) {
	std::string iqn;
	entry.tid = word;
	in_line >> iqn;
	entry.iqn = iqn;

	std::string sid;
	in_line >> sid;
	entry.sid = sid;

	std::string initiator;
	std::string cid;
	in_line >> initiator >> cid;
	entry.initiator = initiator;
	entry.cid = cid;

	std::string ip;
	std::string state;
	in_line >> ip >> state;
	entry.ip = ip;
	entry.state = state;
	continue;
      }
    }
  }

  return entry;
}

iSCSIInfo::iSCSIInfo(map<string, string>& _key_to_command, WebUIApplication* _ap) {
  application = _ap;
  key_to_command = _key_to_command;

  
  std::cout << "iSCSIINFO" << std::endl;
  WContainerWidget* sessioninfo(SessionEntry & entry, int session_id, WebUIApplication* application);

  // Errorhandling for iscsitarget status (stop)
  auto& client = application->get_ssh_client();
  std::string iet_running =
  client.execute_command("LEER='true' && ls /proc/net/iet 2> /dev/null > "
			       "/dev/null && LEER='false' && echo $LEER");

  iet_running = filter_newline(iet_running);

  //false == iscsitarget is running
  if (iet_running == "false") {
    std::string number_of_targets_str = client.execute_command(
	"cd /proc/net/iet/ && grep 'tid' session | wc -l");
    int number_of_targets = stoi(number_of_targets_str);

    std::string number_of_session_str = client.execute_command(
	"cd /proc/net/iet/ && grep 'sid' session | wc -l");
    // TODO error handling
    int number_of_session = stoi(number_of_session_str);

    // infoPanel
    auto containerInfo = new WContainerWidget();
    auto wtTargets = new WText(number_of_targets_str);
    auto wtSessions = new WText(number_of_session_str);
    containerInfo->addWidget(wtTargets);
    containerInfo->addWidget(wtSessions);
    auto infoPanel = new WPanel();
    infoPanel->setTitle("info");
    infoPanel->setStyleClass("panel-primary");
    infoPanel->setCentralWidget(containerInfo);

    auto tISCSI = new Wt::WTemplate(Wt::WString::tr("iscsiinfo-panel"));
    tISCSI->addFunction("id", &Wt::WTemplate::Functions::id);
    tISCSI->bindString("targets", "Targets: ");
    tISCSI->bindWidget("wtTargets", wtTargets);
    tISCSI->bindString("sessions", "Sessions: ");
    tISCSI->bindWidget("wtSessions", wtSessions);
    containerInfo->addWidget(tISCSI);
    this->addWidget(infoPanel);

    // output row session 'sid' from 1-3 lines, and the next output is from 5-7
    // and so on...
    int y = 1;
    int z = 3;
    for (int i = 0; i < number_of_session; ++i) {
      std::string session_str = client.execute_command(
	  ("cd /proc/net/iet/ && grep -A 1 -B 1 'sid' session | sed -n "s +
	   to_string(y) + ","s + to_string(z) + "p")
	      .c_str());

      y = y + 4;
      z = z + 4;

      // filter carraige return \r utility.hpp
      session_str = filter_newline(session_str);

      auto entry = parse_session_entry(session_str, number_of_targets);

      // active session Panel
      this->addWidget(sessioninfo(entry, i + 1, application));
    }
  }
}

iSCSIInfo::~iSCSIInfo() {}
