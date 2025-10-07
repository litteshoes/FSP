
library(FAST) #loads FAST package

path<-("C:/Daten/FORMIND-TRUNK/formind-analysis/FAST/MyLittleProject")
setwd(path)
#********************************************
# 2. the fast-function
#********************************************
model<-"formind.exe"

parameterFile<-"bci4.par"

projectName<-"experiment"

# Modify Parameters
setwd(paste0(path,"/FORMIND/formind_parameters"))
PARfile<-paste0(projectName,".par")
modifyPAR(parameterFile,PARfile
          , list(myResultFileSwitch.dia = 1
                 ,Mort_mean_19 = c(0.01,0.015,0.02,0.025)
          )
)

# run FORMIND
setwd(paste0(path,"/FORMIND"))
runFORMIND(model,PARfile)

# read Data
dia<-read.formindData(projectName,fileTypes="dia")

# histogramms of the last 10% of the years of the simulation
histStemSizeDist(dia, log="y")
histStemSizeDist(dia,y="BasalAreaPFT" ,graphicType="paperC2")

# the already known plots which show a variable over time.

forest<-read.formindData(projectName)
plotSuccession(forest,y=c("TotalBiomass"
                          ,"BiomassPerPFT")
               ,graphicType="paperC1" )


