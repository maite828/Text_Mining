#!/bin/bash

# This script performs crawl for Dowser selected domains, in Spanish
# NUTCH_SEED_FILENAME is relative to NUTCH_DIR/runtime/local/urls, not the full path

SCRIPTS_DIR=$(dirname $0)/..

CRAWLER_NAME="DowserEspCrawler"
NUTCH_SEED_FILENAME="seed_dowser_esp.txt"
NUM_ITERATIONS=4

cd ${SCRIPTS_DIR}
./start-crawler.sh crawl ${CRAWLER_NAME} ${NUTCH_SEED_FILENAME} ${NUM_ITERATIONS}

