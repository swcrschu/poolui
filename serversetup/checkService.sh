#!/bin/bash

# Insert services here ...
SERVICES=("dhcpd" "ietd" "in.tftpd" "named" "ts3server_linux_amd64" "eggdrop")

# DON'T CHANGE ANYTHING BELOW HERE UNTIL YOU KNOW WHAT YOU DO ....

# Get Lenght of Array
aLen=${#SERVICES[@]}

# Printing Title
echo ""
echo -e "\t------------------"
echo -e "\t| Service-States |"
echo -e "\t------------------"
echo ""

# Loop for handling the array of services
for (( i=0; i<${aLen}; i++)); do
      #check service-state and generate state-message
          if [ "$(pidof ${SERVICES[$i]})" ]; then
	            STATE="\033[1;32m[running]\033[0m"
		        else
			          STATE="\033[1;31m[stopped]\033[0m"
				      fi
				          # Printing service-state
					      echo -e "\t$STATE   ${SERVICES[$i]}"
					    done
					    echo ""
