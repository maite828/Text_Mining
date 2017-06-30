###############################################################################
##                   Wordcloud generator from tweets                         ##
###############################################################################
#
#Copyright Telefonica Digital
#Author: User Modelling Analytics Team
#Mantainer: Susana Ferreras <susanaf@tid.es> 
#Version: 1.1
#Date: 28/05/2013
#Purpose of program: generate wordclouds about topics from Twitter data
#Dependencies: XML, tm, wordcloud, RColorBrewer, stringr, R.utils, rjson, 
#   ggplot2, reshape packages
#Inputs: Text files with tweets in 3rd field (one per row)
#Output: PDF file with individual and common wordclouds
#Config: in wordcloud.json file
#Run: wordcloud.sh or wordcloud.bat to be executed in batch mode
#

#Configuration 
args = (commandArgs(TRUE))
if (length(args) == 0) {
 print("-----------------------------")
 print("  Error: missing arguments   ") 
 print("-----------------------------")
  q()
} else {
  for (i in 1:length(args)) {
    eval(parse(text=args[[i]]))
  }
}

if (!file.exists(configfile) == 'TRUE') {
  print("-------------------------------------")
  print("  Error: missing configuration file  ") 
  print("-------------------------------------")
  q()
}

#Import libraries
library(XML)
library(tm)
library(wordcloud)
library(RColorBrewer)
library(stringr)
library(R.utils)
library(rjson)
library(ggplot2)
library(reshape)
options(warn=-1)

#Add wordcloud functions
source("../Wordcloud/src/functions_wordcloud.R")

#Import parameters in JSON (choose either local file or parameter)
#From local file
#config <- fromJSON(paste(readLines("wordcloud.json"), collapse=""))
#From parameter
config <- fromJSON(file=configfile)
tit <- config$input$title 
subt <- config$input$subtitle 
num.par <- config$input$number
path <- config$input$path 
files <- config$input$files
names <- config$input$names
pal.ind <- config$layout$palette_ind
pal.comp <- config$layout$palette_comp
pal.all <- config$layout$palette_all
bg.colour <- config$layout$bg_colour
tx.colour <- config$layout$tx_colour
ti.colour <- config$layout$ti_colour
rp.main <- config$layout$report_title
tx.main <- config$layout$text_title
tx.date <- config$layout$text_date
out.name <- config$output$name

#Read input data
cumm <- data.frame()
tweets <- data.frame()

for (i in 1:num.par) {
  wc <- read_input(paste(path, files[i], sep=""), names[i])
  tweets[i, 1] <- names[i]
  tweets[i, 2] <- nrow(wc)
  tweets[i, 3] <- num.par - i
  call <- get_words(wc, names[i], filt, ftype, del, lang, stopw)
  cumm <- rbind(cumm, call)
}
colnames(tweets) <- c("words", "count", "num")


#Create report
pdf(file=out.name, width=9, height=6, family="Helvetica-Narrow")
par(bg=bg.colour, mar=c(1,1.2,1.2,1.2), col.main=tx.colour, cex.main=1.8)

plot(0:10, type='n', bty='n', xaxt='n', yaxt='n', xlab='', ylab='')
text(6, 7, tit, font=2, col=ti.colour, cex=3)
text(6, 6, subt, font=2, col=ti.colour, cex=3)
text(6, 4, tx.date, font=2, col=ti.colour, cex=2)


#Summary
ggplot(tweets, aes(x=reorder(words, num), y=count, label=count)) +
  geom_bar(stat="identity", fill=brewer.pal(num.par, pal.comp)[num.par:1], 
           colour=tx.colour) + 
  theme_clean() +
  coord_flip() +
  ggtitle(rp.main) +
  theme(plot.title=
          element_text(hjust=0.5, face="bold", colour=tx.colour, size=25),
        plot.background=element_rect(fill=bg.colour),
        axis.text.x=element_blank(),
        axis.text.y=
          element_text(face="bold", size=15, 
                       colour=brewer.pal(num.par, pal.comp)[num.par:1]),
        panel.grid=element_blank(),
        axis.ticks=element_blank()) +
  geom_text(size=3.5, hjust=-0.2, colour=tx.colour, fontface=2) 


#Individual wordclouds
for(n in 1:num.par) {  
  wo <- cumm[cumm[, "topic"] == names[n], ]
  wordcloud(wo$word,wo$freq, scale=c(5,1), min.freq=1, max.words=100, 
          random.order=FALSE, random.color=FALSE, rot.per=0, 
          colors=brewer.pal(7, pal.ind), use.r.layout=FALSE)
  title(main=paste(tx.main, names[n], tx.date))
  par(fig=c(0,1,0,1), new=TRUE)
  add_legend(brewer.pal(7, pal.ind))
  
  if (n == 1) {
    leg <- "* Bigger sizes and higher \ncolours in palette 
     show more importance"
    par(fig=c(0,1,0,1), new=TRUE)
    plot(0:25, type="n")
    text(22, 1, leg, col=tx.colour, cex=0.8)
  }
}


#Multi wordclouds
if (num.par > 1){
  
  mat <- get_matrix(cumm)
  comparison.cloud(mat,scale=c(5,1),max.words=2000,random.order=FALSE, rot.per=0,
                   use.r.layout=FALSE, title.size=2, 
                   colors=brewer.pal(ncol(mat), pal.comp))
  title(main=paste('Multi wordcloud', tx.date))
  leg <- "* If a word is related \nto some topics,\nit will be associated 
   to the one with \nmore importance (in %)"
  par(fig=c(0,1,0,1), new=TRUE)
  plot(0:25, type="n")
  text(24, 1.2, leg, col=tx.colour, cex=0.8)
  
  all <- list()
  for (i in 1:nrow(mat)) {
    all[i] <- sum(mat[i, 1:num.par] > 0)
  }
  
  if (any(all == num.par)) {
    commonality.cloud(mat, comonality.measure=min, scale=c(5,1), 
                      random.order=FALSE, random.color=FALSE, rot.per=0, 
                      colors=brewer.pal(7, pal.all), use.r.layout=FALSE)
    title(main=paste('Wordcloud - Words in all topics', tx.date))
    par(fig=c(0,1,0,1), new=TRUE)
    add_legend(brewer.pal(7, pal.all))
  }
  
  commonality.cloud(mat, comonality.measure=max, scale=c(5,1), 
                    random.order=FALSE, random.color=FALSE, rot.per=0, 
                    colors=brewer.pal(7, pal.all), use.r.layout=FALSE)
  title(main=paste('Wordcloud - Most important words', tx.date))
  par(fig=c(0,1,0,1), new=TRUE)
  add_legend(brewer.pal(7, pal.all)) 

}

dev.off()
rm(list=ls())