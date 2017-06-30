ADD JAR /home/zoraida/hive-serdes-1.0-SNAPSHOT.jar;
use bt;
# Number of total tweets in the table
select count(*) from tweets;

# Number of tweets that
# are not posted by oficial users
# are not retweets
# contains keywords: telefonica, telefónica or movistar
SELECT COUNT(*) 
FROM tweets
LEFT OUTER JOIN oficialusers ON lower(tweets.user.screen_name) = lower(oficialusers.screen_name)
WHERE oficialusers.screen_name IS NULL AND retweeted_status is NULL
AND (locate('telefonica',lower(tweets.text)) > 0 OR locate('telefónica',lower(tweets.text))> 0 OR locate('movistar',lower(tweets.text))> 0)

# Number of tweets that
# are not posted by oficial users
# are not retweets
# contains keywords: telefonica, telefónica or movistar
# are replies to other tweets
SELECT nreplies, in_reply_to_status_id_str
FROM(
SELECT COUNT(*) AS nreplies, t1.in_reply_to_status_id_str AS in_reply_to_status_id_str
FROM tweets t1
LEFT OUTER JOIN oficialusers ON lower(t1.user.screen_name) = lower(oficialusers.screen_name)
LEFT OUTER JOIN (SELECT id_str
                 FROM tweets
                 WHERE retweeted_status is NULL AND (locate('telefonica',lower(tweets.text)) > 0 OR locate('telefónica',lower(tweets.text))> 0 OR locate('movistar',lower(tweets.text))> 0)) t2
                 ON t2.id_str = t1.in_reply_to_status_id_str
WHERE oficialusers.screen_name IS NULL AND t1.retweeted_status is NULL
AND t2.id_str IS NOT NULL
AND (locate('telefonica',lower(t1.text)) > 0 OR locate('telefónica',lower(t1.text))> 0 OR locate('movistar',lower(t1.text))> 0)
AND t1.in_reply_to_status_id_str != ''
GROUP BY t1.in_reply_to_status_id_str)z
WHERE nreplies > 25;

# Different users:
SELECT COUNT(*)
FROM (SELECT distinct tweets.user.id_str
      FROM tweets
      LEFT OUTER JOIN oficialusers ON lower(tweets.user.screen_name) = lower(oficialusers.screen_name)
      WHERE oficialusers.screen_name IS NULL AND retweeted_status is NULL
      AND (locate('telefonica',lower(tweets.text)) > 0 OR locate('telefónica',lower(tweets.text))> 0 OR locate('movistar',lower(tweets.text))> 0)
     )tmp;


# Number of users that have post 25 or more tweets.
# are not posted by oficial users
# are not retweets
# contains keywords: telefonica, telefónica or movistar
SELECT COUNT(*)
FROM (SELECT COUNT(*) AS tweetsByUser, user.id_str
      FROM tweets 
      LEFT OUTER JOIN oficialusers ON lower(tweets.user.screen_name) = lower(oficialusers.screen_name)
      WHERE oficialusers.screen_name IS NULL AND retweeted_status is NULL
      AND (locate('telefonica',lower(tweets.text)) > 0 OR locate('telefónica',lower(tweets.text))> 0 OR locate('movistar',lower(tweets.text))> 0)
      GROUP BY user.id_str
     )tmp 
WHERE tweetsByUser >= 25;


# Doc is a set of tweets belonging to the same user. 
# We estimate that one tweet can have 4 significant words and a doc must have at least 100 significant words. Then, we only select those users that have 25 tweets or more.
# are not posted by oficial users
# are not retweets
# contains keywords: telefonica, telefónica or movistar

INSERT OVERWRITE LOCAL DIRECTORY '/home/zoraida/docsByUser.json' SELECT CONCAT('{"text":"', doc, '","ids":["',ids,'"],"user":{"id_str":"', user_id,'","screen_name":"',screen_name,'"}}')
FROM (SELECT COUNT(*) AS tweetsByUser, tweets.user.id_str AS user_id, tweets.user.screen_name AS screen_name,  concat_ws('.', collect_set(tweets.text)) AS doc, concat_ws('","', collect_set(tweets.id_str)) AS ids
      FROM tweets 
      LEFT OUTER JOIN oficialusers ON lower(tweets.user.screen_name) = lower(oficialusers.screen_name)
      WHERE oficialusers.screen_name IS NULL AND retweeted_status is NULL
      AND (locate('telefonica',lower(tweets.text)) > 0 OR locate('telefónica',lower(tweets.text))> 0 OR locate('movistar',lower(tweets.text))> 0)
      GROUP BY tweets.user.id_str, tweets.user.screen_name
     )tmp 
WHERE tweetsByUser >= 25;

# Select tweets that contains emoticons
SELECT COUNT(*) as num, emoticons.id
FROM tweets
LEFT OUTER JOIN emoticons
WHERE locate(CONCAT(' ',emoticons.id),tweets.text) > 0
and retweeted_status is NULL
group by emoticons.id
order by num DESC
limit 50;

#Number of tweets that are no retweets
SELECT COUNT(*)
FROM tweets
WHERE retweeted_status is NULL;

SELECT count(*)
FROM tweets
WHERE date < '2012-04-11'
and retweeted_status is NULL
6966

SELECT count(*) AS num, emoticons.id, tweets.sentiments.type, tweets.sentiments.value
FROM tweets
LEFT OUTER JOIN emoticons
WHERE locate(emoticons.id,tweets.text) > 0
AND retweeted_status IS NULL AND tweets.date < '2012-04-11'
GROUP BY emoticons.id, tweets.sentiments.type, tweets.sentiments.value
ORDER BY num DESC
LIMIT 200

