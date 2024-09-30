#! /usr/bin/bash

set -e

PROG=$1
SNX=$2
SCR=.libsinex_test_scratch_file_tmp
REF=.libsinex_test_scratch_file_tmp_ref

cat << EndOfReferenceResult > ${REF}
AREA 1 ALCATEL -----
AREA 2 ALCATEL -----
AREA 3 ALCATEL -----
AREA 4 ALCATEL -----
ARFB 1 STAREC-B -----
ARFB 2 STAREC-B -----
ARFB 3 STAREC-B -----
ARFB 4 STAREC-B -----
ARFB 5 STAREC-B -----
ARFB 6 STAREC-B -----
DIOA 1 ALCATEL -----
DIOB 1 STAREC-B -----
KESB 1 STAREC-B -----
KESB 2 STAREC-B -----
SANB 1 STAREC-B -----
SANB 2 STAREC-B -----
SANB 3 STAREC-B -----
SANB 4 STAREC-B -----
SANB 5 STAREC-B -----
SANB 6 STAREC-B -----
SANB 7 STAREC-B -----
ARFB 4 STAREC-B -----
DIOB 1 STAREC-B -----
DIOA 1 ALCATEL -----
KESB 1 STAREC-B -----
SANB 1 STAREC-B -----
SANB 2 STAREC-B -----
EndOfReferenceResult

# Run the test program and store output
${PROG} ${SNX} KOKO AREA ARFB DIOA DIOB KESB BAR SANB FOO> ${SCR}

# Diff the files
diff ${REF} ${SCR}

# exit
exit $?
