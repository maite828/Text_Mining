if [ $# -ne 5 ]; then
	echo " ERROR: No arguments supplied"
	echo " Usage:  $0 data_path srate_dir_sep rttm_sep f0_dir_sep spk_dir_sep "
	echo "         $0 /path/to/data ,   ;   "
	exit 20
fi

DATA_PATH=$1
SRATE_DIR_SEP=$2
RTTM_DIR_SET=$3
F0_DIR_SEP=$4
SPK_DIR_SEP=$5

hive -hiveconf DATA_PATH=$DATA_PATH -f ../inserts.hql
