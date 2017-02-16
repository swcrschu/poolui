/* *
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */ 
 
#include <libssh2.h>
#include <libssh2_sftp.h>
 
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include <string>
#include <iostream>

#include "ssh.h"
 
 
static int waitsocket(int socket_fd, LIBSSH2_SESSION *session)
{
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

 
    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;
 
    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;
 
    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeout);
 
    return rc;
}
 
 
const char* username = NULL;
const char* password = NULL;
const char* host = NULL;

#if 1
static void kbd_callback(const char *name, int name_len,
                         const char *instruction, int instruction_len,
                         int num_prompts,
                         const LIBSSH2_USERAUTH_KBDINT_PROMPT *prompts,
                         LIBSSH2_USERAUTH_KBDINT_RESPONSE *responses,
                         void **abstract)
{
  printf("in kbd callback\n");
    (void)name;
    (void)name_len;
    (void)instruction;
    (void)instruction_len;
    if (num_prompts == 1) {
        responses[0].text = strdup(password);
        responses[0].length = strlen(password);
    }
    (void)prompts;
    (void)abstract;
    printf("leaving\n");
} /* kbd_callback */ 
#endif
 
int authenticate_with_ssh( 
    const char* _username, 
    const char* _password, 
    const char* _host)
{
    unsigned long hostaddr;
    int rc, sock, i, auth_pw = 0;
    struct sockaddr_in sin;
    const char *fingerprint;
    char *userauthlist;
    LIBSSH2_SESSION *session = NULL;
    LIBSSH2_CHANNEL *channel = NULL;
    int exit_code = 0;
    int bytecount = 0;
    FILE* keyfile;

    char private_key_file[4096];
    sprintf( private_key_file, "%s_%s", _username, _host );
    char public_key_file[4096];
    sprintf( public_key_file, "%s.pub", private_key_file );

    fprintf(stderr, "searching for private key file %s\n", private_key_file);
    // TODO if file already exists stop 
    if( access( private_key_file, F_OK ) != -1 ) {
      fprintf( stderr, "connection for user %s is already authenticated\n", _username );
      return 0;      
    } 

    username = strdup( _username );
    password = strdup( _password );
    host = strdup( _host );
 
    hostaddr = inet_addr(host);
 
    rc = libssh2_init (0);

    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }
 
    /* Ultra basic "connect to port 22 on localhost".  Your code is
     * responsible for creating the socket establishing the connection
     */ 
    sock = socket(AF_INET, SOCK_STREAM, 0);
 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(sock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }
 
    /* Create a session instance and start it up. This will trade welcome
     * banners, exchange keys, and setup crypto, compression, and MAC layers
     */ 
    session = libssh2_session_init();

    if (libssh2_session_handshake(session, sock)) {

        fprintf(stderr, "Failure establishing SSH session\n");
        return -1;
    }
 
    /* At this point we havn't authenticated. The first thing to do is check
     * the hostkey's fingerprint against our known hosts Your app may have it
     * hard coded, may go to a file, may present it to the user, that's your
     * call
     */ 
    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

    fprintf(stderr, "Fingerprint: ");
    for(i = 0; i < 20; i++) {
        fprintf(stderr, "%02X ", (unsigned char)fingerprint[i]);
    }
    fprintf(stderr, "\n");
 
    /* check what authentication methods are available */ 
    userauthlist = libssh2_userauth_list(session, username, strlen(username));

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
    if(5 > 4) {
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

    printf(" username %s password %s host %s \n ", username, password, host );
    if ( auth_pw & 1 ) {
      printf("authenticating with password\n");
      /* We could authenticate via password */ 
      if (libssh2_userauth_password(session, username, password)) {
	  fprintf(stdout, "\tAuthentication by password failed!\n");
	  goto shutdown;
      } else {
	  // means everything is ok
	  exit_code = 1;
	  fprintf(stdout, "\tAuthentication by password succeeded.\n");
      }
    }else if (auth_pw & 2) {
	printf("authenticating with interactive password\n");
        /* Or via keyboard-interactive */ 
        if (libssh2_userauth_keyboard_interactive(session, username,
                                                  &kbd_callback) ) {
            fprintf(stdout,
                "\tAuthentication by keyboard-interactive failed!\n");
            goto shutdown;
        } else {
	  exit_code = 1;
            fprintf(stdout,
                "\tAuthentication by keyboard-interactive succeeded.\n");
        }
    }
#if 1 
    else if (auth_pw & 4) {

	fprintf(stderr, "using public and private key authentication\n");

	
        /* Or by public key */ 
        if (libssh2_userauth_publickey_fromfile(session, username, public_key_file,
                                                private_key_file, "")) {
            fprintf(stderr, "\tAuthentication by public key failed!\n");
            goto shutdown;
        } else {
            fprintf(stderr, "\tAuthentication by public key succeeded.\n");
        }
    } else {
        fprintf(stderr, "No supported authentication methods found!\n");
        goto shutdown;
    }
#endif
 
#if 1
    /* Request a shell */ 
    if (!(channel = libssh2_channel_open_session(session))) {

        fprintf(stderr, "Unable to open a session\n");
        goto shutdown;
    }
 
    // not needed in this case
    /* Some environment variables may be set,
     * It's up to the server which ones it'll allow though
     */ 
    // libssh2_channel_setenv(channel, "FOO", "bar");

 
    /* Request a terminal with 'vanilla' terminal emulation
     * See /etc/termcap for more options
     */ 
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
     * libssh2_channel_read()
     * libssh2_channel_read_stderr()
     * libssh2_channel_write()
     * libssh2_channel_write_stderr()
     *
     * Blocking mode may be (en|dis)abled with: libssh2_channel_set_blocking()
     * If the server send EOF, libssh2_channel_eof() will return non-0
     * To send EOF to the server use: libssh2_channel_send_eof()
     * A channel can be closed with: libssh2_channel_close()
     * A channel can be freed with: libssh2_channel_free()
     */ 

    if( channel == NULL )
    {
      fprintf(stderr,"Error channel not open\n");
      exit( 1 );
    }

    char command[4096];
    sprintf ( command , "ssh-keygen -t rsa -f %s -N \"\"", private_key_file );
    // TODO generate a key pair 
    system( command );

    keyfile = fopen( public_key_file, "r" ); 

    char key[4096];
    fgets( key, 4096, keyfile );
    fprintf( stderr, "key is %s\n", key );
    fclose( keyfile );

    char remote_command[4096];

    sprintf( remote_command, "echo \" %s \" >> ~/.ssh/authorized_keys", key ); 
    fprintf( stderr, "%s\n", remote_command );

    while( (rc = libssh2_channel_exec(channel, remote_command)) == LIBSSH2_ERROR_EAGAIN )
    {
      fprintf(stderr, "calling waitsocket\n" );
      waitsocket(sock, session);
    }
    fprintf( stderr, "channel rc state is %d\n", rc );

    if( rc != 0 )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }

    for( ;; )
    {
        /* loop until we block */ 
        int rc;
        do
        {
            char buffer[0x4000];
            rc = libssh2_channel_read( channel, buffer, sizeof(buffer) );

            if( rc > 0 )
            {
                int i;
                bytecount += rc;
                fprintf(stderr, "We read:\n");
                for( i=0; i < rc; ++i )
                    fputc( buffer[i], stderr);
                fprintf(stderr, "\n");
            }
            else {
                if( rc != LIBSSH2_ERROR_EAGAIN )
                    /* no need to output this for the EAGAIN case */ 
                    fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
            }
        }
        while( rc > 0 );
 
        /* this is due to blocking that would occur otherwise so we loop on
           this condition */ 
        if( rc == LIBSSH2_ERROR_EAGAIN )
        {
            waitsocket(sock, session);
        }
        else
            break;
    }

 
  skip_shell:
    if (channel) {
        libssh2_channel_free(channel);
        channel = NULL;
    }
 
    /* Other channel types are supported via:
     * libssh2_scp_send()
     * libssh2_scp_recv2()
     * libssh2_channel_direct_tcpip()
     */ 
 
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

 
    return exit_code;
}

