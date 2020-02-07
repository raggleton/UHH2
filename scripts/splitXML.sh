#!/bin/bash

"""Script to run splitNtuple.C over a XML file

Example usage:

    ./splitXML.sh /a/b/c/my.xml splitResultsDir
"""

XMLFILE=$1
ODIR=$2
mkdir -p "$ODIR"

for f in $(grep -oP 'FileName="(.+.root)"' $XMLFILE | sed 's/FileName="//g' | sed 's/"//g');
do
    newf=$(basename $f)
    newf=${newf/.root/_split%d.root}
    newf="$ODIR/$newf"
    echo $newf
    root -q -b -l 'splitNtuple.C+("'${f}'","'${newf}'",4)'
done
