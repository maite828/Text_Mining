use ${hiveconf:DATABASE};

LOAD DATA LOCAL INPATH "${hiveconf:DATA_PATH}/${hiveconf:F0_DIR}/" OVERWRITE INTO TABLE pdi_f0intensity_tmp;
INSERT OVERWRITE TABLE pdi_f0intensity SELECT split(split(INPUT__FILE__NAME,"/")[8], "\\.")[0],* FROM pdi_f0intensity_tmp;

-- LOAD DATA LOCAL INPATH "${hiveconf:DATA_PATH}/${hiveconf:RTTM_DIR}/" OVERWRITE INTO TABLE rttm_iir_smooth;

--LOAD DATA LOCAL INPATH "${hiveconf:DATA_PATH}/${hiveconf:SRATE_DIR}/" OVERWRITE INTO TABLE pdi_speech_rate_tmp;
--INSERT OVERWRITE TABLE pdi_speech_rate SELECT TRIM(soundname), TRIM(spkr), CAST(TRIM(segmentID) AS INT), CAST(TRIM(syll) AS INT),
--                                         CAST(TRIM(npause) AS FLOAT), CAST(TRIM(duration) AS FLOAT),
--                                         CAST(TRIM(phonationtime) AS FLOAT), CAST(TRIM(speechrate) AS FLOAT),
--                                         CAST(TRIM(articulation_rate) AS FLOAT), CAST(TRIM(asd) AS FLOAT)
--                                  FROM pdi_speech_rate_tmp;

--LOAD DATA LOCAL INPATH "${hiveconf:DATA_PATH}/${hiveconf:SPK_DIR}/" OVERWRITE INTO TABLE pdi_trans_tmp; 
-- 0.12 hive does not support non constant expressions when indexing arrays (8 is calculated manually :>)
--INSERT OVERWRITE TABLE pdi_trans SELECT split(split(INPUT__FILE__NAME,"/")[8], "\\.")[0],* FROM pdi_trans_tmp;
