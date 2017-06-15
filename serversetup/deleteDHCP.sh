#!/bin/bash
#
# delete DHCP host from 02_hosts  

echo "$1"
echo "$2"

hostname=$1
mac=$2

HOSTS=/home/schuldtc/setup/dhcp/generatefiles/02_hosts

# answer the dhcpHost file
findmac=$(sudo find $HOSTS -type f -exec grep -i -l -w -o "$mac" {} \;)
findhostname=$(sudo find $HOSTS -type f -exec grep -i -l -w -o "$hostname" {} \;)

array=($findhostname)
aLen=${#findhostname[@]}

# in addition check
if [ $findmac == $findhostname ]; then
 
  for ((i=0;i<=${aLen};i++))
    do
      if [ "${array[i]}" == "$findmac" ]; then
	
	echo "${array[i]}"
	del=$(sudo rm ${array[i]})
	echo "DHCP-Host $hostname $mac deleted"
      fi 
  done
  
else 
  echo "nicht gefunden"  

fi

  

