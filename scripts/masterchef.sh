if [ $# -ne 3 ]; then
        echo " ERROR: No arguments supplied"
	echo " Usage:  $0 option input_dir output_dir "
	echo "		  option: (0:prepare | 1:cook | 2:serve) "	
        echo "         $0 0 /user/zoraida/text-mining/tweets/raw/2014/04/0?/??/* user/zoraida/text-mining/tweets/unique "
        exit 20
fi

OPTION=$1
INPUT_DIR=$2
OUTPUT_DIR=$3

# Home directory
if [ -z "$TEXTMINING_HOME" ]; then
    TEXTMINING_HOME='/home/zoraida/analytics-text-mining'
fi

# Unique config file. 
CONFIG_FILE=${TEXTMINING_HOME}'/masterchef/conf/masterchef.ini'

# Dependencies
YAML_LIBRARY=${TEXTMINING_HOME}'/masterchef/conf/yaml.zip'
NLTK_LIBRARY=${TEXTMINING_HOME}'/masterchef/conf/nltk.zip'
DEPENDENCIES=${{TEXTMINING_HOME}'/masterchef/coo2cfg.py,'${TEXTMINING_HOME}'/masterchef/langdetector.py,'${TEXTMINING_HOME}'/masterchef/twittercook.py,'${YAML_LIBRARY}','${NLTK_LIBRARY}
FEATHERS_JAR=${TEXTMINING_HOME}'/masterchef/conf/feathers.jar'
HADOOP_HOME='/usr/lib/hadoop'

# Stop words. Uncompress, edit and compress to add more words
STOPWORDS_FILE=${TEXTMINING_HOME}'/masterchef/conf/stopwords.zip'

# Weird but needed to work
ln -s ${CONFIG_FILE}  masterchef.ini
ln -s ${STOPWORDS_FILE} stopwords.zip
ln -s ${NLTK_LIBRARY} nltk.zip
ln -s ${YAML_LIBRARY} yaml.zip


case "$OPTION" in

0)  SCRIPT=${TEXTMINING_HOME}'/masterchef/preparing.py'
    dumbo start ${SCRIPT} \
            -libjar ${FEATHERS_JAR} \
            -getpath yes \
            -file ${CONFIG_FILE} \
            -files ${DEPENDENCIES} \
            -input ${INPUT_DIR} \
            -output ${OUTPUT_DIR} \
            -overwrite yes  \
            -hadoop ${HADOOP_HOME}
    ;;

1)  SCRIPT=${TEXTMINING_HOME}'/masterchef/cooking.py'
    dumbo start ${SCRIPT} \
            -libjar ${FEATHERS_JAR} \
            -getpath yes \
            -file ${STOPWORDS_FILE} \
            -file ${CONFIG_FILE} \
            -files ${DEPENDENCIES} \
            -input ${INPUT_DIR} \
            -output ${OUTPUT_DIR} \
            -overwrite yes  \
            -hadoop ${HADOOP_HOME}
    ;;
     
2)  SCRIPT=${TEXTMINING_HOME}'/masterchef/serving.py'
    dumbo start ${SCRIPT} \
            -file ${CONFIG_FILE} \
            -file ${STOPWORDS_FILE} \
            -files ${DEPENDENCIES} \
	    -input ${INPUT_DIR} \
	    -output ${OUTPUT_DIR} \
	    -overwrite yes  \
	    -hadoop ${HADOOP_HOME}
    ;;
*) echo "WRONG option: $OPTION. Use option: (0:prepare | 1:cook | 2:serve)"
   ;;

esac

if [ $? -ne 0 ]; then
    echo "Error running: ${SCRIPT}"
    exit 20
fi

rm masterchef.ini
rm stopwords.zip
rm nltk.zip
rm yaml.zip
