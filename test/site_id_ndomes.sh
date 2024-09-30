#! /usr/bin/bash

set -e

PROG=$1
SNX=$2
SCR=.libsinex_test_scratch_file_tmp
REF=.libsinex_test_scratch_file_tmp_ref

cat << EndOfReferenceResult > ${REF}
CRQC  A 91301S004
CRRC  A 91301S005
DAKA  A 34101S004
DIOA  A 12602S011
DIOB  A 12602S012
DJIA  A 39901S002
DJIB  A 39901S003
EASA  A 41703S008
EASB  A 41703S009
EVEB  A 21501S001
EndOfReferenceResult

# Run the test program and store output
${PROG} ${SNX} KOKO FOO CRQC CRRC DAKA EASA XAN EASB EVEB DIOB DIOA DJIA \
  DIOA DJIC DJIB DIOA > ${SCR}

# Diff the files
diff ${REF} ${SCR}

# exit
exit $?
