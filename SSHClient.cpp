#include <ctype.h>
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

using namespace std;

#include "SSHClient.hpp"

bool SSHClient::is_authenticated() {
  if (access(private_key_file.c_str(), F_OK) != -1) {
    std::cerr << "connection is already authenticated" << std::endl;
    return true;
  }
  return false;
}

#if 1
static void kbd_callback(const char *name, int name_len,
			 const char *instruction, int instruction_len,
			 int num_prompts,
			 const LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
			 LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
			 void **abstract) {
  printf("in kbd callback\n");
  (void)name;
  (void)name_len;
  (void)instruction;
  (void)instruction_len;
  if (num_prompts == 1) {
  }
  (void)prompts;
  (void)abstract;
  printf("leaving\n");
} /* kbd_callback */
#endif

static int waitsocket(int socket_fd, LIBSSH2_SESSION *session) {
  struct timeval timeout;
  int rc;
  fd_set fd;
  fd_set *writefd = NULL;
  fd_set *readfd = NULL;
  int dir;

  timeout.tv_sec = 10;
  timeout.tv_usec = 0;

  FD_ZERO(&fd);

  FD_SET(socket_fd, &fd);

  /* now make sure we wait in the correct direction */
  dir = libssh2_session_block_directions(session);

  if (dir & LIBSSH2_SESSION_BLOCK_INBOUND) readfd = &fd;

  if (dir & LIBSSH2_SESSION_BLOCK_OUTBOUND) writefd = &fd;

  rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);

  return rc;
}

bool SSHClient::initialize() {
  auto rc = libssh2_init(0);
  if (rc != 0) {
    fprintf(stderr, "libssh2 initialization failed (%d)\n", rc);
    return false;
  }
  return true;
}

LIBSSH2_SESSION *SSHClient::new_session(int sock) {
  /* Create a session instance and start it up. This will trade welcome
   *    * banners, exchange keys, and setup crypto, compression, and MAC layers
   *       */
  auto session = libssh2_session_init();

  if (libssh2_session_handshake(session, sock)) {
    fprintf(stderr, "Failure establishing SSH session\n");
    return (LIBSSH2_SESSION *)nullptr;
  }
  return session;
}

bool SSHClient::authenticate(std::string password) {
  std::cerr << "searching for private key file " << private_key_file
	    << std::endl;

  if (is_authenticated()) return false;

  unsigned int hostaddr = inet_addr(destination_server.c_str());

  if (!initialize()) return false;

  /* Ultra basic "connect to port 22 on localhost".  Your code is
   *      * responsible for creating the socket establishing the connection
   *           */
  auto sock = socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(222);
  sin.sin_addr.s_addr = hostaddr;
  if (connect(sock, (struct sockaddr *)(&sin), sizeof(struct sockaddr_in)) !=
      0) {
    fprintf(stderr, "failed to connect!\n");
    return -1;
  }

  auto session = new_session(sock);

  /* At this point we havn't authenticated. The first thing to do is check
   *      * the hostkey's fingerprint against our known hosts Your app may have
   * it
   *           * hard coded, may go to a file, may present it to the user,
   * that's your
   *                * call
   *                     */
  auto fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

  fprintf(stderr, "Fingerprint: ");
  for (int i = 0; i < 20; i++) {
    fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
  }
  fprintf(stderr, "\n");

  /* check what authentication methods are available */
  auto userauthlist =
      libssh2_userauth_list(session, username.c_str(), username.length());

  int auth_pw = 0;
#if 1
  fprintf(stderr, "Authentication methods: %s\n", userauthlist);
  if (strstr(userauthlist, "password") != NULL) {
    auth_pw |= 1;
  }
  if (strstr(userauthlist, "keyboard-interactive") != NULL) {
    auth_pw |= 2;
  }
  if (strstr(userauthlist, "publickey") != NULL) {
    auth_pw |= 4;
  }

  /* if we got an 4. argument we set this option if supported */
  if (5 > 4) {
    if ((auth_pw & 1) && !strcasecmp("-p", "-p")) {
      auth_pw = 1;
    }
    if ((auth_pw & 2) && !strcasecmp("-p", "-i")) {
      auth_pw = 2;
    }
    if ((auth_pw & 4) && !strcasecmp("-p", "-k")) {
      auth_pw = 4;
    }
  }
#endif

  if (auth_pw & 1) {
    printf("authenticating with password\n");
    /* We could authenticate via password */
    if (libssh2_userauth_password(session, username.c_str(),
				  password.c_str())) {
      fprintf(stdout, "\tAuthentication by password failed!\n");
      return false;
    } else {
      fprintf(stdout, "\tAuthentication by password succeeded.\n");
    }
  } else if (auth_pw & 2) {
    printf("authenticating with interactive password\n");
    std::cerr << "not implemented" << std::endl;
    exit(-1);
    /* Or via keyboard-interactive */
    if (libssh2_userauth_keyboard_interactive(session, username.c_str(),
					      &kbd_callback)) {
      fprintf(stdout, "\tAuthentication by keyboard-interactive failed!\n");
      return false;
    } else {
      fprintf(stdout, "\tAuthentication by keyboard-interactive succeeded.\n");
    }
  }
#if 1
  else if (auth_pw & 4) {

    fprintf(stderr, "using public and private key authentication\n");

    /* Or by public key */
    if (libssh2_userauth_publickey_fromfile(session, username.c_str(),
					    public_key_file.c_str(),
					    private_key_file.c_str(), "")) {
      fprintf(stderr, "\tAuthentication by public key failed!\n");
      return false;
    } else {
      fprintf(stderr, "\tAuthentication by public key succeeded.\n");
    }
  } else {
    fprintf(stderr, "No supported authentication methods found!\n");
    return false;
  }
#endif

  int rc = 0;
  int bytecount = 0;
  ifstream keyfile;
  std::string key;
  std::string command;
  std::string remote_command;
#if 1
  LIBSSH2_CHANNEL *channel = nullptr;
  /* Request a shell */
  if (!(channel = libssh2_channel_open_session(session))) {
    fprintf(stderr, "Unable to open a session\n");
    return false;
  }

  /* Some environment variables may be set,
   *      * It's up to the server which ones it'll allow though
   *           */

  /* Request a terminal with 'vanilla' terminal emulation
   *      * See /etc/termcap for more options
   *           */
  if (libssh2_channel_request_pty(channel, "vanilla")) {
    fprintf(stderr, "Failed requesting pty\n");
    goto skip_shell;
  }

#if 0
																          /* Open a SHELL on that pty */ 
																          if (libssh2_channel_shell(channel)) {

																	            fprintf(stderr, "Unable to request shell on allocated pty\n");
																		            goto shutdown;
																			        }
#endif
#endif

  /* At this point the shell can be interacted with using
   *      * libssh2_channel_read()
   *           * libssh2_channel_read_stderr()
   *                * libssh2_channel_write()
   *                     * libssh2_channel_write_stderr()
   *                          *
   *                               * Blocking mode may be (en|dis)abled with:
   * libssh2_channel_set_blocking()
   *                                    * If the server send EOF,
   * libssh2_channel_eof() will return non-0
   *                                         * To send EOF to the server use:
   * libssh2_channel_send_eof()
   *                                              * A channel can be closed
   * with: libssh2_channel_close()
   *                                                   * A channel can be freed
   * with: libssh2_channel_free()
   *                                                        */

  if (channel == NULL) {
    fprintf(stderr, "Error channel not open\n");
    exit(1);
  }

  command = "ssh-keygen -t rsa -f "s + private_key_file + " -N \"\"";

  system(command.c_str());

  keyfile.open(public_key_file);
  getline(keyfile, key);

  fprintf(stderr, "key is %s\n", key.c_str());

  remote_command = "echo \" "s + key + "\" >> ~/.ssh/authorized_keys";
  fprintf(stderr, "%s\n", remote_command.c_str());

  while ((rc = libssh2_channel_exec(channel, remote_command.c_str())) ==
	 LIBSSH2_ERROR_EAGAIN) {
    fprintf(stderr, "calling waitsocket\n");
    waitsocket(sock, session);
  }
  fprintf(stderr, "channel rc state is %d\n", rc);

  if (rc != 0) {
    fprintf(stderr, "Error\n");
    exit(1);
  }

  for (;;) {
    /* loop until we block */
    int rc;
    do {
      char buffer[0x4000];
      rc = libssh2_channel_read(channel, buffer, sizeof(buffer));

      if (rc > 0) {
	int i;
	bytecount += rc;
	fprintf(stderr, "We read:\n");
	for (i = 0; i < rc; ++i) fputc(buffer[i], stderr);
	fprintf(stderr, "\n");
      } else {
	if (rc != LIBSSH2_ERROR_EAGAIN)
	  /* no need to output this for the EAGAIN case */
	  fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
      }
    } while (rc > 0);

    /* this is due to blocking that would occur otherwise so we loop on
     *            this condition */
    if (rc == LIBSSH2_ERROR_EAGAIN) {
      waitsocket(sock, session);
    } else
      break;
  }

skip_shell:
  if (channel) {
    libssh2_channel_free(channel);
    channel = NULL;
  }

/* Other channel types are supported via:
 *      * libssh2_scp_send()
 *           * libssh2_scp_recv2()
 *                * libssh2_channel_direct_tcpip()
 *                     */

shutdown:

  printf("disconnecting session\n");
  libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
  printf("freeing session\n");
  libssh2_session_free(session);

  printf("closing socket\n");
  close(sock);
  fprintf(stderr, "all done!\n");

  libssh2_exit();
  fprintf(stderr, "exit done!\n");
  return true;
}

bool SSHClient::is_session_prepared() {
  if (connection.session == nullptr) return false;
  return true;
}

void SSHClient::setup_session() {
  struct sockaddr_in sin;

  auto hostaddr = inet_addr(destination_server.c_str());

  if (!initialize()) return;

  /* Ultra basic "connect to port 22 on localhost".  Your code is
   *      * responsible for creating the socket establishing the connection
   *           */
  connection.sock = socket(AF_INET, SOCK_STREAM, 0);

  sin.sin_family = AF_INET;
  sin.sin_port = htons(222);
  sin.sin_addr.s_addr = hostaddr;
  if (connect(connection.sock, (struct sockaddr *)(&sin),
	      sizeof(struct sockaddr_in)) != 0) {
    fprintf(stderr, "failed to connect!\n");
    return;
  }

  connection.session = new_session(connection.sock);

  /* At this point we havn't authenticated. The first thing to do is check
   *      * the hostkey's fingerprint against our known hosts Your app may have
   * it
   *           * hard coded, may go to a file, may present it to the user,
   * that's your
   *                * call
   *                     */
  const char *fingerprint =
      libssh2_hostkey_hash(connection.session, LIBSSH2_HOSTKEY_HASH_SHA1);

  fprintf(stderr, "Fingerprint: ");
  for (int i = 0; i < 20; i++) {
    fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
  }
  fprintf(stderr, "\n");

  /* check what authentication methods are available */

  fprintf(stderr, "using public and private key authentication\n");
  if (libssh2_userauth_publickey_fromfile(connection.session, username.c_str(),
					  public_key_file.c_str(),
					  private_key_file.c_str(), "")) {
    fprintf(stderr, "\tAuthentication by public key failed!\n");
    return;
  } else {
    fprintf(stderr, "\tAuthentication by public key succeeded.\n");
  }
}

void SSHClient::setup_channel() {
  if (!(connection.channel =
	    libssh2_channel_open_session(connection.session))) {
    fprintf(stderr, "Unable to open a session\n");
    return;
  }

  if (libssh2_channel_request_pty(connection.channel, "vanilla")) {
    fprintf(stderr, "Failed requesting pty\n");
    teardown_session();
    return;
  }
}

void SSHClient::teardown_channel() {
  if (connection.channel) {
    libssh2_channel_free(connection.channel);
    connection.channel = nullptr;
  }
}

#if 1
std::string SSHClient::execute_command(std::string remote_command) {
  int rc;
  std::string result;

  if (!is_session_prepared()) {
    setup_session();
  }

  fprintf(stderr, "%s\n", remote_command.c_str());

  setup_channel();

  while (
      (rc = libssh2_channel_exec(connection.channel, remote_command.c_str())) ==
      LIBSSH2_ERROR_EAGAIN) {
    fprintf(stderr, "calling waitsocket\n");
    waitsocket(connection.sock, connection.session);
  }
  int bytecount = 0;
#if 1
  if (rc != 0) {
    fprintf(stderr, "Error\n");
    exit(1);
  }
#endif
  while (1) {
    /* loop until we block */
    int rc;
    do {
      char buffer[0x4000];
      rc = libssh2_channel_read(connection.channel, buffer, sizeof(buffer));

      if (rc > 0) {
	int i;
	bytecount += rc;
	fprintf(stderr, "We read:\n");
	for (i = 0; i < rc; ++i) {
	  fputc(buffer[i], stderr);
	  result += buffer[i];
	}
	fprintf(stderr, "\n");

	result += "\n";
      } else {
	if (rc != LIBSSH2_ERROR_EAGAIN)
	  /* no need to output this for the EAGAIN case */
	  fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
      }
    } while (rc > 0);

    /* this is due to blocking that would occur otherwise so we loop on
     *    this condition */
    if (rc == LIBSSH2_ERROR_EAGAIN) {
      waitsocket(connection.sock, connection.session);
    } else
      break;
  }

  teardown_channel();

  std::cout << "result: " << result << std::endl;

  return result;
}
#endif

void SSHClient::teardown_session() {
  printf("disconnecting session\n");
  libssh2_session_disconnect(connection.session,
			     "Normal Shutdown, Thank you for playing");
  printf("freeing session\n");
  libssh2_session_free(connection.session);

  printf("closing socket\n");
  close(connection.sock);
  fprintf(stderr, "all done!\n");

  libssh2_exit();
  fprintf(stderr, "exit done!\n");
}
