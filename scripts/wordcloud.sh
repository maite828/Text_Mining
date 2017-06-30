#Linux
R CMD BATCH --no-save --no-restore --max-mem-size=2G \
       	"--args configfile='../wordcloud/conf/wordcloud.json'" \
       	../wordcloud/src/main_wordcloud.R \
        ../log/wordcloud.log


