#!/usr/bin/python3

import os, sys, subprocess, argparse

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

progs = [
  {'name': 'test-sinex', 'args': ['DATA_DIR/foobar'], 'exit': 1},
  {'name': 'test-sinex', 'args': ['DATA_DIR/dpod.snx'], 'exit': 0},
  {'name': 'test-site-id-ndomes', 'args': ['DATA_DIR/dpod.snx', 'DIOA', 'DIOB', 'FOO', 'BAR', 'DIOC'], 'sout': ds1, 'exit': 0},
  {'name': 'test-site-id-wdomes', 'args': ['DATA_DIR/dpod.snx', 'DIOA', '12602S011', 'A', 'B', 'DIOB', '12602S012', 'FOO', 'BAR', 'BAR', '12602S011', 'DIOC', '123'], 'sout': ds2, 'exit': 0},
  {'name': 'test-site-eccentricity', 'args': ['DATA_DIR/dpod.snx', 'FOO', 'DIOA', 'BAR1', 'DIOB', 'DIOC'], 'sout': ds3, 'exit': 0},
  {'name': 'test-parameter-exists', 'args': [], 'exit': 0},
  ]

def check_file_vs_str(file, str):
  with open(file, 'r') as fin:
    ## read whole file to a string
    fstr = fin.read()
    return (fstr == str)

class myFormatter(argparse.ArgumentDefaultsHelpFormatter,
                  argparse.RawTextHelpFormatter):
    pass

parser = argparse.ArgumentParser(
    formatter_class=myFormatter,
    description=
    'Run validatation programs against SOFA library',
    epilog=('''National Technical University of Athens,
    Dionysos Satellite Observatory\n
    Send bug reports to:
    Xanthos Papanikolaou, xanthos@mail.ntua.gr
    May, 2023'''))

parser.add_argument(
    '--progs-dir',
    metavar='PROGS_DIR',
    dest='progs_dir',
    default=os.path.abspath(os.getcwd()),
    required=False,
    help='Directory with test executables (top-level test directory')

parser.add_argument(
    '--data-dir',
    metavar='DATA_DIR',
    dest='data_dir',
    default=os.path.abspath(os.getcwd()),
    required=False,
    help='Directory with test data')

parser.add_argument(
    '--verbose',
    action='store_true',
    dest='verbose',
    help='Verbose mode on')

if __name__ == '__main__':
  ## parse cmd
  args = parser.parse_args()

  verboseprint = print if args.verbose else lambda *a, **k: None

## temporary file for logging/output
  temp_fn = '.testemp'

  ## call each of the test programs with siutable args
  for dct in progs:
## test-program (executable)
    exe = os.path.join(args.progs_dir, dct['name']) + '.out'
## check that the program is where expected
    if not os.path.isfile(exe):
      print('ERROR Failed to find executable {:}'.format(exe), fout=sys.stderr)
      sys.exit(1)
## write output (if needed) here
    ftmp = open(temp_fn, "w")
## replace DATA_DIR with the actual data_dir in any of the arguments
    cmdargs = [ x.replace('DATA_DIR',args.data_dir) for x in dct['args'] ]
## run the command, catch output and exit code
    verboseprint('Running command: {:}'.format([exe]+cmdargs))
    result = subprocess.run([exe]+cmdargs, stdout=ftmp, check=False)
    ftmp.close()
## check the return code
    if result.returncode != dct['exit']:
      print('ERROR Expected a return code {:} and got {:}; exe {:}'.format(dct['exit'], result.returncode, exe), file=sys.stderr)
      sys.exit(2)
    else:
      verboseprint('\tExit Code OK')
## check the output against the should-be output
    if 'sout' in dct:
      if check_file_vs_str(temp_fn, dct['sout']):
        verboseprint('\tCommand Output OK')
      else:
        print('ERROR Program\'s {:} output different than expected'.format(exe), file=sys.stderr)
