###############################################################################
##                          Wordcloud functions                              ##
###############################################################################
#
#Copyright Telefonica Digital
#Author: User Modelling Analytics Team, Pedro Concejero <pedroc@tid.es>
#Mantainer: Susana Ferreras <susanaf@tid.es> 
#Version: 1.1
#Date: 28/05/2013
#Purpose of program: Create useful functions for wordcloud process
#Dependencies: - 
#Inputs: Depending on the function
#Output: Depending on the function
#Config: -
#Run: -
#

read_input <- function(ifile, topic) {
  df <- read.table(ifile, header=F, sep="\n", fill=FALSE, strip.white=TRUE, 
                   comment.char="", flush=TRUE, quote = "", col.names=c("tweet"))    
  wc.df <- as.data.frame(str_split_fixed(df$tweet, "\\|", 3))
  wc.df <- subset(wc.df, !duplicated(wc.df))
  colnames(wc.df) <- c("id","user_id","text")
  return(wc.df)
}

get_words <- function(wc.df, topic, filt, ftype, del, lang, stopw) {
  wc.corpus <- Corpus(DataframeSource(data.frame(as.character(wc.df$text))))  
  wc.m <- as.matrix(TermDocumentMatrix(wc.corpus))
  wc.v <- sort(rowSums(wc.m), decreasing=TRUE)
  wc.d <- data.frame(word=names(wc.v), freq=wc.v)
  wc.d$topic <- topic
  row.names(wc.d) <- NULL
  return(wc.d)
}

theme_clean <- function(base_size = 12) {
  require(grid)
  theme_grey(base_size)
  theme(
    axis.title=element_blank(),
    panel.background=element_rect(fill=bg.colour),
    axis.ticks.length=unit(0.1, "cm"),
    axis.ticks.margin=unit(0.1, "cm"),
    panel.margin=unit(0, "lines"),
    plot.margin = unit(c(2, 2, 2, 2), "lines"),
    plot.background=element_blank(),
    complete=TRUE
  )
}

add_legend <- function(pall) {
  min = -1
  max = 1
  scale = length(pall)/(max - min)
  plot(c(0, 20), c(min, max), type='n', bty='n', xaxt='n', yaxt='n', 
       xlab='', ylab='')
  for (p in 1:(length(pall)+1)) {
    y = (p-1)/scale + min
    rect(19,y,20,y+1/scale, col=pall[p], border=NA)
  }
}

assign_col <- function(words1, words2, num) {
  extra.pal <- rep(brewer.pal(10, "Greys")[4], num)
  words <- NULL
  col <- NULL
  for (w in 1:num) {
    words[w] <- toString(words1$word[w])
    words[w+num] <- toString(words2$word[w])
  }
  uni <- unique(words)
  col <- append(rainbow(num), extra.pal[1:(length(uni)-num)])  
  words.uni <- cbind(uni, col)
  col.all <- NULL
  for (x in 1:(num*2)) {
    col.all[x] <- words.uni[, 2][words.uni[, 1] == words[x]]
  }
  words.all <- cbind(words, col.all)
  return(words.all[, 2])
}

multiplot <- function(..., plotlist=NULL, file, cols=1, layout=NULL) {
  require(grid)
  plots <- c(list(...), plotlist)
  numPlots = length(plots)
  if (is.null(layout)) {
    layout <- matrix(seq(1, cols * ceiling(numPlots/cols)),
                     ncol=cols, nrow=ceiling(numPlots/cols))
  }
  if (numPlots == 1) {
    print(plots[[1]]) 
  } else {
    grid.newpage()
    pushViewport(viewport(layout=grid.layout(nrow(layout), ncol(layout))))
    for (i in 1:numPlots) {
      matchidx <- as.data.frame(which(layout == i, arr.ind=TRUE))
      
      print(plots[[i]], vp=viewport(layout.pos.row=matchidx$row,
                                      layout.pos.col=matchidx$col))
    }
  }
}

comparison <- function(obj1, obj2, num) {
  words.t1 <- obj1[1:num, ]
  words.t2 <- obj2[1:num, ]
  words.t1$num <- seq(num,1)
  words.t2$num <- seq(num,1)  
  colours <- assign_col(words.t1, words.t2, num)
  
  p1 <- ggplot(words.t1, aes(x=freq, y=reorder(word, num))) +
    geom_point(size=3, colour="dark grey") +
    theme_clean() +
    ggtitle(paste("Top", num, "words:", tx.date.t1)) +
    theme(plot.title=
            element_text(hjust=1, vjust=1, face="bold", colour=tx.colour, 
                         size=20),
          axis.title=element_blank(),
          panel.background=element_rect(fill=bg.colour),
          panel.grid.major.x=element_blank(),
          panel.grid.minor.x=element_blank(),
          axis.text.y=
            element_text(face="bold", colour=colours[num:1], size=15),
          panel.grid.major.y=element_line(colour="grey", linetype="dashed"),
          plot.background=element_rect(fill=bg.colour)) +
    coord_cartesian(xlim = c(0, max(obj1$freq[1], obj2$freq[1])*1.05))
  
  p2 <- ggplot(words.t2, aes(x=freq, y=reorder(word, num))) +
    geom_point(size=3, colour="dark grey") +
    theme_clean() +
    ggtitle(paste("Top", num, "words:", tx.date.t2)) +
    theme(plot.title=
            element_text(hjust=1, vjust=1, face="bold", colour=tx.colour, 
                         size=20),
          axis.title=element_blank(),
          panel.background=element_rect(fill=bg.colour),
          panel.grid.major.x=element_blank(),
          panel.grid.minor.x=element_blank(),
          axis.text.y=
            element_text(face="bold", colour=colours[(num*2):(num+1)], size=15),
          panel.grid.major.y=element_line(colour="grey", linetype="dashed"),
          plot.background=element_rect(fill=bg.colour)) +
    coord_cartesian(xlim=c(0, max(obj1$freq[1], obj2$freq[1])*1.05))  
  
  multiplot(p1, p2, cols=2)
}

get_matrix <- function(cumm) {
  matrix <- cast(cumm, word ~ topic, value="freq", fill=0)
  rownames(matrix) <- matrix[, 1]
  matrix <- matrix[, -1]
  return(matrix)
}

