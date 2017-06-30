#!/bin/bash

BIN_DIR=$(dirname $0)

source ${BIN_DIR}/conf/config.properties
source ${BIN_DIR}/utils.sh

echo
echo Stopping HBase....
cd ${HBASE_DIR}
./stop-hbase.sh

echo
echo Stopping Solr....
kill_process start.jar

