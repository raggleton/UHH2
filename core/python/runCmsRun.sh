#!/bin/bash

#$ -V
#$ -cwd
#$ -l os=sld6
#$ -l site=hh
#$ -P unihh2
#$ -m eas
#$ -M robin.aggleton@desy.de
#$ -t 1-75
#$ -l h_vmem=3G
#$ -l h_fsize=2G
#$ -l h_rt=18:00:00

# Run on BIRD: qsub runCmsRun.sh
# Don't forget to adjust the number of jobs after -t, and the walltime (h_rt)
# Don't lower vmem - will segfault otherwise

# Hack to get grid cert as it can't see the original /tmp/x509up_u28860
# Remember to copy your grid cert first to $HOME/cert!
ls -lh /afs/desy.de/user/a/aggleton/cert
ls -lh /nfs/dust/cms/user/aggleton/cert
if [ -f /tmp/x509up_u28860 ]; then rm /tmp/x509up_u28860; fi
cp /afs/desy.de/user/a/aggleton/cert /tmp/x509up_u28860
voms-proxy-info

echo "Run job ${SGE_TASK_ID}"
cmsRun wrapper.py


