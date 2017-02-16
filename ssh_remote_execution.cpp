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

using namespace std;
 
 
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


std::string execute_command_with_ssh( 
    const char* _username, 
    const char* _host, 
    const char* remote_command
    )
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
    std::string result;

    char private_key_file[4096];
    sprintf( private_key_file, "%s_%s", _username, _host );
    char public_key_file[4096];
    sprintf( public_key_file, "%s.pub", private_key_file );

    hostaddr = inet_addr(_host);
 
    rc = libssh2_init (0);

    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return "initialization failed";
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
        return "failed to connect";
    }
 
    /* Create a session instance and start it up. This will trade welcome
     * banners, exchange keys, and setup crypto, compression, and MAC layers
     */ 
    session = libssh2_session_init();

    if (libssh2_session_handshake(session, sock)) {

        fprintf(stderr, "Failure establishing SSH session\n");
        return "session establish failed";
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
    userauthlist = libssh2_userauth_list(session, _username, strlen(_username));

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
    auth_pw = 4;
    fprintf(stderr, "using public and private key authentication\n");
    
    /* Or by public key */ 
    if (libssh2_userauth_publickey_fromfile(session, _username, public_key_file,
					    private_key_file, "")) {
	fprintf(stderr, "\tAuthentication by public key failed!\n");
	goto shutdown;
    } else {
	fprintf(stderr, "\tAuthentication by public key succeeded.\n");
    }
 
#if 1
    /* Request a shell */ 
    if (!(channel = libssh2_channel_open_session(session))) {

        fprintf(stderr, "Unable to open a session\n");
        goto shutdown;
    }
 
 
    /* Request a terminal with 'vanilla' terminal emulation
     * See /etc/termcap for more options
     */ 
    if (libssh2_channel_request_pty(channel, "vanilla")) {

        fprintf(stderr, "Failed requesting pty\n");
        goto skip_shell;
    }
 
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

    fprintf( stderr, "%s\n", remote_command );
    while( (rc = libssh2_channel_exec(channel, remote_command)) == LIBSSH2_ERROR_EAGAIN )
    {
      fprintf(stderr, "calling waitsocket\n" );
      waitsocket(sock, session);
    }
    bytecount = 0;
#if 1
    if( rc != 0 )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }
#endif
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
                for( i=0; i < rc; ++i ){
                  fputc( buffer[i], stderr);
		  result += buffer[i];
		}
                fprintf(stderr, "\n");
		//std::string s(&buffer[i+1], rc);
		//std::cout << "s:" << s << std::endl;
		result += "\n"; 
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

    std::cout << "result: " << result << std::endl;
 
    return result;
}
