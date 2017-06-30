CREATE DATABASE IF NOT EXISTS pdi_lleida;
add jar /home/zoraida/jars/csv-serde-1.1.2-0.11.0-all.jar;

-- Table for loading Luq features
CREATE TABLE pdi_audio_features(
response string,
cust_mean_articulation_rate decimal,
cust_sd_articulation_rate decimal,
age_mean_articulation_rate decimal,
age_sd_articulation_rate decimal,
cust_mean_f0 decimal,
cust_sd_f0 decimal,
cust_mean_loudness decimal,
cust_sd_loudness decimal,
SID_Key decimal,
age_mean_f0 decimal,
age_sd_f0 decimal,
age_mean_loudness decimal,
age_sd_loudness decimal
) ROW FORMAT SERDE 'com.bizo.hive.serde.csv.CSVSerde'
WITH serdeproperties (
   'separatorChar' = ',',
   'quoteChar' = '"',
   'escapeChar' = '\\'
)
STORED AS TEXTFILE;

-- f0intensity directory
CREATE TABLE pdi_f0intensity_tmp(
frameIndex int,
frameTime decimal,
voiceprob_sma decimal,
f0_sma decimal,
pcm_loudness decimal
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

CREATE TABLE pdi_f0intensity(
id string,
frameIndex int,
frameTime decimal,
voiceprob_sma decimal,
f0_sma decimal,
pcm_loudness decimal
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

-- speech_rate directory
CREATE TABLE pdi_speech_rate_tmp(
  soundname string,
  spkr string,
  segmentID string,
  syll string,
  npause string,
  duration string,
  phonationtime string,
  speechrate string,
  articulation_rate string,
  asd string
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

CREATE TABLE pdi_speech_rate(
  soundname string,
  spkr string,
  segmentID int,
  syll int,
  npause float,
  duration float,
  phonationtime float,
  speechrate float,
  articulation_rate float,
  asd float
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;


-- rtt_iir_smooth directory
CREATE TABLE rttm_iir_smooth(
  speaker string,
  sid_key string,
  channel int,
  start_sec float,
  duration float,
  ignore1 string,
  ignore2 string,
  locutor string,
  ignore3 string
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

-- Datawarehouse information related to the calls. 'tipo' ['A'|'B']
CREATE TABLE pdi_dwh(
  tipo string,
  telefono string,
  fecha_contacto string,
  det string,
  motivo_baja string,
  arpu string
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

-- Transcription meta information. 'sid_key': audio file id
CREATE  TABLE pdi_meta(
  id1 string,
  id2 string,
  sid_key string,
  espera string,
  conferencia string,
  transferidas string,
  tiempo_espera string,
  tiempo_final string,
  direccion string,
  tlf string,
  dnis string,
  pbx string,
  conmutador string,
  idconmutador string,
  extension string,
  excepcion string,
  ucid string,
  vdn string,
  skill string,
  campana string,
  canalcelulaterritorio string,
  tiemposilencio string,
  timetmp string,
  duration string,
  baja string,
  keywords string,
  agents string)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

CREATE TABLE pdi_trans(
  id string,
  word string,
  start_sec float,
  end_sec float,
  locutor string
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

CREATE TABLE pdi_trans_tmp(
  word string,
  start_sec float,
  end_sec float,
  locutor string 
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

