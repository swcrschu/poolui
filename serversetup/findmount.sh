#!/bin/bash
#
# find all mounts 


mounts=$(cat /etc/mtab | grep "/mnt" | awk '{print $2}' | tr -d '\r')
array=($mounts)
aLen=${#mounts[@]}

printf "%s \n" ${mounts[@]} 

