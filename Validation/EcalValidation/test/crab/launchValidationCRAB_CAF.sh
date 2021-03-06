#!/bin/bash

usage='Usage: -d <dataset> -n <name>'
args=`getopt rdcsfe: -- "$@"`
if test $? != 0
     then
         echo $usage
         exit 1
fi

eval set -- "$args"
for i 
  do
  case "$i" in
      -d) shift; data_set=$2;shift;;      
      -n) shift; name=$2;shift;;      
  esac      
done

this_dir=`pwd`;
# set up directory
mkdir -p ${name};

# set up ecalCosmicsHists.cfg
cp ecalvalidationTEMPLATE_cfg.py ${name}/ecalvalidation_${name}_cfg.py

# set up crab.cfg file
cat > ${name}/crab.cfg <<EOF

[CRAB]

jobtype = cmssw
scheduler = caf


[CMSSW]

### The data you want to access (to be found on DBS)
datasetpath=${data_set}

### The ParameterSet you want to use
pset = ecalvalidation_${name}_cfg.py

### Run selection: NB use with the option split_by_run
#runselection=123596,124022,124120
#split_by_run=1

### Splitting parameters
#total_number_of_events=-1
total_number_of_events=-1
events_per_job = 100000
#number_of_jobs = 5

### The output files (comma separated list)
output_file = EcalValidation.root

[USER]

### OUTPUT files Management
##  output back into UI
return_data = 1

### To use a specific name of UI directory where CRAB will create job to submit (with full path).
### the default directory will be "crab_0_data_time"
#ui_working_dir = 

### To specify the UI directory where to store the CMS executable output
### FULL path is mandatory. Default is <ui_working_dir>/res will be used.
#logdir= /full/path/yourLogDir

### OUTPUT files INTO A SE
copy_data = 0

### if you want to copy data in a "official CMS site"
### you have to specify the name as written in
#storage_element = T2_IT_Bari
### the user_remote_dir will be created under the SE mountpoint
### in the case of publication this directory is not considered
#user_remote_dir = name_directory_you_want

### if you want to copy your data at CAF
#storage_element = T2_CH_CAF
### the user_remote_dir will be created under the SE mountpoint
### in the case of publication this directory is not considered
#user_remote_dir = name_directory_you_want

### if you want to copy your data to your area in castor at cern
### or in a "not official CMS site" you have to specify the complete name of SE
#storage_element=srm-cms.cern.ch
### this directory is the mountpoin of SE
#storage_path=/srm/managerv2?SFN=/castor/cern.ch/
### directory or tree of directory under the mounpoint
#user_remote_dir = name_directory_you_want

### To publish produced output in a local istance of DBS set publish_data = 1
publish_data=0
### Specify the dataset name. The full path will be <primarydataset>/<publish_data_name>/USER
publish_data_name = name_you_prefer
### Specify the URL of DBS istance where CRAB has to publish the output files
#dbs_url_for_publication = https://cmsdbsprod.cern.ch:8443/cms_dbs_caf_analysis_01_writer/servlet/DBSServlet

### To specify additional files to be put in InputSandBox
### write the full path  if the files are not in the current directory
### (wildcard * are allowed): comma separated list
#additional_input_files = file1, file2, /full/path/file3

#if server
#thresholdLevel = 100
#eMail = your@Email.address

[GRID]
rb                      = CERN
proxy_server            = myproxy.cern.ch
virtual_organization    = cms
retry_count             = 0
lcg_catalog_type        = lfc
lfc_host                = lfc-cms-test.cern.ch
lfc_home                = /grid/cm

[CAF]
queue=cmscaf1nd


EOF

cd ${name};
#pwd

echo " setting up crab env"
# setup crab environment
source /afs/cern.ch/cms/LCG/LCG-2/UI/cms_ui_env.sh;
eval `scramv1 runtime -sh`;
source /afs/cern.ch/cms/ccs/wm/scripts/Crab/crab.sh;

echo " launching crab jobs"
crab -create;
crab -submit;
crab -status;
cd -;

exit
