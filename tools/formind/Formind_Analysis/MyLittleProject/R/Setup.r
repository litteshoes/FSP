##*******************************************
## 1. Setup: where is what
##*******************************************
#path to the folder MyLittleProject e.g:
yourPath<-"C:/Daten/FORMIND-TRUNK/formind-analysis/FAST/MyLittleProject/"

setwd(yourPath) 
# now R nows where your little Project is located on you computer..
FORMINDFolder = paste0(yourPath,"FORMIND/") 
#...where the Model can be found...
graphicFolder = paste0(yourPath,"Graphics/") 
#...and where the grafiks of your simulations should be placed.

# install the package FAST and its dependencies
install.packages("Hmisc","data.table","RColorBrewer","openair") 
# these packages are needed to run FAST
install.packages("FAST.zip",repos = NULL, dependencies = T)

