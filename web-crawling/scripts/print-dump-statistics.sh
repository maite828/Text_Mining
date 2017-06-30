#!/bin/bash
set -u

if [ $# -ne 1 ]; then
    echo "This script prints a summary of crawled urls by reading the HBase dump output file" 
    echo "Usage: $(basename $0) <dump_file>"
    exit 1
fi

echo "Number of crawled URLs:" 
grep -c 'baseUrl:' $1
echo "Number of crawled URLs with text:" 
grep -c 'text:start:' $1
echo "Number of crawled domains:"
awk 'BEGIN {RS="";ORS="\n"} { print gensub("^http://([^/]*)/.*","\\1","g",$1) }' $1 | sort | uniq | wc -l
echo "Number of crawled domains with text:"
awk 'BEGIN {RS="";ORS="\n"} /text:start:/ { print gensub("^http://([^/]*)/.*","\\1","g",$1) }' $1 | sort | uniq | wc -l
echo "Crawl status summary: "
egrep -a '^status:' $1 | sort | uniq -c
