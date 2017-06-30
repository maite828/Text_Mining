ADD JAR /home/zoraida/hive-serdes-1.0-SNAPSHOT.jar;

USE bt;

CREATE TABLE oficialusers (
    screen_name STRING
)

LOAD DATA LOCAL INPATH '/home/zoraida/analytics-text-mining/oficial_users.quitar'  INTO TABLE oficialusers

CREATE TABLE emoticons (
    id STRING
);

LOAD DATA LOCAL INPATH '/home/zoraida/unicode300.hql' INTO TABLE emoticons;
LOAD DATA LOCAL INPATH '/home/zoraida/unicode400.hql' INTO TABLE emoticons;
LOAD DATA LOCAL INPATH '/home/zoraida/unicode500.hql' INTO TABLE emoticons;
LOAD DATA LOCAL INPATH '/home/zoraida/unicode600.hql' INTO TABLE emoticons;
LOAD DATA LOCAL INPATH '/home/zoraida/plainemoticons.hql' INTO TABLE emoticons;

CREATE EXTERNAL TABLE tweets (
  id_str STRING,
  created_at STRING,
  source STRING,
  favorited BOOLEAN,
  retweeted_status STRUCT<
    text:STRING,
    user:STRUCT<screen_name:STRING,name:STRING>,
    retweet_count:INT,
    coordinates:STRING>,
  entities STRUCT<
    urls:ARRAY<STRUCT<expanded_url:STRING>>,
    user_mentions:ARRAY<STRUCT<screen_name:STRING,name:STRING>>,
    hashtags:ARRAY<STRUCT<text:STRING>>>,
  text STRING,
  user STRUCT<
    id_str:STRING,
    screen_name:STRING,
    name:STRING,
    friends_count:INT,
    followers_count:INT,
    statuses_count:INT,
    geo_enabled:BOOLEAN,
    verified:BOOLEAN,
    lang:STRING,
    location:STRING,
    utc_offset:INT,
    time_zone:STRING>,
  in_reply_to_status_id_str STRING,
  in_reply_to_user_id_str STRING,
  in_reply_to_screen_name STRING,
  topics ARRAY<STRING>,
  sentiments ARRAY<STRUCT<type:STRING,value:STRING, entity:STRING>>
) 
PARTITIONED BY (date STRING)
ROW FORMAT SERDE 'com.cloudera.hive.serde.JSONSerDe'
LOCATION '/user/zoraida/text-mining/tweets/raw/';

#TASS tweets
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-01') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-01';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-02') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-02';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-03') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-03';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-04') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-04';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-05') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-05';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-06') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-06';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-07') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-07';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-08') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-08';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-09') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-09';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-10') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-10';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-11') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-11';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-12') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-12';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-13') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-13';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-14') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-14';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-15') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-15';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-16') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-16';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-17') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-17';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-18') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-18';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-19') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-19';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-20') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-20';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-21') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-21';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-22') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-22';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-23') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-23';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-24') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-24';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-25') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-25';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-26') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-26';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-27') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-27';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-28') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-28';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-29') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-29';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-30') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-30';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2011-12-31') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2011-12-31';

ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-01') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-01';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-02') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-02';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-03') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-03';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-04') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-04';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-05') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-05';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-06') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-06';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-07') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-07';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-08') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-08';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-09') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-09';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-10') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-10';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-11') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-11';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-12') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-12';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-13') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-13';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-14') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-14';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-15') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-15';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-16') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-16';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-17') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-17';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-18') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-18';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-19') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-19';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-20') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-20';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-21') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-21';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-22') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-22';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-23') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-23';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-24') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-24';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-25') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-25';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-26') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-26';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-27') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-27';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-28') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-28';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-29') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-29';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-30') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-30';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-01-31') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-01-31';

ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-01') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-01';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-02') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-02';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-03') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-03';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-04') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-04';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-05') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-05';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-06') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-06';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-07') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-07';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-08') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-08';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-09') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-09';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-10') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-10';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-11') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-11';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-12') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-12';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-13') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-13';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-14') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-14';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-15') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-15';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-16') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-16';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-17') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-17';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-18') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-18';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-19') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-19';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-20') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-20';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-21') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-21';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-22') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-22';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-23') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-23';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-24') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-24';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-25') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-25';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-26') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-26';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-27') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-27';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-28') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-28';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-02-29') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-02-29';


ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-01') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-01';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-02') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-02';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-03') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-03';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-04') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-04';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-05') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-05';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-06') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-06';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-07') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-07';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-08') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-08';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-09') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-09';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-10') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-10';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-11') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-11';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-12') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-12';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-13') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-13';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-14') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-14';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-15') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-15';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-16') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-16';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-17') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-17';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-18') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-18';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-19') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-19';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-20') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-20';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-21') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-21';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-22') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-22';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-23') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-23';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-24') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-24';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-25') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-25';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-26') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-26';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-27') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-27';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-28') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-28';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-29') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-29';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-30') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-30';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-03-31') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-03-31';

ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-01') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-01';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-02') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-02';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-03') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-03';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-04') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-04';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-05') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-05';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-06') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-06';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-07') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-07';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-08') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-08';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-09') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-09';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2012-04-10') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2012-04-10';


ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-02-25') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-02-25';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-02-26') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-02-26';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-02-27') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-02-27';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-02-28') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-02-28';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-03') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-03';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-04') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-04';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-05') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-05';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-06') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-06';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-07') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-07';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-08') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-08';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-09') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-09';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-10') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-10';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-18') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-18';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-19') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-19';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-20') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-20';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-21') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-21';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-22') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-22';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-23') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-23';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-24') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-24';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-25') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-25';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-26') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-26';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-27') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-27';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-28') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-28';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-29') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-29';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-30') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-30';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-03-31') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-03-31';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-01') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-01';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-02') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-02';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-03') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-03';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-04') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-04';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-05') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-05';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-06') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-06';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-07') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-07';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-08') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-08';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-09') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-09';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-10') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-10';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-11') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-11';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-12') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-12';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-13') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-13';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-14') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-14';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-15') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-15';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-16') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-16';
ALTER TABLE tweets ADD IF NOT EXISTS PARTITION (date='2014-04-22') LOCATION '/user/zoraida/text-mining/tweets/unique/date=2014-04-22';
