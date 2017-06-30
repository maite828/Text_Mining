#!/bin/bash
set -u
set -e

# This script runs the Nutch crawler and cleans the output as an input for subsequent processes
# It runs the sequence as defined in COMMANDS_SEQUENCE variable:
# -Nutch crawl
# -Database dump
# -Language detection
# -Output file clean 
# -Print statistics
# In case the process fails at some point, it can be run again indicating the recovery point
# of the command sequence. Partial results from the last successful step may be required as parameters. 


BIN_DIR=$(dirname $0)
source ${BIN_DIR}/conf/config.properties
source ${BIN_DIR}/utils.sh

########## FUNCTIONS ##########

# Parameters:
# They are passed to this function as they come to the script
function check_input_parameters {
    if [ $# -eq 0 ]; then
        echo "ERROR: invalid arguments" 
        echo "Usage: $(basename $0) <command> [--help] [command-args]"
        echo "   command must be one of (${COMMANDS_SEQUENCE[@]})"
        echo "   --help will show help for the specific command"
        exit 1
    fi

    COMMAND=$1
    case ${COMMAND} in
        "crawl" )
            if [ $# -ne 4 ] || [ "$2" == "--help" ]; then
                echo "Usage: $(basename $0) ${COMMAND} <crawler-name> <seed-file> <num-iterations>"
                echo "   seed-file is relative to ${NUTCH_SEEDS_DIR}"
                exit 1
            fi
            CRAWLER_NAME=$2
            NUTCH_SEED_FILE=${NUTCH_SEEDS_DIR}/$3
            NUTCH_ITERATIONS=$4
            if [ ! -f ${NUTCH_SEED_FILE} ]; then
                echo "ERROR: Seed file ${NUTCH_SEED_FILE} does not exist"
                exit 1
            fi
            ;;
        "dump" )
            if [ $# -ne 2 ] || [ "$2" == "--help" ]; then
                echo "Usage: $(basename $0) ${COMMAND} <crawler-name>"
                exit 1
            fi
            CRAWLER_NAME=$2
            ;;
        "detlang" )
            if [ $# -ne 3 ] || [ "$2" == "--help" ]; then
                echo "Usage: $(basename $0) ${COMMAND} <crawler-name> <dump-file>"
                exit 1
            fi 
            CRAWLER_NAME=$2
            DUMP_OUTPUT_FILE=$3
            DUMP_OUTPUT_DIR=$(dirname ${DUMP_OUTPUT_FILE})
            ;;
        "clean" )
            if [ $# -ne 3 ] || [ "$2" == "--help" ]; then
                echo "Usage: $(basename $0) ${COMMAND} <crawler-name> <det_id-file>"
                exit 1
            fi 
            CRAWLER_NAME=$2
            DETECT_LANG_OUTPUT_FILE=$3
            # Remove det-id suffix to get original dump file
            DUMP_OUTPUT_FILE=${DETECT_LANG_OUTPUT_FILE%.$DET_ID_OUTPUT_SUFFIX}
            DUMP_OUTPUT_DIR=$(dirname ${DUMP_OUTPUT_FILE})
            ;;
        "stats" )
            echo "ERROR: ${COMMAND} is not implemented for recovery"
            echo "Use print-dump-stats.sh and print-crawl-stats.sh instead"
            exit 1
            ;;
        * )
            echo "ERROR: ${COMMAND} is not a valid command"
            echo "Command must be one of (${COMMANDS_SEQUENCE[@]})"
            exit 1
            ;;
    esac
}

function crawl {
    log "Starting crawler ${CRAWLER_NAME}...." 
    log "Seed file: ${NUTCH_SEED_FILE}"
    log "Number of URLs in seed file: $(egrep -c ^http ${NUTCH_SEED_FILE})"

    PREVIOUS_DIR=$(pwd)
    cd ${NUTCH_RUNTIME_DIR}

    ./bin/crawl ${NUTCH_SEED_FILE} ${CRAWLER_NAME} ${SOLR_URL} ${NUTCH_ITERATIONS}
    if [ $? -ne 0 ]; then
        cd ${PREVIOUS_DIR}
        log "ABORT: Crawling finished with errors."
        exit 1
    fi

    cd ${PREVIOUS_DIR}
    log "Crawling finished"
}

# Parameters:
# $1: Dump output dir name
function dump {
    log "Starting ${CRAWLER_NAME} dump...."
    PREVIOUS_DIR=$(pwd)
    cd ${NUTCH_RUNTIME_DIR}
    
    # Dump fails if the output directory exists
    # First check if it exists and rename it in that case
    if rename_existing_file $1 $1.${DUMP_BACKUP_EXTENSION}; then
        log "Output dir $1 already exists"
        log "Renamed old $1 to $1.${DUMP_BACKUP_EXTENSION}"
    fi
    ./bin/nutch readdb -dump $1 -crawlId ${CRAWLER_NAME} -text
    if [ $? -ne 0 ]; then
        cd ${PREVIOUS_DIR}
        log "ABORT: ${CRAWLER_NAME} dump finished with errors."
        exit 1
    fi
    
    cd ${PREVIOUS_DIR}
    log "${CRAWLER_NAME} dump finished. Output dir: $1"
}

# Parameters:
# $1: Input file name
# $2: Output file name
function detect_language {
    log "Starting TID Language Detector...."
    log "Language detector - Input file: $1"
    log "Language detector - Output file: $2"
        
    export LD_LIBRARY_PATH=${TID_LANGUAGE_DETECTOR_DIR}/libs
    ${TID_LANGUAGE_DETECTOR_DIR}/bin/det_id_nutch2 -m ${TID_LANGUAGE_DETECTOR_DIR}/modid_bin $1 $2 
    RESULT=$?
    if [ ${RESULT} -ne 0 ]; then
        echo "ABORT: Language Detector finished with errors"
        exit 1
    fi

    log "Language Detector finished. Output file: $2"
}

# Parameters:
# $1: Input file
# $2: Output file. If ommited, changes are made to the original file 
function clean_dump {
    log "Starting dump clean up...."
    log "Dump clean - Input file: $1"
    log "Dump clean - Output file: $2"

    # STEP 1: Clean URLs in the text only is flag is activated by configuration
    log "Cleaning crawled text:"
    # The variable to group the operations to be executed by sed
    SED_CLEAN_TEXT_CMD=""

    log "...replace special characters from this list: ${CLEAN_CHARS_LIST}"
    if [ "${CLEAN_TEXT_URLS}" == "Y" ]; then
        log "...replace URL elements (${CLEAN_URL_ELEMENTS_LIST}) by spaces"
        CLEAN_CHARS_LIST="${CLEAN_CHARS_LIST}|${CLEAN_URL_ELEMENTS_LIST}"
    fi
    # List separator '|' must be escaped to sed; thus we replace each occurrence of '|' in the list by '\|'
    SED_CLEAN_TEXT_CMD="${SED_CLEAN_TEXT_CMD}; s/${CLEAN_CHARS_LIST//|/\\|}/ /g"

    if [ "${CLEAN_TO_LOWERCASE}" == "Y" ]; then
        log "...replace text to lowercase"
        SED_CLEAN_TEXT_CMD="${SED_CLEAN_TEXT_CMD}; s/^\(.*\)$/\L\1/"
    fi

    if [ "${CLEAN_NUMBERS}" == "Y" ]; then
        log "...delete all numbers from text"
        SED_CLEAN_TEXT_CMD="${SED_CLEAN_TEXT_CMD}; s/[0-9]*//g"
    fi

    # NOTE: mind that this deletes also punctuation in text tags and may produce subsequent failures 
    if [ "${CLEAN_PUNCTUATION}" == "Y" ]; then
        log "...delete all puntuation characters"
        SED_CLEAN_TEXT_CMD="${SED_CLEAN_TEXT_CMD}; s/[[:punct:]]/ /g"
    fi

    # This must be the last to be executed, as previous substitutions may lead to empty texts
    log "...delete blank lines in text"
    SED_CLEAN_TEXT_CMD="${SED_CLEAN_TEXT_CMD}; s/^[ \t]*$/x/g"

    # Now execute the previous sed commands to the texts
    sed "/${TAG_TEXT_START}/,/${TAG_TEXT_END}/ { ${SED_CLEAN_TEXT_CMD} }" $1 > $2

    # STEP 2: Now clean the Nutch output
    log "Cleaning records without language detected"
    # Keep only records which have passed language detection (temporary file is needed)
    awk "/${TAG_LANGUAGE_ID}/" RS= ORS="\n\n" $2 > $2.tmp && mv $2.tmp $2

    log "Align text with '${TAG_TEXT_START}' tag"
    # Put the text in the same line as tag replacing EOL by tab character
    sed -i ":a; N; \$!ba; s/\(${TAG_TEXT_START}\S*\)\n/\1\t/g" $2
    # In case there was no text, the end tag is now aligned with start tag
    log "Replace empty texts with 'x' character (to ease visual checking)"
    sed -i "s/${TAG_TEXT_END}/x/g" $2

    log "Cleaning unwanted field lines:"
    echo "...keep fields in ${INCLUDE_FIELDS_LIST}"
    # Delete each line not including any of the fields in the Include List
    # We need to escape the | character in the expression passed to sed command
    # Thus we replace each occurrence of '|' in the list by '\|'
    sed -i "/${INCLUDE_FIELDS_LIST//|/\\|}/!d" $2
    
    log "Cleaning field tags:"
    echo "...clean fields in ${CLEAN_TAGS_LIST}"
    # Remove each occurrence of any of the items in the Clean Tags List (keep the rest of the line)
    # We need to escape the | character in the expression passed to sed command
    # Thus we replace each occurrence of '|' in the list by '\|'
    sed -i "s/${CLEAN_TAGS_LIST//|/\\|}//g" $2
        
    log "Dump clean up done"
}

########## MAIN ##########


check_input_parameters "$@"

### Create output directory and set logging file ###
# Write output into file
# We cannot create the dump dir and log into it now because dump process creates it and fails if it previously exists
CRAWL_OUT_FILE_NAME=${CRAWLER_NAME}_${CRAWL_OUT_FILE_SUFFIX}
echo "Logging to ${CRAWL_OUT_FILE_NAME}"
exec 1>${CRAWL_OUT_FILE_NAME} 2>&1

### Detect start of the commands sequence ###
COMMAND_INDEX=0
# Find the first command in the running sequence
for ITEM in "${COMMANDS_SEQUENCE[@]}"; do
    if [ "${COMMAND}" == "${ITEM}" ]; then
        break;
    fi  
    COMMAND_INDEX=$((COMMAND_INDEX+1))
done

# Run only commands starting from the passed as parameter
for COMMAND in "${COMMANDS_SEQUENCE[@]:${COMMAND_INDEX}:${#COMMANDS_SEQUENCE[*]}}"; do
    case ${COMMAND} in
        "crawl" )
            crawl
            ;;
        "dump" )
            DUMP_OUTPUT_DIR=${DUMPS_DIR}/${CRAWLER_NAME}_${DUMP_DIR_SUFFIX}
            DUMP_OUTPUT_FILE=${DUMP_OUTPUT_DIR}/${DUMP_FILE_NAME}
            dump ${DUMP_OUTPUT_DIR}
            ;;
        "detlang" )
            DETECT_LANG_INPUT_FILE=${DUMP_OUTPUT_FILE}
            DETECT_LANG_OUTPUT_FILE=${DETECT_LANG_INPUT_FILE}.${DET_ID_OUTPUT_SUFFIX}
            detect_language ${DETECT_LANG_INPUT_FILE} ${DETECT_LANG_OUTPUT_FILE}
            ;;
        "clean" )
            CLEAN_DUMP_INPUT_FILE=${DETECT_LANG_OUTPUT_FILE}
            CLEAN_DUMP_OUTPUT_FILE=${CLEAN_DUMP_INPUT_FILE}.${CLEAN_OUTPUT_SUFFIX}
            clean_dump ${CLEAN_DUMP_INPUT_FILE} ${CLEAN_DUMP_OUTPUT_FILE}
            ;;
        "stats" )
            echo
            echo "Crawl statistics from file ${CRAWL_OUT_FILE_NAME}:"
            . ${BIN_DIR}/print-crawl-statistics.sh ${CRAWL_OUT_FILE_NAME}
            echo
            echo "Dump statistics from file ${DUMP_OUTPUT_FILE}:"
            . ${BIN_DIR}/print-dump-statistics.sh ${DUMP_OUTPUT_FILE}
            ;;
    esac
done

# Move generated log file to dump dir before exiting
mv ${CRAWL_OUT_FILE_NAME} ${DUMP_OUTPUT_DIR}/${CRAWL_OUT_FILE_NAME}
# Restore output redirection
exec 1>/dev/tty 2>&1
echo "Crawl finished. Find execution details in ${DUMP_OUTPUT_DIR}/${CRAWL_OUT_FILE_NAME}"
