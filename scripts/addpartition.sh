
DATE=`date +%Y-%m-%d -d "yesterday"`
QUERY="use bt;add jar /home/zoraida/hive-serdes-1.0-SNAPSHOT.jar;ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date = '${DATE}') LOCATION '/user/zoraida/text-mining/tweets/unique/${DATE}';"
hive -e $DATE
