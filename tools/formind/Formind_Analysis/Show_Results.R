# R-Script to visualize FORMIND output
# Contact: info@formind.org


#------------------------------------------------------------------------------
# General options - Please adjust!
#------------------------------------------------------------------------------

# result folder of your project
folder<-"C:/Formind_Model/Project_Brazil_Amazon/results/"

# Provide the project name (name of the result files)
project = "generalAmazonianForest_3pft"   # name of the result files (= name of your project)
N_ha = 1               # number of simulated hectares (= parameter switch.ha)
N_pft = 3              # number of plant functional types (PFT) (= parameter N_Par.Div_MAXGRP)


#------------------------------------------------------------------------------
# Color options
#------------------------------------------------------------------------------
color = c(rainbow(N_pft, alpha=1), "#000000FF")   # Colors for each PFT + total (black)
colortransparent = c(rainbow(N_pft, alpha=0.1))   # Colors for each PFT 

#------------------------------------------------------------------------------
# Special options (do not change)
#------------------------------------------------------------------------------

margins = c(4.5, 4.5, 1, 1)  # specify graphics margins (bottom, left, top, right)
ylim.factor = 1.3          # scaling factor for y-axis (multiple of the maximal y-value)
size.axis.labels = 1.2       # text size for axis titles (cex)
size.axis.numbers = 1.2      # text size for axis numbers (cex)
size.legend.text = 1.2    # text size for legend text (cex)
size.lwd = 3            # line width

#------------------------------------------------------------------------------
# Read all simulation files
#------------------------------------------------------------------------------
  
options(scipen=15)
file_ba = paste(project,".ba",sep="")                                     
file_agb = paste(project,".bt",sep="")
file_sn = paste(project,".n",sep="")
file_cflux = paste(project,".cflux",sep="")

results_ba = try(read.table(paste(folder,file_ba, sep=""), sep=c("","\t"), header=T, skip=2, na.strings= c("?", "", "!"),fill=TRUE, blank.lines.skip=TRUE),silent=T)
if (class(results_ba) == "try-error") results_ba = NULL  else 
results_agb = try(read.table(paste(folder,file_agb, sep=""), sep=c("","\t"), header=T, skip=2, na.strings= c("?", "", "!"),fill=TRUE, blank.lines.skip=TRUE),silent=T)
if (class(results_agb) == "try-error") results_agb = NULL  else
results_sn = try(read.table(paste(folder,file_sn, sep=""), sep=c("","\t"), header=T, skip=2, na.strings= c("?", "", "!"),fill=TRUE, blank.lines.skip=TRUE),silent=T)
if (class(results_sn) == "try-error") results_sn = NULL  else
results_cflux = try(read.table(paste(folder,file_cflux, sep=""), sep=c("","\t"), header=T, skip=2, na.strings= c("?", "", "!"),fill=TRUE, blank.lines.skip=TRUE),silent=T)
if (class(results_cflux) == "try-error") results_cflux = NULL 

#------------------------------------------------------------------------------
# Simulation time
endtime = max(results_agb$Time)
print(paste("Simulation time: ", endtime))


par(mfrow=c(2,2))
legendpft = paste("PFT", 1:N_pft)
#------------------------------------------------------------------------------
# 1 Plot: Biomass over time for PFTs
#------------------------------------------------------------------------------

  par(mar=margins)
  maxagb=max(results_agb$TotalBiomass)
  plot(results_agb$TotalBiomass, type="l", xlab="Time [yr]", ylab=expression(paste("Biomass [ ",t[ODM],ha^-1,"]")),lwd=size.lwd+1, xlim=c(0, endtime), ylim=c(0, ylim.factor*maxagb), cex.lab=size.axis.labels, cex.axis=size.axis.numbers, las=1)
  for(i in 1:N_pft) { 
    time = results_agb$Time
    val = eval(parse(text=paste("results_agb$BiomassPerPFT_", i, sep="")))
    xx = c(time[1:endtime], rev(time[1:endtime]))
    yy = c(val[1:endtime], (1:endtime)*0)
    polygon(xx, yy, col=colortransparent[i],border = NA)
    lines(x=time, y=val, col=color[i], lwd=size.lwd)
  }  
  legend("topleft", legend=c(legendpft,"total"), lwd=3, col=c(color,"black"), lty=1, bty="n", ncol=3, cex=size.legend.text)


#------------------------------------------------------------------------------
# 2 Plot: Basal area over time for PFTs
#------------------------------------------------------------------------------

  par(mar=margins)
  plot(results_ba$TotalBasalArea, type="l", xlab="Time [yr]", ylab=expression(paste("Basal area [ ",m^2, ha^-1,"]")), xlim=c(0, endtime), ylim=c(0, max(results_ba$TotalBasalArea)*ylim.factor), lwd=size.lwd+1, col="black", las=1, cex.lab=size.axis.labels, cex.axis=size.axis.numbers)
  for (i in 1:N_pft) {
    time = results_ba$Time
    val = eval(parse(text=paste("results_ba$BasalAreaPerPFT_",i,sep="")))
    xx = c(time[1:endtime], rev(time[1:endtime]))
    yy = c(val[1:endtime], (1:endtime)*0)
    polygon(xx, yy, col=colortransparent[i],border = NA)
    lines(x=time, y=val, col=color[i], lwd=size.lwd)
  }
  legend("topleft",legend=c(legendpft,"total"),col=c(color,"black"), lty=1, lwd=3, bty="n", ncol=3, cex=size.legend.text)


#------------------------------------------------------------------------------
# 3 Plot: Stem number over time for PFTs
#------------------------------------------------------------------------------

  par(mar=margins)
  plot(results_sn$TotalNumber, type="l", xlab="Time [yr]", ylab=expression(paste("Stem number [ ",ha^-1,"]")), xlim=c(0,endtime), ylim=c(0,max(results_sn$TotalNumber)*ylim.factor), lwd=size.lwd+1, col="black", las=1, cex.lab=size.axis.labels, cex.axis=size.axis.numbers)
  for (i in 1:N_pft) {
    time = results_sn$Time
    val = eval(parse(text=paste("results_sn$NumberPerPFT_",i,sep="")))
    xx = c(time[1:endtime], rev(time[1:endtime]))
    yy = c(val[1:endtime], (1:endtime)*0)
    polygon(xx, yy, col=colortransparent[i],border = NA)
    lines(x=time,y=val, col=color[i], lwd=size.lwd)
  }
  legend("topleft", legend=c(legendpft, "total"), col=c(color,"black"), lty=1, lwd=3, bty="n", ncol=3, cex=size.legend.text)


#------------------------------------------------------------------------------
# 4 Plot: Carbon flux over time
#------------------------------------------------------------------------------

  par(mar=margins)
  maxNEE = max(results_cflux$NEE)
  minNEE = max(min(results_cflux$NEE))
  plot(NULL, xlim=c(0,endtime), ylim=c(minNEE,maxNEE)*ylim.factor, lwd= size.lwd,cex.axis=size.axis.numbers, cex.lab=size.axis.labels, xlab="Time [yr]", ylab=expression(paste("NEE [ ",t[C],ha^-1,yr^-1,"]")), las=1)  
  xx = c(results_cflux$Time[1:endtime], rev(results_cflux$Time[1:endtime]))
  yy = c(results_cflux$NEE[1:endtime], (1:endtime)*0)
  yy[yy<=0]=0
  polygon(xx, yy, col="cornflowerblue",border = NA)  
  yy = c(results_cflux$NEE[1:endtime], (1:(endtime))*0)
  yy[yy>0]=0
  polygon(xx, yy, col="indianred2",border = NA)
  lines(x=results_cflux$Time[1:endtime],y=results_cflux$NEE[1:endtime],col="black",lwd=size.lwd)  
  abline(h=0)
  legend("topright", ncol=2, cex=size.legend.text, pt.cex=size.legend.text+2, bty="n", legend = c("C sink", "C source"), col=c("cornflowerblue","indianred2"), pch=15)
  par(xpd=F)
  par(mfrow=c(1,1))

