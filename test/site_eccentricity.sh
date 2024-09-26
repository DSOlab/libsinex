#! /usr/bin/bash

set -e

PROG=$1
SNX=$2
SCR=.libsinex_test_scratch_file_tmp
REF=.libsinex_test_scratch_file_tmp_ref

cat << EndOfReferenceResult > ${REF}
DIOA 0.5100 0.0000 0.0000
MANB 0.4870 0.0000 0.0000
DIOB 0.4870 0.0000 0.0000
MANB 0.4870 0.0000 0.0000
DIOA 0.5100 0.0000 0.0000
MANB 0.4870 0.0000 0.0000
DIOA 0.5100 0.0000 0.0000
DIOB 0.4870 0.0000 0.0000
MANB 0.4870 0.0000 0.0000
EndOfReferenceResult

# Run the test program and store output
${PROG} ${SNX} DIOA DIOB DIOC MANB > ${SCR}

# Diff the files
diff ${REF} ${SCR}

# exit
exit $?
