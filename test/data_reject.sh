#! /usr/bin/bash

set -e

PROG=$1
SNX=$2
SCR=.libsinex_test_scratch_file_tmp
REF=.libsinex_test_scratch_file_tmp_ref

cat << EndOfReferenceResult > ${REF}
DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
MALB 2012/04/15 00:00:00.000000000 2012/05/14 23:59:59.000000000 Electric pb 20120415-20120514
DIOA 2005/12/15 00:00:00.000000000 2006/05/16 23:59:59.000000000 Transmission stopped
MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
MALB 2009/02/01 00:00:00.000000000 2009/04/05 23:59:59.000000000 Corrupted data 20090201-20090405
EndOfReferenceResult

# Run the test program and store output
${PROG} ${SNX} DIOA DIOB DIOC MALB > ${SCR}

# Diff the files
diff ${REF} ${SCR}

# exit
exit $?