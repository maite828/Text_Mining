use pe_churn_mo;

-- Aux table for susana
create table pdi_anis_by_scene(
id string,
ani string,
downgrade int,
scene int
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

-- Table pdi_customer_status contains status of a client until may included
create table pdi_customer_status(
periodo string,
fecha string,
ANI string,
from_tipo string,
to_tipo string,
GRU_CLI_A string,
GRU_CLI_D string
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

INSERT INTO TABLE pdi_customer_status
SELECT PLA_FE_PERIODO, FEC_DETALLE, PLA_NU_TELEFO, from_tipo, to_tipo, GRU_CLI_A, GRU_CLI_D
FROM pdi_down

INSERT INTO TABLE pdi_customer_status
SELECT NULL, NULL, telefono, tipo, NULL, 
IF (segmento = 'Residencial', 'RES', 'NEG'), NULL
FROM pdi_not_down

-- Table pdi_down contains downgrades
create table pdi_down(
PLA_FE_PERIODO string,
PLA_ID_ANEXO string,
PLA_NU_TELEFO string,
FEC_DETALLE string,
from_tipo string,
to_tipo string,
GRU_CLI_A string,
GRU_CLI_D string
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201403_CC_CF.hql' INTO TABLE pdi_down;
 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201403_CC_CP.hql' INTO TABLE pdi_down;
 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201403_CF_CP.hql' INTO TABLE pdi_down;

 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201404_CC_CF.hql' INTO TABLE pdi_down;
 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201404_CC_CP.hql' INTO TABLE pdi_down;
 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201404_CF_CP.hql' INTO TABLE pdi_down;

 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201405_CC_CF.hql' INTO TABLE pdi_down;
 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201405_CC_CP.hql' INTO TABLE pdi_down;
 LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/Down/da_down_201405_CF_CP.hql' INTO TABLE pdi_down;

-- Table pdi_meta_calls contains meta from transcriptions
CREATE TABLE pdi_meta_calls(
id string,
StartTime string,
AgentID string,
ANI string,
CD7 string,
CD2 string, -- PROGRAMA
CD5 string, -- TIPO CLIENTE
CD16 string, -- SKILL
CD17 string, --ANI-CD
CD18 string, -- DNIS-CD
CD23 string, -- CONTACT CENTER
Duration int,
NumberOfHolds int, 
NumberOfConferences int, 
NumberOfTransfers int,
TotalHoldTime int,
WrapUpTime int,
TOTAL_AGENT_TALK_TIME int, 
NUM_AGENT_TALK_SEGMENTS int,
TOTAL_CUSTOMER_TALK_TIME int,
NUM_CUSTOMER_TALK_SEGMENTS int, 
TOTAL_SILENCE_TIME int,
NUM_SILENCE_SEGMENTS int,
TOTAL_TALK_OVER_TIME int, 
NUM_TALK_OVER_SEGMENTS int,
NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS int,
NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS int
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201405/201405.hql' INTO TABLE pdi_meta_calls;
LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201404/201404.hql' INTO TABLE pdi_meta_calls;
LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201403/201403.hql' INTO TABLE pdi_meta_calls;

-- This table contains transcriptions. Currently is not partitioned but it will be in the future
CREATE TABLE pdi_transcriptions_calls(
id string,
word string,
precision int,
start_sec int,
end_sec int,
locutor int,
)
ROW FORMAT DELIMITED
FIELDS TERMINATED BY '\t'
STORED AS TEXTFILE;

LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201403/transcriptions.hql' INTO TABLE pdi_transcriptions_calls;
LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201404/transcriptions.hql' INTO TABLE pdi_transcriptions_calls;
LOAD DATA LOCAL INPATH '/home/usertestpe/data/04_ChurnMovil/2.Speech_Analytics/201405/transcriptions.hql' INTO TABLE pdi_transcriptions_calls;
