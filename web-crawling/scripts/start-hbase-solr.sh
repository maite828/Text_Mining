#!/bin/bash

BIN_DIR=$(dirname $0)

source ${BIN_DIR}/conf/config.properties
source ${BIN_DIR}/utils.sh

##### HBASE #####
log "Starting HBase...."
cd ${HBASE_DIR}
./start-hbase.sh

RESULT=$?
if [ ${RESULT} -ne 0 ] && [ ${RESULT} -ne 1 ]; then
    log "...ERROR: HBase not started"
	exit 1
fi
log "...HBase started"

##### SOLR #####
log "Starting Solr...."
cd $SOLR_DIR

nohup java -jar start.jar > /dev/null 2>&1 &

is_process_running start.jar
RESULT=$?
if [ ${RESULT} -eq 0 ]; then
    log "...ERROR: Solr not started"
	exit 1
fi
log "...Solr started"

