#!/bin/bash
set -u
set -e

if [ $# -lt 2 ]; then
    echo "This script creates a CSV file to be used as input for the text dashboard" 
    echo "Usage: $0 <crawl-file> <search-words> [<language>] [<ignore-case>] [<output-file>]"
    echo "   crawl-file: Nutch output file, clean with 4 field lines: URL, text, language, language-trust"
    echo "   search-words: words to be found in the text, separated by ',' character"
    echo "   language: language to filter (example: ESP, ENG, CAT). Leave empty for not filtering"
    echo "   ignore-case: if 'Y', case is ignored when searching (by default)"
    echo "   output-file: if not specified, output file is <language>_<search-words>_yyyymmdd_yyyymmdd.csv"
    exit 1
fi

echo "CSV generation started...."

CRAWL_FILE=$1
SEARCH_WORDS=${2//,/\|} # Replace commas by pipes
LANGUAGE=${3:-""}
IGNORECASE_FLAG=${4:-"Y"}

# Define de filter of the expression
MATCH_EXP="/${SEARCH_WORDS}/"
if [ "${LANGUAGE}" != "" ]; then
    MATCH_EXP='$3~/'${LANGUAGE}'/ && '${MATCH_EXP}
else
    LANGUAGE="all"
fi

# Determine if the search expression must match case or not
if [ "${IGNORECASE_FLAG}" == "y" ]  || [ "${IGNORECASE_FLAG}" == "Y" ]; then
    IGNORECASE_FLAG=1
else
    IGNORECASE_FLAG=0
fi 

# Include double quotes in the separator (begin and end quotes will be needed)
OUTPUT_FIELD_SEPARATOR='\",\"'
# Separators without escaping character: {FS="\n";RS="\n\n";OFS=",";ORS="\n"}
CONFIGURATION="IGNORECASE=${IGNORECASE_FLAG};FS=\"\n\";RS=\"\n\n\";OFS=\"${OUTPUT_FIELD_SEPARATOR}\";ORS=\"\n\""

# Initial and final quotes added. Intermediate quotes are part of the field separator
HEADER='"\"source","host","link","time","auth","age","gender","country","location","sentiment","title","snippet","contents","uniqueid","authname\""'
DATETIME="datetime=\"$(date --utc +%FT%TZ)\""

# Add initial and final quotes to the expression that will print each line
PRINT_EXP='"\"WEB",gensub("^http://([^/]*)/.*","\\1","g",$1),$1,datetime,0,"","","","","NONE",substr($2,1,10),substr($2,1,10),$2,int(rand()*999999999999999),"""\""'

OUTPUT_FILE=${5:-${LANGUAGE}_${SEARCH_WORDS//\|/-}_$(date +%Y%m%d)_$(date +%Y%m%d).csv}

awk "BEGIN {${CONFIGURATION}; print ${HEADER}} ${MATCH_EXP} { ${DATETIME}; print ${PRINT_EXP} }" $1 > ${OUTPUT_FILE}

echo "Generated CSV file ${OUTPUT_FILE}"
