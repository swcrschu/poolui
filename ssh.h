
#pragma once

#include <string>

int authenticate_with_ssh(  const char* user, 
			    const char* password, 
			    const char* host
			    );
std::string execute_command_with_ssh( 
			    const char* user, 
			    const char* host, 
			    const char* command
			    );
