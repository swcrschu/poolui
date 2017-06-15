#!/bin/bash

LEER="true" && ls /home/schuldtc/setup/dhcp/generatefiles/hosts/* 2> /dev/null > /dev/null && LEER="false"
echo Leer?: $LEER
