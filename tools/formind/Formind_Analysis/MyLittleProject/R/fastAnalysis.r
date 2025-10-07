
##*******************************************
## 1. Setup
##*******************************************

library(FAST) #loads FAST package

#********************************************
# 2. the fast-function
#********************************************
# please replace with your path to the Formind.exe
model<-"C:/Data/FORMIND-Project/Formind.exe" 
# please replace with your path to the test.par
parameterFile<-"C:/Data/FORMIND-Project/test.par" 

data<-fast(model,parameterFile)

