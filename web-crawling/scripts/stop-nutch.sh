#!/bin/bash

BIN_DIR=$(dirname $0)

source ${BIN_DIR}/conf/config.properties
source ${BIN_DIR}/utils.sh

echo
echo Stopping Nutch crawler....
kill_process crawl
echo ....done

echo
echo Stopping readdb....
kill_process readdb
echo ....done
