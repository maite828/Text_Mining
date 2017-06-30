#!/bin/bash

# This script performs crawl for Movistar and Telefonica domains, plus categories of interest for brand tracking
# NUTCH_SEED_FILENAME is relative to NUTCH_DIR/runtime/local/urls, not the full path

SCRIPTS_DIR=$(dirname $0)/..

CRAWLER_NAME="AnalyticsCrawler"
NUTCH_SEED_FILENAME="seed_intersantes_coldomain_15k.txt"
NUM_ITERATIONS=2
            
cd ${SCRIPTS_DIR}
./start-crawler.sh crawl ${CRAWLER_NAME} ${NUTCH_SEED_FILENAME} ${NUM_ITERATIONS}

