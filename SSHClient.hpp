

#pragma once
#include <libssh2.h>
#include <string>

class SSHClient {
 public:
  SSHClient(std::string _destination_server, std::string _username)
      : destination_server(_destination_server), username(_username) {
    using namespace std;
    set_key_file_name(username + "_"s + destination_server);
    connection.session = nullptr;
    connection.channel = nullptr;
  }
  virtual ~SSHClient() {
    if (is_session_prepared()) {
      teardown_session();
    }
  }

  bool authenticate(std::string _password);

  std::string execute_command(std::string _command);

  void set_key_file_name(std::string name) {
    private_key_file = name;
    public_key_file = name + ".pub";
  }

  void set_username(std::string _username) { username = _username; }

  std::string get_username() { return username; }

  void set_destination_server(std::string _destination_server) {
    destination_server = _destination_server;
  }

  std::string get_destination_server() { return destination_server; }

  bool is_authenticated();

 private:
  /* data */
  std::string destination_server;
  std::string private_key_file;
  std::string public_key_file;
  std::string username;

  struct SSHConnection {
    LIBSSH2_CHANNEL* channel = NULL;
    int sock;
    LIBSSH2_SESSION* session;
  };

  SSHConnection connection;

  bool is_session_prepared();
  void setup_session();
  void teardown_session();

  void setup_channel();
  void teardown_channel();

  bool initialize();
  LIBSSH2_SESSION* new_session(int sock);
};
