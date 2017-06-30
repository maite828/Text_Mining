add jar /home/zoraida/hive/brickhouse-0.7.0-SNAPSHOT.jar;
CREATE TEMPORARY FUNCTION collect AS 'brickhouse.udf.collect.CollectUDAF';

SELECT count(*) as num_cust, num_days
FROM (SELECT DATEDIFF(from_unixtime(down_date,'yyyy-MM-dd'), from_unixtime(trans_datetime,'yyyy-MM-dd')) as num_days, ani
      FROM (SELECT down_date, MAX(trans_datetime) as trans_datetime, downs.ani as ani
            FROM(SELECT ani, MIN(unix_timestamp(fecha, "dd/MM/yyyy hh:mm:ss aa")) AS down_date
                 FROM pdi_customer_status
                 WHERE gru_cli_a='RES' AND gru_cli_d='RES' AND fecha IS NOT NULL
                      AND unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") >=
                          unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                      AND unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") <
                          unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                 GROUP BY ani
                 ) downs
            LEFT OUTER JOIN (SELECT ani, unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") AS trans_datetime
                             FROM pdi_meta_calls
                             WHERE unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") >=
                                   unix_timestamp("01/03/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                                   unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") <
                                   unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                                   AND cd2 <> 'DECISORES' AND cd2 <> 'MEDIO VALOR' AND cd2 <> 'NEG CORTE POR ROBO'
                                   AND cd2 <> 'NEG MASIVO' AND cd2 <> 'NEG TOP 1' AND cd2 <> 'NEGOCIOS CONVERGENTE'
                                   AND cd2 <> 'NEGOCIOS USUARIOS' AND cd2 <> 'Negocios Convergente'
                            ) meta on meta.ani = downs.ani
            WHERE meta.ani IS NOT NULL AND  trans_datetime < down_date
            GROUP BY downs.ani, down_date
           )tmp1
     )tmp
GROUP BY num_days;

-- Histogram:users that called once, users that called two, etc. 
SELECT count(*) as num_customers, num
FROM (SELECT count(*) as num, downs.ani
      FROM(SELECT ani, MIN(unix_timestamp(fecha, "dd/MM/yyyy hh:mm:ss aa")) AS down_date
           FROM pdi_customer_status
           WHERE gru_cli_a='RES' AND gru_cli_d='RES' AND fecha IS NOT NULL 
                 AND unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") >=
                     unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") 
                 AND unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") <
                     unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
           GROUP BY ani
          ) downs
      LEFT OUTER JOIN (SELECT NumeroOrigen, unix_timestamp(FechaInicioLlamada,"dd/MM/yyyy") AS call_date
                       FROM pdi_calls_by_ani calls) calls ON downs.ani = calls.NumeroOrigen
     GROUP BY downs.ani
    )tmp
GROUP BY num
ORDER BY num

-- Histogram:number of calls that where made 1 day before the downgrade date, 2 days before, etc.
SELECT 

SELECT down.pla_nu_telefo, 
FROM(SELECT pla_nu_telefo, fec_detalle
     FROM pdi_down
     WHERE gru_cli_a='RES' AND gru_cli_d='RES') down
LEFT OUTER JOIN pdi_calls_by_ani calls ON down.pla_nu_telefo = calls.NumeroOrigen

-- Total number of downgrades for residencial in the three month:
SELECT COUNT(*)
FROM(SELECT distinct pla_nu_telefo
     FROM pdi_down
     WHERE gru_cli_a='RES' AND gru_cli_d='RES') down;


-- Number of transcriptions that are from downgrades residencial in the three month:
SELECT COUNT(*) AS NUM
FROM (SELECT meta.id,
      unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") AS trans_datetime,
      unix_timestamp(down.fec_detalle,"dd/MM/yyyy hh:mm:ss aa") AS down_date
      FROM pdi_meta_calls meta
      LEFT OUTER JOIN (SELECT pla_nu_telefo, fec_detalle
                       FROM pdi_down
                       WHERE gru_cli_a='RES' AND gru_cli_d='RES') down ON down.pla_nu_telefo = meta.ani
      WHERE down.pla_nu_telefo IS NOT NULL
     )tmp
WHERE down_date IS NOT NULL AND trans_datetime <= down_date;


-- Number of transcriptions per program for downgrades with call date < downgrade date
SELECT count(*) as num, cd2
FROM ( SELECT meta.cd2,
              unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") AS trans_datetime,
              unix_timestamp(down.fec_detalle,"dd/MM/yyyy hh:mm:ss aa") AS down_date
              FROM pdi_meta_calls meta
              LEFT OUTER JOIN  (SELECT pla_nu_telefo, fec_detalle
                                FROM pdi_down
                                WHERE gru_cli_a='RES' AND gru_cli_d='RES') down
              ON down.pla_nu_telefo = meta.ani
              WHERE down. pla_nu_telefo IS NOT NULL
            ) tmp
WHERE trans_datetime <= down_date
GROUP BY cd2;


-- Number of transcriptions per program for downgrades with call date > downgrade date
SELECT count(*) as num, cd2
FROM ( SELECT meta.cd2,
              unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") AS trans_datetime,
              unix_timestamp(down.fec_detalle,"dd/MM/yyyy hh:mm:ss aa") AS down_date
              FROM pdi_meta_calls meta
              LEFT OUTER JOIN  (SELECT pla_nu_telefo, fec_detalle
                                FROM pdi_down
                                WHERE gru_cli_a='RES' AND gru_cli_d='RES') down
              ON down.pla_nu_telefo = meta.ani
              WHERE down. pla_nu_telefo IS NOT NULL
            ) tmp
WHERE trans_datetime > down_date
GROUP BY cd2;

    

-- (Escenario 1/2 | cosecha 3) llamadas en marzo y downgrade en mayo
--INSERT OVERWRITE LOCAL DIRECTORY '/tmp/meta_escenario12.csv'
INSERT OVERWRITE LOCAL DIRECTORY '/tmp/meta_escenario12_migrados.csv'
SELECT meta.id, tmp1.trans_datetime, 
IF (tmp1.downgrade <> esc2_cos3.target, unix_timestamp(esc2_cos3.fec_down, "yyyy-MM-dd"), tmp1.down_date), 
      meta.AgentID, meta.ANI, meta.CD7,meta.CD2,
      meta.CD5,meta.CD16,meta.CD17,meta.CD18,meta.CD23,meta.Duration,
      meta.NumberOfHolds,meta.NumberOfConferences,meta.NumberOfTransfers,meta.TotalHoldTime,meta.WrapUpTime,
      meta.TOTAL_AGENT_TALK_TIME,meta.NUM_AGENT_TALK_SEGMENTS,meta.TOTAL_CUSTOMER_TALK_TIME,
      meta.NUM_CUSTOMER_TALK_SEGMENTS,meta.TOTAL_SILENCE_TIME,meta.NUM_SILENCE_SEGMENTS,
      meta.TOTAL_TALK_OVER_TIME,meta.NUM_TALK_OVER_SEGMENTS,meta.NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS,
      meta.NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS,
IF(tmp1.downgrade <> esc2_cos3.target, esc2_cos3.target, tmp1.downgrade) as downgrade
--INSERT OVERWRITE TABLE pdi_anis_by_scene
--INSERT OVERWRITE TABLE pdi_anis_by_scene_migrados
--SELECT meta.id, meta.ani, tmp1.downgrade, 12
FROM pdi_meta_calls meta
LEFT OUTER JOIN (SELECT meta.ani, meta.trans_datetime, custom.down_date as down_date,
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           NULL, custom.down_date),
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           1,0) AS downgrade
                 FROM (SELECT ani,
                             MAX(unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")) AS trans_datetime
                       FROM pdi_meta_calls
                       WHERE unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") >=
                             unix_timestamp("01/03/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                             unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") <
                             unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                       GROUP BY ani
                            ) meta
                 LEFT OUTER JOIN (SELECT ani,
                                         MIN(unix_timestamp(fecha, "dd/MM/yyyy hh:mm:ss aa")) AS down_date
                                  FROM pdi_customer_status
                                  WHERE gru_cli_a='RES' AND (gru_cli_d='RES' OR gru_cli_d IS NULL) AND
                                        (fecha IS NULL OR (unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") >=
                                        unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                                        unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") <
                                        unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss")))
                                  GROUP BY ani
                                 ) custom ON custom.ani = meta.ani
                 WHERE custom.ani IS NOT NULL
                ) tmp1 ON tmp1.ani = meta.ani AND 
                          tmp1.trans_datetime = unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")
LEFT OUTER JOIN pe_down_mo.esc2_cos3_data_mig esc2_cos3 ON esc2_cos3.numerotelefono = meta.ani
--LEFT OUTER JOIN pe_down_mo.esc2_cos3_data_all esc2_cos3 ON esc2_cos3.numerotelefono = meta.ani
WHERE tmp1.ani IS NOT NULL AND esc2_cos3.numerotelefono IS NOT NULL
--WHERE tmp1.ani IS NOT NULL
--AND cd2 <> 'DECISORES' AND cd2 <> 'MEDIO VALOR' AND cd2 <> 'NEG CORTE POR ROBO'
--AND cd2 <> 'NEG MASIVO' AND cd2 <> 'NEG TOP 1' AND cd2 <> 'NEGOCIOS CONVERGENTE'
--AND cd2 <> 'NEGOCIOS USUARIOS' AND cd2 <> 'Negocios Convergente'
AND (cd2 = 'MIGRADOS' OR cd2 = 'RETENCION RES Y MIG')
ORDER BY id;


-- (Escenario 4/2 | cosecha 3) llamadas en marzo y downgrade en abril
--INSERT OVERWRITE LOCAL DIRECTORY '/tmp/meta_escenario34.csv'
INSERT OVERWRITE LOCAL DIRECTORY '/tmp/meta_escenario34_migrados.csv'
SELECT meta.id, tmp1.trans_datetime, 
IF (tmp1.downgrade <> esc4_cos2.target, unix_timestamp(esc4_cos2.fec_down, "yyyy-MM-dd"), tmp1.down_date) as down_date,
      meta.AgentID, meta.ANI, meta.CD7,meta.CD2,
      meta.CD5,meta.CD16,meta.CD17,meta.CD18,meta.CD23,meta.Duration,
      meta.NumberOfHolds,meta.NumberOfConferences,meta.NumberOfTransfers,meta.TotalHoldTime,meta.WrapUpTime,
      meta.TOTAL_AGENT_TALK_TIME,meta.NUM_AGENT_TALK_SEGMENTS,meta.TOTAL_CUSTOMER_TALK_TIME,
      meta.NUM_CUSTOMER_TALK_SEGMENTS,meta.TOTAL_SILENCE_TIME,meta.NUM_SILENCE_SEGMENTS,
      meta.TOTAL_TALK_OVER_TIME,meta.NUM_TALK_OVER_SEGMENTS,meta.NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS,
      meta.NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS, 
IF(tmp1.downgrade <> esc4_cos2.target, esc4_cos2.target, tmp1.downgrade) as downgrade
--INSERT INTO TABLE pdi_anis_by_scene
--INSERT INTO TABLE pdi_anis_by_scene_migrados
--SELECT meta.id, meta.ani, tmp1.downgrade, 34
FROM pdi_meta_calls meta
LEFT OUTER JOIN (SELECT meta.ani, meta.trans_datetime, custom.down_date as down_date,
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           custom.down_date, NULL),
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           1,0) AS downgrade
                 FROM (SELECT ani,
                             MAX(unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")) AS trans_datetime
                       FROM pdi_meta_calls
                       WHERE unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") >=
                             unix_timestamp("01/03/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                             unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") <
                             unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                       GROUP BY ani
                            ) meta
                 LEFT OUTER JOIN (SELECT ani,
                                         MIN(unix_timestamp(fecha, "dd/MM/yyyy hh:mm:ss aa")) AS down_date
                                  FROM pdi_customer_status
                                  WHERE gru_cli_a='RES' AND (gru_cli_d='RES' OR gru_cli_d IS NULL) AND
                                        (fecha IS NULL OR (unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") >=
                                        unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                                        unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") <
                                        unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss")))
                                  GROUP BY ani
                                 ) custom ON custom.ani = meta.ani
                 WHERE custom.ani IS NOT NULL
                ) tmp1 ON tmp1.ani = meta.ani AND
                          tmp1.trans_datetime = unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")
--LEFT OUTER JOIN pe_down_mo.esc4_cos2_data_all esc4_cos2 ON esc4_cos2.numerotelefono = meta.ani
LEFT OUTER JOIN pe_down_mo.esc4_cos2_data_mig esc4_cos2 ON esc4_cos2.numerotelefono = meta.ani
WHERE tmp1.ani IS NOT NULL AND esc4_cos2.numerotelefono IS NOT NULL
--WHERE tmp1.ani IS NOT NULL
--AND cd2 <> 'DECISORES' AND cd2 <> 'MEDIO VALOR' AND cd2 <> 'NEG CORTE POR ROBO'
--AND cd2 <> 'NEG MASIVO' AND cd2 <> 'NEG TOP 1' AND cd2 <> 'NEGOCIOS CONVERGENTE'
--AND cd2 <> 'NEGOCIOS USUARIOS' AND cd2 <> 'Negocios Convergente'
AND (cd2 = 'MIGRADOS' OR cd2 = 'RETENCION RES Y MIG')
ORDER BY id;

-- select transcriptions
-- (Escenario 1/2 | cosecha 3) llamadas en marzo y downgrade en mayo

--INSERT OVERWRITE LOCAL DIRECTORY '/tmp/info_escenario12.csv'
INSERT OVERWRITE LOCAL DIRECTORY '/tmp/info_escenario12_migrados.csv'
SELECT meta.id,
       meta.cd2,
       IF(tmp1.downgrade <> esc2_cos3.target, esc2_cos3.target, tmp1.downgrade) as downgrade,
       trans.text
FROM pdi_meta_calls meta
LEFT OUTER JOIN  (SELECT id, CONCAT_WS(' ', COLLECT(word)) as text
                  FROM pdi_transcriptions_calls
                  GROUP BY id) trans ON trans.id = meta.id
LEFT OUTER JOIN (SELECT meta.ani, meta.trans_datetime, custom.down_date as down_date,
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           custom.down_date, NULL),
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           1,0) AS downgrade
                 FROM (SELECT ani,
                             MAX(unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")) AS trans_datetime
                       FROM pdi_meta_calls
                       WHERE unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") >=
                             unix_timestamp("01/03/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                             unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") <
                             unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                       GROUP BY ani
                            ) meta
                 LEFT OUTER JOIN (SELECT ani,
                                         MIN(unix_timestamp(fecha, "dd/MM/yyyy hh:mm:ss aa")) AS down_date
                                  FROM pdi_customer_status
                                  WHERE gru_cli_a='RES' AND (gru_cli_d='RES' OR gru_cli_d IS NULL) AND
                                        (fecha IS NULL OR (unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") >=
                                        unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                                        unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") <
                                        unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss")))
                                  GROUP BY ani
                                 ) custom ON custom.ani = meta.ani
                 WHERE custom.ani IS NOT NULL
                ) tmp1 ON tmp1.ani = meta.ani AND
                          tmp1.trans_datetime = unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")
LEFT OUTER JOIN pe_down_mo.esc2_cos3_data_mig esc2_cos3 ON esc2_cos3.numerotelefono = meta.ani
--LEFT OUTER JOIN pe_down_mo.esc2_cos3_data_all esc2_cos3 ON esc2_cos3.numerotelefono = meta.ani
WHERE tmp1.ani IS NOT NULL AND esc2_cos3.numerotelefono IS NOT NULL
--WHERE tmp1.ani IS NOT NULL
--AND cd2 <> 'DECISORES' AND cd2 <> 'MEDIO VALOR' AND cd2 <> 'NEG CORTE POR ROBO'
--AND cd2 <> 'NEG MASIVO' AND cd2 <> 'NEG TOP 1' AND cd2 <> 'NEGOCIOS CONVERGENTE'
--AND cd2 <> 'NEGOCIOS USUARIOS' AND cd2 <> 'Negocios Convergente'
AND (cd2 = 'MIGRADOS' OR cd2 = 'RETENCION RES Y MIG')
ORDER BY id;

-- (Escenario 4/2 | cosecha 3) llamadas en marzo y downgrade en abril
--INSERT OVERWRITE LOCAL DIRECTORY '/tmp/info_escenario34.csv'
INSERT OVERWRITE LOCAL DIRECTORY '/tmp/info_escenario34_migrados.csv'
SELECT meta.id,
       meta.cd2,
       IF(tmp1.downgrade <> esc4_cos2.target, esc4_cos2.target, tmp1.downgrade) as downgrade,
       trans.text
FROM pdi_meta_calls meta
LEFT OUTER JOIN  (SELECT id, CONCAT_WS(' ', COLLECT(word)) as text
                  FROM pdi_transcriptions_calls
                  GROUP BY id) trans ON trans.id = meta.id
LEFT OUTER JOIN (SELECT meta.ani, meta.trans_datetime, custom.down_date as down_date,
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           custom.down_date, NULL),
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           1,0) AS downgrade
                 FROM (SELECT ani,
                             MAX(unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")) AS trans_datetime
                       FROM pdi_meta_calls
                       WHERE unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") >=
                             unix_timestamp("01/03/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                             unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") <
                             unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                       GROUP BY ani
                            ) meta
                 LEFT OUTER JOIN (SELECT ani,
                                         MIN(unix_timestamp(fecha, "dd/MM/yyyy hh:mm:ss aa")) AS down_date
                                  FROM pdi_customer_status
                                  WHERE gru_cli_a='RES' AND (gru_cli_d='RES' OR gru_cli_d IS NULL) AND
                                        (fecha IS NULL OR (unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") >=
                                        unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                                        unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") <
                                        unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss")))
                                  GROUP BY ani
                                 ) custom ON custom.ani = meta.ani
                 WHERE custom.ani IS NOT NULL
                ) tmp1 ON tmp1.ani = meta.ani AND
                          tmp1.trans_datetime = unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")
--LEFT OUTER JOIN pe_down_mo.esc4_cos2_data_all esc4_cos2 ON esc4_cos2.numerotelefono = meta.ani
LEFT OUTER JOIN pe_down_mo.esc4_cos2_data_mig esc4_cos2 ON esc4_cos2.numerotelefono = meta.ani
WHERE tmp1.ani IS NOT NULL AND esc4_cos2.numerotelefono IS NOT NULL
--WHERE tmp1.ani IS NOT NULL
--AND cd2 <> 'DECISORES' AND cd2 <> 'MEDIO VALOR' AND cd2 <> 'NEG CORTE POR ROBO'
--AND cd2 <> 'NEG MASIVO' AND cd2 <> 'NEG TOP 1' AND cd2 <> 'NEGOCIOS CONVERGENTE'
--AND cd2 <> 'NEGOCIOS USUARIOS' AND cd2 <> 'Negocios Convergente'
AND (cd2 = 'MIGRADOS' OR cd2 = 'RETENCION RES Y MIG')
ORDER BY id;

-— Transcriptions grouped by cd2, downgrade
SELECT count(*) as num, cd2, downgrade 
FROM (
SELECT id, trans_datetime, down_date, AgentID, ANI, CD7, CD2, CD5, CD16, CD17, CD18, CD23, Duration,
UTER JOIN (SELECT meta.ani, meta.trans_datetime, custom.down_date as down_date,
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           custom.down_date, NULL),
                        IF(custom.down_date IS NOT NULL AND
                           custom.down_date >= unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                           custom.down_date < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss"),
                           1,0) AS downgrade
                 FROM (SELECT ani,
                             MAX(unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")) AS trans_datetime
                       FROM pdi_meta_calls
                       WHERE unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") >=
                             unix_timestamp("01/03/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                             unix_timestamp(substr(StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") <
                             unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
                       GROUP BY ani
                            ) meta
                 LEFT OUTER JOIN (SELECT ani,
                                         MIN(unix_timestamp(fecha, "dd/MM/yyyy hh:mm:ss aa")) AS down_date
                                  FROM pdi_customer_status
                                  WHERE gru_cli_a='RES' AND (gru_cli_d='RES' OR gru_cli_d IS NULL) AND
                                        (fecha IS NULL OR (unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") >=
                                        unix_timestamp("01/04/2014 00:00:00","dd/MM/yyyy HH:mm:ss") AND
                                        unix_timestamp(fecha,"dd/MM/yyyy hh:mm:ss aa") <
                                        unix_timestamp("01/06/2014 00:00:00","dd/MM/yyyy HH:mm:ss")))
                                  GROUP BY ani
                                 ) custom ON custom.ani = meta.ani
                 WHERE custom.ani IS NOT NULL
                ) tmp1 ON tmp1.ani = meta.ani AND
                          tmp1.trans_datetime = unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss")
LEFT OUTER JOIN pe_down_mo.esc4_cos2_data_final_all esc4_cos2 ON esc4_cos2.numerotelefono = meta.ani
WHERE tmp1.ani IS NOT NULL AND esc4_cos2.numerotelefono IS NOT NULL
--WHERE tmp1.ani IS NOT NULL
AND cd2 <> 'DECISORES' AND cd2 <> 'MEDIO VALOR' AND cd2 <> 'NEG CORTE POR ROBO'
AND cd2 <> 'NEG MASIVO' AND cd2 <> 'NEG TOP 1' AND cd2 <> 'NEGOCIOS CONVERGENTE'
AND cd2 <> 'NEGOCIOS USUARIOS' AND cd2 <> 'Negocios Convergente'
--AND (cd2 = 'MIGRADOS' OR cd2 = 'RETENCION RES Y MIG')
ORDER BY id;


       NumberOfHolds, NumberOfConferences, NumberOfTransfers, TotalHoldTime, WrapUpTime,
       TOTAL_AGENT_TALK_TIME, NUM_AGENT_TALK_SEGMENTS, TOTAL_CUSTOMER_TALK_TIME,
       NUM_CUSTOMER_TALK_SEGMENTS, TOTAL_SILENCE_TIME, NUM_SILENCE_SEGMENTS,
       TOTAL_TALK_OVER_TIME, NUM_TALK_OVER_SEGMENTS, NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS,
       NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS, downgrade
FROM (SELECT meta.id,
      unix_timestamp(substr(meta.StartTime,0,19),"yyyy-MM-dd'T'HH:mm:ss") AS trans_datetime,
      unix_timestamp(down.fec_detalle,"dd/MM/yyyy hh:mm:ss aa") AS down_date,
      meta.AgentID, meta.ANI, meta.CD7,meta.CD2,
      meta.CD5,meta.CD16,meta.CD17,meta.CD18,meta.CD23,meta.Duration,
      meta.NumberOfHolds,meta.NumberOfConferences,meta.NumberOfTransfers,meta.TotalHoldTime,meta.WrapUpTime,
      meta.TOTAL_AGENT_TALK_TIME,meta.NUM_AGENT_TALK_SEGMENTS,meta.TOTAL_CUSTOMER_TALK_TIME,
      meta.NUM_CUSTOMER_TALK_SEGMENTS,meta.TOTAL_SILENCE_TIME,meta.NUM_SILENCE_SEGMENTS,
      meta.TOTAL_TALK_OVER_TIME,meta.NUM_TALK_OVER_SEGMENTS,meta.NUM_AGENT_INITIATED_TALK_OVER_SEGMENTS,
      meta.NUM_CUSTOMER_INITIATED_TALK_OVER_SEGMENTS,
      not_down.telefono as not_down_telefono,
      IF(down.pla_nu_telefo is not null,1,0) AS downgrade
      FROM pdi_meta_calls meta
      LEFT OUTER JOIN (SELECT pla_nu_telefo, fec_detalle
                       FROM pdi_down
                       WHERE gru_cli_a='RES' AND gru_cli_d='RES') down ON down.pla_nu_telefo = meta.ani
      LEFT OUTER JOIN ( SELECT telefono
                        FROM pdi_not_down
                        WHERE segmento = 'Residencial') not_down ON not_down.telefono = meta.ani
     )tmp
WHERE ( (down_date IS NULL AND trans_datetime < unix_timestamp("01/05/2014 00:00:00","dd/MM/yyyy HH:mm:ss")
AND not_down_telefono IS NOT NULL)
OR (down_date IS NOT NULL AND trans_datetime < down_date))
AND cd2 <> 'DECISORES' AND cd2 <> 'MEDIO VALOR' AND cd2 <> 'NEG CORTE POR ROBO'
AND cd2 <> 'NEG MASIVO' AND cd2 <> 'NEG TOP 1' AND cd2 <> 'NEGOCIOS CONVERGENTE'
AND cd2 <> 'NEGOCIOS USUARIOS' AND cd2 <> 'Negocios Convergente')tmp
GROUP BY cd2, downgrade
