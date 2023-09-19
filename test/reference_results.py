##
# Reference results for testing purposes
# Load them using import
##

ds1 = """DIOA  A 12602S011
DIOB  A 12602S012
"""

ds2 = """DIOA  A 12602S011
DIOB  A 12602S012
"""

ds3 = """DIOB 0.487000 0.000000 0.000000
DIOA 0.510000 0.000000 0.000000
DIOB 0.487000 0.000000 0.000000
"""

ds4 = """DIOA 2005-12-15 00:00:00.000000000 2006-05-16 23:59:59.000000000 Transmission stopped
DIOA 2005-12-15 00:00:00.000000000 2006-05-16 23:59:59.000000000 Transmission stopped
DIOA 2005-12-15 00:00:00.000000000 2006-05-16 23:59:59.000000000 Transmission stopped
"""

ds5 = """STAX DIOA 4595215.24319 +/- 0.00087
STAY DIOA 2039475.41598 +/- 0.00112
STAZ DIOA 3912615.02726 +/- 0.00067
VELX DIOA 0.00348 +/- 0.00008
VELY DIOA 0.01069 +/- 0.00010
VELZ DIOA -0.00937 +/- 0.00006
STAX KRVB 3855261.23189 +/- 0.00206
STAY KRVB -5049734.77842 +/- 0.00185
STAZ KRVB 563057.86369 +/- 0.00116
VELX KRVB -0.00622 +/- 0.00035
VELY KRVB -0.00035 +/- 0.00031
VELZ KRVB 0.01249 +/- 0.00019
STAX DIOB 4595212.45177 +/- 0.00117
STAY DIOB 2039473.63506 +/- 0.00156
STAZ DIOB 3912617.94881 +/- 0.00090
VELX DIOB 0.00348 +/- 0.00008
VELY DIOB 0.01068 +/- 0.00010
VELZ DIOB -0.00937 +/- 0.00006
STAX YEMB -1224405.08181 +/- 0.00109
STAY YEMB -2689216.49980 +/- 0.00092
STAZ YEMB 5633649.75663 +/- 0.00085
VELX YEMB -0.02132 +/- 0.00007
VELY YEMB -0.00477 +/- 0.00006
VELZ YEMB 0.00024 +/- 0.00005
STAX KRVB 3855261.23097 +/- 0.00356
STAY KRVB -5049734.78428 +/- 0.00322
STAZ KRVB 563057.85691 +/- 0.00194
VELX KRVB -0.00622 +/- 0.00035
VELY KRVB -0.00035 +/- 0.00031
VELZ KRVB 0.01249 +/- 0.00019
-------------------------------------------------------------
STAX KRVB 3855261.23189 +/- 0.00206
STAY KRVB -5049734.77842 +/- 0.00185
STAZ KRVB 563057.86369 +/- 0.00116
VELX KRVB -0.00622 +/- 0.00035
VELY KRVB -0.00035 +/- 0.00031
VELZ KRVB 0.01249 +/- 0.00019
STAX DIOB 4595212.45177 +/- 0.00117
STAY DIOB 2039473.63506 +/- 0.00156
STAZ DIOB 3912617.94881 +/- 0.00090
VELX DIOB 0.00348 +/- 0.00008
VELY DIOB 0.01068 +/- 0.00010
VELZ DIOB -0.00937 +/- 0.00006
STAX YEMB -1224405.08181 +/- 0.00109
STAY YEMB -2689216.49980 +/- 0.00092
STAZ YEMB 5633649.75663 +/- 0.00085
VELX YEMB -0.02132 +/- 0.00007
VELY YEMB -0.00477 +/- 0.00006
VELZ YEMB 0.00024 +/- 0.00005
-------------------------------------------------------------
STAX DIOB 4595212.45177 +/- 0.00117
STAY DIOB 2039473.63506 +/- 0.00156
STAZ DIOB 3912617.94881 +/- 0.00090
VELX DIOB 0.00348 +/- 0.00008
VELY DIOB 0.01068 +/- 0.00010
VELZ DIOB -0.00937 +/- 0.00006
STAX YEMB -1224405.08181 +/- 0.00109
STAY YEMB -2689216.49980 +/- 0.00092
STAZ YEMB 5633649.75663 +/- 0.00085
VELX YEMB -0.02132 +/- 0.00007
VELY YEMB -0.00477 +/- 0.00006
VELZ YEMB 0.00024 +/- 0.00005
STAX KRVB 3855261.23097 +/- 0.00356
STAY KRVB -5049734.78428 +/- 0.00322
STAZ KRVB 563057.85691 +/- 0.00194
VELX KRVB -0.00622 +/- 0.00035
VELY KRVB -0.00035 +/- 0.00031
VELZ KRVB 0.01249 +/- 0.00019
-------------------------------------------------------------
-------------------------------------------------------------
STAX DIOA 4595215.24319 +/- 0.00087
STAY DIOA 2039475.41598 +/- 0.00112
STAZ DIOA 3912615.02726 +/- 0.00067
VELX DIOA 0.00348 +/- 0.00008
VELY DIOA 0.01069 +/- 0.00010
VELZ DIOA -0.00937 +/- 0.00006
STAX DIOB 4595212.45177 +/- 0.00117
STAY DIOB 2039473.63506 +/- 0.00156
STAZ DIOB 3912617.94881 +/- 0.00090
VELX DIOB 0.00348 +/- 0.00008
VELY DIOB 0.01068 +/- 0.00010
VELZ DIOB -0.00937 +/- 0.00006
STAX YEMB -1224405.08181 +/- 0.00109
STAY YEMB -2689216.49980 +/- 0.00092
STAZ YEMB 5633649.75663 +/- 0.00085
VELX YEMB -0.02132 +/- 0.00007
VELY YEMB -0.00477 +/- 0.00006
VELZ YEMB 0.00024 +/- 0.00005
STAX KRVB 3855261.23097 +/- 0.00356
STAY KRVB -5049734.78428 +/- 0.00322
STAZ KRVB 563057.85691 +/- 0.00194
VELX KRVB -0.00622 +/- 0.00035
VELY KRVB -0.00035 +/- 0.00031
VELZ KRVB 0.01249 +/- 0.00019
-------------------------------------------------------------
STAX DIOA 4595215.24319 +/- 0.00087
STAY DIOA 2039475.41598 +/- 0.00112
STAZ DIOA 3912615.02726 +/- 0.00067
VELX DIOA 0.00348 +/- 0.00008
VELY DIOA 0.01069 +/- 0.00010
VELZ DIOA -0.00937 +/- 0.00006
STAX KRVB 3855261.23189 +/- 0.00206
STAY KRVB -5049734.77842 +/- 0.00185
STAZ KRVB 563057.86369 +/- 0.00116
VELX KRVB -0.00622 +/- 0.00035
VELY KRVB -0.00035 +/- 0.00031
VELZ KRVB 0.01249 +/- 0.00019
STAX DIOB 4595212.45177 +/- 0.00117
STAY DIOB 2039473.63506 +/- 0.00156
STAZ DIOB 3912617.94881 +/- 0.00090
VELX DIOB 0.00348 +/- 0.00008
VELY DIOB 0.01068 +/- 0.00010
VELZ DIOB -0.00937 +/- 0.00006
STAX YEMB -1224405.08181 +/- 0.00109
STAY YEMB -2689216.49980 +/- 0.00092
STAZ YEMB 5633649.75663 +/- 0.00085
VELX YEMB -0.02132 +/- 0.00007
VELY YEMB -0.00477 +/- 0.00006
VELZ YEMB 0.00024 +/- 0.00005
"""

progs = [{'name': 'test-sinex',
          'args': ['DATA_DIR/foobar'],
          'exit': 1},
         {'name': 'test-sinex',
          'args': ['DATA_DIR/dpod2020.snx'],
          'exit': 0},
         {'name': 'test-site-id-ndomes',
          'args': ['DATA_DIR/dpod2020.snx',
                   'DIOA',
                   'DIOB',
                   'FOO',
                   'BAR',
                   'DIOC'],
          'sout': ds1,
          'exit': 0},
         {'name': 'test-site-id-wdomes',
          'args': ['DATA_DIR/dpod2020.snx',
                   'DIOA',
                   '12602S011',
                   'A',
                   'B',
                   'DIOB',
                   '12602S012',
                   'FOO',
                   'BAR',
                   'BAR',
                   '12602S011',
                   'DIOC',
                   '123'],
          'sout': ds2,
          'exit': 0},
         {'name': 'test-site-eccentricity',
          'args': ['DATA_DIR/dpod2020.snx',
                   'FOO',
                   'DIOA',
                   'BAR1',
                   'DIOB',
                   'DIOC'],
          'sout': ds3,
          'exit': 0},
         {'name': 'test-parameter-exists',
          'args': [],
          'exit': 0},
         {'name': 'test-data-reject',
          'args': ['DATA_DIR/dpod2020.snx',
                   'FOO',
                   'DIOA',
                   'BAR1',
                   'DIOB',
                   'DIOC'],
          'sout': ds4,
          'exit': 0},
         {'name': 'test-get-solution',
          'args': ['DATA_DIR/dpod2020.snx',
                   'DIOA',
                   'DIOB',
                   'KRVB',
                   'YEMB'],
          'sout': ds5,
          'exit': 0},
         ]
