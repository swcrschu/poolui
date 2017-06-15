
#pragma once

#include <vector>
#include <utility>
#include <string>
#include <thread>
#include <map>

#include <Wt/WApplication>

#include "SSHClient.hpp"

class WebUI{
  public:


    ~WebUI();

    /// run the web server in a separate thread
    int run( int argc, char** argv );



  private:


};


class WebUIApplication : public Wt::WApplication {
 public:
  WebUIApplication(const Wt::WEnvironment &env);

  SSHClient& get_ssh_client() {
    return client;
  }

 private:
// LayoutContainer
  Wt::WWidget *content_south();

// command-Output mouseover
  Wt::WLabel *label;
// Comand-Output Area
  Wt::WTextArea *ta_command_output;
  
//  key value map command file key_to_command
  std::map<std::string, std::string> key_to_command;
  void read_input_file(std::string filename);

  SSHClient client;

};
