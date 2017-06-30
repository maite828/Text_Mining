INSERT OVERWRITE LOCAL DIRECTORY '/tmp/audio_text_lleida.csv'
SELECT response, cust_mean_articulation_rate decimal, cust_sd_articulation_rate decimal,
age_mean_articulation_rate decimal, age_sd_articulation_rate decimal,
cust_mean_f0 decimal, cust_sd_f0 decimal, cust_mean_loudness decimal,
cust_sd_loudness decimal, SID_Key decimal, age_mean_f0 decimal,
age_sd_f0 decimal, age_mean_loudness decimal, age_sd_loudness decimal, text
FROM pdi_audio_features audio
LEFT OUTER JOIN (SELECT SUBSTR(id, 5) AS id, CONCAT_WS(' ', COLLECT(word)) as text
                 FROM pdi_trans
                 GROUP BY id) trans ON trans.id = audio.SID_Key
WHERE text IS NOT NULL
