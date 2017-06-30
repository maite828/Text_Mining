#!/bin/bash
set -u

if [ $# -ne 1 ]; then
    echo "This script prints a summary of fetched urls by reading the start-crawler.sh output file" 
    echo "Usage: $(basename $0) <crawler_out>"
    exit 1
fi

echo "Fetched URLs: $(grep 'fetching http' $1 | wc -l)"
echo "Fetch errors:"
# Remove the host name when UnknownHostException happens, to group by error
grep 'failed with:' $1 | sed 's/.*failed with:/failed with:/' | sed 's/, url=.*//' | sed 's/UnknownHostException:.*/UnknownHostException/' | sort | uniq -c
echo "Total errors: $(grep 'fetch of http' $1 | wc -l)"
