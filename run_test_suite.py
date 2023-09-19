#!/usr/bin/python3

import os
import sys
import subprocess
import argparse
import ftplib
import importlib.util


def runtime_load_progs(pyfile):
    spec = importlib.util.spec_from_file_location("bar.baz", pyfile)
    foo = importlib.util.module_from_spec(spec)
    sys.modules["bar.baz"] = foo
    spec.loader.exec_module(foo)
    return foo.progs


def check_file_vs_str(file, str):
    with open(file, 'r') as fin:
        # read whole file to a string
        fstr = fin.read()
        return (fstr == str)


class myFormatter(argparse.ArgumentDefaultsHelpFormatter,
                  argparse.RawTextHelpFormatter):
    pass


parser = argparse.ArgumentParser(
    formatter_class=myFormatter,
    description='Run validatation programs for the libsinex library',
    epilog=('''National Technical University of Athens,
    Dionysos Satellite Observatory\n
    Send bug reports to:
    Xanthos Papanikolaou, xanthos@mail.ntua.gr
    Sep, 2023'''))

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
    '--reference-results',
    metavar='REFERENCE_RESULTS_FILE',
    dest='ref_respy',
    default=os.path.join(
        os.path.abspath(
            os.getcwd()),
        'test/reference_results.py'),
    required=False,
    help='File with reference test results, for comparing against')

parser.add_argument(
    '--verbose',
    action='store_true',
    dest='verbose',
    help='Verbose mode on')

if __name__ == '__main__':
    # parse cmd
    args = parser.parse_args()

# verbose print
    verboseprint = print if args.verbose else lambda *a, **k: None

# import reference results (including the progs dictionary)
    if not os.path.isfile(args.ref_respy):
        print(
            'ERROR Failed to locate reference results file {:}'.format(
                args.ref_respy),
            file=sys.stderr)
        sys.exit(1)
    progs = runtime_load_progs(args.ref_respy)

# Download the latest dpod2020 SINEX file and store it as DATA_DIR/dpod2020.snx
    if not os.path.isfile(os.path.join(args.data_dir, 'dpod2020.snx')):
        verboseprint(
            'Downloading latest dpod2020 SINEX file from doris.ign.fr')
        ftp = ftplib.FTP('doris.ign.fr')
        ftp.login()
        ftp.cwd('pub/doris/products/dpod/dpod2020')
        ftp.retrbinary("RETR {}".format('dpod2020_current.snx.Z'), open(
            os.path.join(args.data_dir, 'dpod2020.snx.Z'), 'wb').write)
        ftp.quit()
# uncompress
        subprocess.run(['uncompress', '{}'.format(
            os.path.join(args.data_dir, 'dpod2020.snx.Z'))], check=True)

# temporary file for logging/output
    temp_fn = '.testemp'

    # call each of the test programs with siutable args
    for dct in progs:
        # test-program (executable)
        exe = os.path.join(args.progs_dir, dct['name']) + '.out'
# check that the program is where expected
        if not os.path.isfile(exe):
            print(
                'ERROR Failed to find executable {:}'.format(exe),
                file=sys.stderr)
            sys.exit(1)
# write output (if needed) here
        ftmp = open(temp_fn, "w")
# replace DATA_DIR with the actual data_dir in any of the arguments
        cmdargs = [x.replace('DATA_DIR', args.data_dir) for x in dct['args']]
# run the command, catch output and exit code
        verboseprint('Running command: {:}'.format([exe] + cmdargs))
        result = subprocess.run([exe] + cmdargs, stdout=ftmp, check=False)
        ftmp.close()
# check the return code
        if result.returncode != dct['exit']:
            print('ERROR Expected a return code {:} and got {:}; exe {:}'.format(
                dct['exit'], result.returncode, exe), file=sys.stderr)
            sys.exit(2)
        else:
            verboseprint('\tExit Code OK')
# check the output against the should-be output
        if 'sout' in dct:
            if check_file_vs_str(temp_fn, dct['sout']):
                verboseprint('\tCommand Output OK')
            else:
                print(
                    'ERROR Program\'s {:} output different than expected'.format(exe),
                    file=sys.stderr)
