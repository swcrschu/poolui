#!/bin/bash

# ssh shutdown 

ip=$1
ip_neu=$(echo $1 | awk '{print substr($0,4)}')
down=$(ssh -o StrictHostKeyChecking=no poolui@$ip_neu sudo systemctl poweroff)
echo $down
$down
