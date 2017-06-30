#!/bin/bash

# This script performs crawl for a list of 84k DMOZ classified domains visited by Movistar users
# NUTCH_SEED_FILENAME is relative to NUTCH_DIR/runtime/local/urls, not the full path

SCRIPTS_DIR=$(dirname $0)/..

CRAWLER_NAME="AutomaticClassifierCrawler"
NUTCH_SEED_FILENAME="seed_domains_all_84mil.txt"
NUM_ITERATIONS=2
            
cd ${SCRIPTS_DIR}
./start-crawler.sh crawl ${CRAWLER_NAME} ${NUTCH_SEED_FILENAME} ${NUM_ITERATIONS}

