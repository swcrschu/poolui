#!/bin/bash
#	Copyright (c) 2017 - Christian Schuldt (christian.schuldt@uni-greifswald.de)
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
################################################################
#
#	 $# = enthält die Anzahl der übergebenen Parameter
#	 $0 = enthält den Namen des scripts
#	 $1-9 = enthalten die ersten neun Parameter
#
#
#echo "\$# = $#"
#echo "\$0 = $0"
#echo "Tastk = $1"
#echo "IQN = $2"
#echo "VolumeSize = $3"
#echo "Master-Image = $4"
#echo "Zielpfad = $5"
#echo "User = $6"
#echo "Passwort = $7"
##############################################################
# task [check/create/DHCP]
task=$1

iqn=$2
volumeSize=$3
image=$4
destination=$5
user=$6
password=$7

# only for createDHCP
hostname=$8 
mac=$9

#############################################################
#	preCheck global variable
# c1 = iqn ; c2 = new volume or image copy ; c3 = destination ; c4 = dhcp
c1=false
c2=false
c3=false
c4=false
c5=false
#
############
#
ietdconf=/etc/iet/ietd.conf
DHCPDCONF=/etc/dhcp/dhcpd.conf
errors=();
#
############

function preCheck
{
# ---------------------------------------------------
  # Search for an existing target 
  # search and save all targets form ietd.conf
  target=$(sudo find $ietdconf -type f -exec grep 'Target iqn.*' {} \; | awk '{print $2}')
  array=($target)
  aLen=${#array[@]}
  STATE=0
	
  # iqn: go through and check avaliable 

  for ((i=0;i<=${aLen};i++))
   do
      if [ "${array[i]}" == "$iqn" ]; then		
	STATE=1	
  
      fi		
	
   done
  
      if [ $STATE -eq 1 ]; then		 
	errors=("${errors[@]}" "IQN is available! Try again with an not existing Target") 
	 c1=false
      else 
	c1=true

      fi
# --------------------------------------------------
  # new volume or image copy
	## -o prüft erste Bedienung, wenn true wird zweite nicht mehr gepfüft
	## -a prüft erste & zweite Bedienung

  if [ "$volumeSize" -gt 0 ]; then
  	c2=true		
  
  elif [ "$volumeSize" -eq 0 -a -e $image ]; then
  	c2=true		
  else
	errors=("${errors[@]}" "You need a new image or a copy of an image") 

  fi

# --------------------------------------------------
   # destination exists ?
   # -d = file is a directory
   # TODO check verfügbare ordner größe und prüfe rest nach kopie oder neu volume

  if [ -d $destination ]; then
	c3=true
  else
	errors=("${errors[@]}" "$destination The directory does not exist ") 
	c3=false
  fi

  preCheckSUM
}



function preCheckSUM
{
  if [ "$c1" == "true" -a "$c2" == "true" -a "$c3" == "true"  ]; then
#     echo "all checks complete and done"
#     echo "true"
      preCheckTASK $task	
  
  else
    echo "check finished with: ${#errors[@]} error/s"
#   echo ${errors[@]} Ausgabe der Schleife ist schicker
    max=${#errors[*]}
    typeset -i i=0
    while (( i < max ))
	do
   	  echo "error $i :"  ${errors[$i]}
          i=i+1
        done
	exit 1
  fi	
		

}


function preCheckTASK
{

  if [ "$task" == "create" ]; then
    
   echo "task = $task 
	 aufruf function create "
	createTARGET $iqn $volumeSize $image $destination $user $password

   elif [ "$task" == "check" ]; then
	echo "target: $task complete
	      were tested:
	      -----------
	      IQN: $c1
              Image: $c2
	      Destionation: $c3	 "
			 
	exit 0
  
   elif [ "$task" == "dhcp" ]; then
	echo "task = $task
	      call function createDHCP "
	preCheckDHCP $iqn $volumeSize $image $destination $user $password $hostname $mac     

   else	
	echo "task: $task no task available [check/create/dhcp] required"
	errors=("${errors[@]}" "no task available [check/create/dhcp] required ")

 fi
}

function createTARGET
{
# new volume or image copy
	## -o prüft erste Bedienung, wenn true wird zweite nicht mehr gepfüft
	## -a prüft erste & zweite Bedienung
# Lun complete 
 Lun=$destination/$iqn\0
 
 if [ "$volumeSize" -gt 0 ]; then
	echo "create a new Volume"
	
	create=$(sudo dd if=/dev/zero of=$destination/$iqn\0 count=0 obs=1 seek=$volumeSize\G)
	$create
	

  elif [ "$volumeSize" -eq 0 -a -e $image ]; then
	echo "create a Copy from: $image"
 	copy=$(sudo mv $image $Lun )	
	$copy
	echo "kopieren fertig"
	
 fi

#### concatenate ietdconf from multiple files
 datum=$(date +"%Y-%m-%d %H:%M:%S")
 NUMBER_OF_LUNS=1
 BASEDIR=/home/schuldtc/setup/
 TARGETS=/home/schuldtc/setup/targets

 cd $TARGETS
 
 last_target=$(ls -d *_target | sort -n | tail -n 1 )
 last_id=$(echo $last_target | awk -F "_" '{ print $1 }')
 new_id=$((10#$last_id + 1))
 echo $new_id
 folder_name=$(printf "%03d_target" $new_id)
 
 mkdir $folder_name
 cd $folder_name
 echo "
 # Create $datum
Target $iqn" > 001_iqn
 mkdir 002_luns
 for(( i=0 ; $i < $NUMBER_OF_LUNS ; i++ )); do
   filename=$(printf "002_luns/%03d_lun" $i)
   echo " LUN $i PATH=$Lun,Type=fileio" > $filename
 done 
 
 echo "	InComingUser $user $password
 	MaxRecvDataSegmentLength        8192
 	MaxXmitDataSegmentLength        8192
 	NOPInterval                     15
 	NOPTimeout                      15" > 003_auth

 cd $BASEDIR
 echo "find $TARGETS/*_target/ -type f | sort -n | xargs cat > $ietdconf" > regenerate_targets.sh
 chmod +x regenerate_targets.sh

# regenerate ietdconf every time
 sudo ./regenerate_targets.sh


}

function preCheckDHCP
{
    if [ -z "$hostname" -o -z "$mac" ]; then

      echo "complete all Parameter to start the DHCP function: hostname MAC"
      errors=("${errors[@]}" "complete all Parameter to start the DHCP function: hostname MAC")
      exit 1

    fi 
    
 dhcpMacoutput=$(sudo find $DHCPDCONF -type f -exec grep -i -w -B1 -A3 "$mac" {} \; )
 dhcpMacCheck=$(sudo find $DHCPDCONF -type f -exec grep -i -o -w "$mac" {} \; )  
## Mac Check 
 array=($dhcpMacCheck)
 aLen=${#array[@]}
 MAC=0
  # Mac: go through 

  for ((i=0;i<=${aLen};i++))
   do

      if [ "${array[i]}" == "$mac" ]; then		
	 	  MAC=1
	fi    
  done  
	
  if [ $MAC -eq 1 ]; then		 
     echo "Host with $mac is available in $DHCPDCONF"
	  errors=("${errors[@]}" "Host with $mac available in $DHCPDCONF")
	  c4=false
	  echo $dhcpMacoutput
      else 
	c4=true
#	createDHCP $iqn $user $password $hostname $mac
	
  fi	

# hostname check
 dhcpHostnameoutput=$(sudo find $DHCPDCONF -type f -exec grep -i -w -B1 -A3 "$hostname" {} \; )
 dhcpHostnameCheck=$(sudo find $DHCPDCONF -type f -exec grep -i -w -o "$hostname" {} \; )  

 array2=($dhcpHostnameCheck)
 aLen2=${#array2[@]}
 Host=0
 # Hostname: go through 

  for ((i=0;i<=${aLen2};i++))
   do

      if [ "${array2[i]}" == "$hostname" ]; then		
	 	  Host=1
	fi    
  done  
	
  if [ $Host -eq 1 ]; then		 
	 echo "Host with $hostname is available in $DHCPDCONF"
	  errors=("${errors[@]}" "Host with $hostname available in $DHCPDCONF")
	  c5=false
	  echo $dhcpHostnameoutput
      else 
	c5=true
	
  fi	

 if [ "$c4" == "true" -a "$c5" == "true" ]; then
  
	createDHCP $iqn $user $password $hostname $mac
 
 else
    echo "check finished with: ${#errors[@]} error/s"
#   echo ${errors[@]} Ausgabe der Schleife ist schicker
    max=${#errors[*]}
    typeset -i i=0
    while (( i < max ))
	do
   	  echo "error $i :"  ${errors[$i]}
          i=i+1
        done
	exit 1
       
 fi


  }
 

function createDHCP
{
  
 datum=$(date +"%Y-%m-%d %H:%M:%S")
 BASEDIR=/home/schuldtc/setup/
 DHCP=/home/schuldtc/setup/dhcp
 GENERATEFILES=/home/schuldtc/setup/dhcp/generatefiles
 OPTIONS=/home/schuldtc/setup/dhcp/generatefiles/01_options
 HOSTS=/home/schuldtc/setup/dhcp/generatefiles/02_hosts
 EOF=/home/schuldtc/setup/dhcp/generatefiles/03_EOF

 #load standard header dhcpd.conf and write to generatefiles/options
 source $DHCP/dhcp-options 

 if [ -d $OPTIONS ]; then
   cd $OPTIONS

 else
   mkdir $OPTIONS && cd $OPTIONS
 fi

 echo "
  $dhcpoptions" > 001_options

 echo "
  $subnet" > 002_subnet

 
 if [ -d $HOSTS ]; then 
    cd $HOSTS
  
  else
    mkdir $HOSTS && cd $HOSTS 
 fi


 # search last HOSTID in HOSTS
 initial_id=003 
 last_DHCP=$(ls | sort -n | tail -n 1 )
 last_id=$(echo $last_DHCP | awk -F "_" '{ print $1 }')

 #hostsfolder is empty? start with 003_
 LEER="true" && ls $HOSTS/* 2> /dev/null > /dev/null && LEER="false"
 #echo Leer?: $LEER

 if [ $LEER == "true" ]; then
    new_id=$initial_id
    host_id=$(printf "%03d_" $initial_id)
 else
    new_id=$(( 10#$last_id + 1))
    host_id=$(printf "%03d_" $new_id)
 fi 
 
 echo $host_id
# TODO root-path next-server
 echo "
  # $datum
  host $hostname {
      hardware ethernet $mac;
      if exists user-class and option user-class = \"gPXE\" {
      filename \"\";
      option root-path \"iscsi:10.10.0.1::::$iqn\";
      option gpxe.username \"$user\";
      option gpxe.password \"$password\";
      }
      else {filename \"gpxelinux.0\";}
  } " > $host_id$hostname

 if [ -d $EOF ]; then 
    cd $EOF
  
  else
    mkdir $EOF && cd $EOF 
 fi
 
 echo "
 }
 " > $EOF/999_EOF

 cd $BASEDIR
 echo "find $GENERATEFILES/ -type f | sort -n | xargs cat > $DHCPDCONF" > regenerate_dhcp.sh
 chmod +x regenerate_dhcp.sh

 # regenerate ietdconf every time
 cat regenerate_dhcp.sh
 sudo ./regenerate_dhcp.sh


}



####################################
#
# inital check (all parameters ?)
#
###################################
# -z check paramter empty or not and all config files available

if [ -z "$iqn" -o -z "$volumeSize" -o -z "$image" -o -z "$destination" -o -z "$user" -o -z "$password" ]; then
  
  echo "complete all Parameters to start the Check: iqn volumeSize image destination user password
		Try again..."
else
#	echo "check start..."
	 
	preCheck $iqn $volumeSize $image $destination $user $password
fi 





