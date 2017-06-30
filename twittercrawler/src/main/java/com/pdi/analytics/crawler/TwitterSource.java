package com.pdi.analytics.crawler;

import java.util.HashMap;
import java.util.Map;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.EventDrivenSource;
import org.apache.flume.channel.ChannelProcessor;
import org.apache.flume.conf.Configurable;
import org.apache.flume.event.EventBuilder;
import org.apache.flume.source.AbstractSource;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import twitter4j.FilterQuery;
import twitter4j.StallWarning;
import twitter4j.Status;
import twitter4j.StatusDeletionNotice;
import twitter4j.StatusListener;
import twitter4j.TwitterStream;
import twitter4j.TwitterStreamFactory;
import twitter4j.conf.ConfigurationBuilder;
import twitter4j.json.DataObjectFactory;

/**
 * A Flume Source, which pulls data from Twitter's streaming API. Currently,
 * this only supports pulling from the sample API, and only gets new status
 * updates.
 */
public class TwitterSource extends AbstractSource implements EventDrivenSource, Configurable {

    private static final Logger logger = LoggerFactory.getLogger(TwitterSource.class);

    /** Information necessary for accessing the Twitter API */
    private String consumerKey;
    private String consumerSecret;
    private String accessToken;
    private String accessTokenSecret;

    private String[] trackList;
    private String[] languageList;

    /** The actual Twitter stream. It's set up to collect raw JSON data */
    private TwitterStream twitterStream;

    /**
     * The initialization method for the Source. The context contains all the
     * Flume configuration info, and can be used to retrieve any configuration
     * values necessary to set up the Source.
     */
    @Override
    public void configure(Context context) {
        consumerKey = context.getString(TwitterSourceConstants.CONSUMER_KEY_KEY);
        consumerSecret = context.getString(TwitterSourceConstants.CONSUMER_SECRET_KEY);
        accessToken = context.getString(TwitterSourceConstants.ACCESS_TOKEN_KEY);
        accessTokenSecret = context.getString(TwitterSourceConstants.ACCESS_TOKEN_SECRET_KEY);
        logger.info("Configuring Twitter source using consumer key {} and" + " access token {}", new String[] {
                consumerKey, accessToken });

        String track = context.getString(TwitterSourceConstants.TRACK_KEY, "");
        trackList = track.split(TwitterSourceConstants.SEPARATOR_DEFAULT);
        for (int i = 0; i < trackList.length; i++) {
            trackList[i] = trackList[i].trim();
        }
        logger.info("Configuring Twitter source to track keywors  {} ", track);

        String language = context.getString(TwitterSourceConstants.LANGUAGE_KEY,
                TwitterSourceConstants.LANGUAGE_DEFAULT);
        languageList = language.split(TwitterSourceConstants.SEPARATOR_DEFAULT);
        logger.info("Configuring Twitter source to track tweets in languages  {} ", language);

        ConfigurationBuilder cb = new ConfigurationBuilder();
        cb.setOAuthConsumerKey(consumerKey);
        cb.setOAuthConsumerSecret(consumerSecret);
        cb.setOAuthAccessToken(accessToken);
        cb.setOAuthAccessTokenSecret(accessTokenSecret);
        cb.setJSONStoreEnabled(true);
        cb.setIncludeEntitiesEnabled(true);

        twitterStream = new TwitterStreamFactory(cb.build()).getInstance();
    }

    /**
     * Start processing events. This uses the Twitter Streaming API to sample
     * Twitter, and process tweets.
     */
    @Override
    public void start() {
        final ChannelProcessor channel = getChannelProcessor();

        final Map<String, String> headers = new HashMap<String, String>();

        // The StatusListener is a twitter4j API, which can be added to a
        // Twitter
        // stream, and will execute methods every time a message comes in
        // through
        // the stream.
        StatusListener listener = new StatusListener() {
            // The onStatus method is executed every time a new tweet comes in.
            public void onStatus(Status status) {
                logger.debug("Tweet [user: {}], [text: {}]",
                        new String[] { status.getUser().getScreenName(), status.getText() });

                headers.put("timestamp", String.valueOf(status.getCreatedAt().getTime()));
                Event event = EventBuilder.withBody(DataObjectFactory.getRawJSON(status).getBytes(), headers);

                channel.processEvent(event);

            }

            // This listener will ignore everything except for new tweets
            public void onDeletionNotice(StatusDeletionNotice statusDeletionNotice) {
                logger.warn("DELETION NOTICE [statusDeletionNotice: {}]", statusDeletionNotice.getStatusId());
            }

            public void onTrackLimitationNotice(int numberOfLimitedStatuses) {
                logger.warn("TRACK LIMITATION NOTICE [numberOfLimitedStatuses: {}]", numberOfLimitedStatuses);
            }

            public void onScrubGeo(long userId, long upToStatusId) {
                logger.warn("ON SCRUB GEO [userId: {}], [upToStatusId: {}]", new String[] { String.valueOf(userId),
                        String.valueOf(upToStatusId) });
            }

            public void onException(Exception ex) {
                logger.error(ex.getMessage());
                ex.printStackTrace();
            }

            public void onStallWarning(StallWarning warning) {
                logger.warn(
                        "ON STALL WARNING [code: {}], [message: {}], [percent full:{}]",
                        new String[] { warning.getCode(), warning.getMessage(),
                                String.valueOf(warning.getPercentFull()) });
            }
        };

        logger.debug("Setting up Twitter sample stream using consumer key {} and" + " access token {}", new String[] {
                consumerKey, accessToken });
        // Set up the stream's listener (defined above),
        twitterStream.addListener(listener);

        FilterQuery query = new FilterQuery().track(trackList);
        query.language(languageList);
        twitterStream.filter(query);

        super.start();
    }

    /**
     * Stops the Source's event processing and shuts down the Twitter stream.
     */
    @Override
    public void stop() {
        logger.debug("Shutting down Twitter sample stream...");
        twitterStream.shutdown();
        super.stop();
    }
}
