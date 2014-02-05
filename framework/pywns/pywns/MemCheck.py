#!/usr/bin/python

import subprocess
import os
import sys
import signal
import glob

def searchPathToSDK(path):
    rootSign = ".thisIsTheRootOfWNS"
    while rootSign not in os.listdir(path):
        if path == os.sep:
            # arrived in root dir
            return None
        path, tail = os.path.split(path)
    return os.path.abspath(path)

class SignalHandler:
    def __init__(self, subprocess):
        self.subprocess = subprocess

    def __call__(self, signal, frame):
        # forward the signal to the subprocess ...
        os.kill(self.subprocess.pid, signal)
        # ... and wait for process to terminate
        returncode = self.subprocess.wait()
        sys.exit(returncode)

class Runner:
    def __init__(self,
                 # use args (list of str) to provide program and arguments for memcheck
                 args=[],
                 cwd=None,
                 num_callers=20,
                 leak_check='full',
                 leak_resolution='low',
                 errorExitCode="192",
                 suppressions=[],
                 useOpenWNSSuppressions = True):
        self.env = os.environ
        self.env['GLIBCPP_FORCE_NEW']='1'
        self.env['GLIBCXX_FORCE_NEW']='1'
        self.cwd = cwd
        self.executable = 'valgrind'
        self.args = [
            '--tool=memcheck',
            '--num-callers='+str(num_callers),
            '--leak-check='+leak_check,
            '--leak-resolution='+leak_resolution,
            '--error-exitcode='+errorExitCode
        ]

        pathToSDK = searchPathToSDK(os.path.abspath(os.path.dirname(sys.argv[0])))

        if pathToSDK == None:
            print "Error! You are note within an openWNS-SDK. Giving up"
            exit(1)

        if useOpenWNSSuppressions:
            self.args.append('--suppressions='+os.path.join(pathToSDK, "config", "valgrind.supp"))
        for suppressionsFile in suppressions:
            self.args.append('--suppressions='+suppressionsFile)
        self.args += args

    def run(self):
        sp = subprocess.Popen(self.args, bufsize=0, executable=self.executable, env=self.env, cwd=self.cwd)
        # save old signal handler for SIGINT
        oldSigIntHandler = signal.getsignal(signal.SIGINT)
        # install new signal handler for SIGINT
        signal.signal(signal.SIGINT, SignalHandler(sp))
        # return returncode of subprocess
        returncode = sp.wait()
        # after process ended, restore old signal handler
        signal.signal(signal.SIGINT, oldSigIntHandler)
        return returncode

if __name__ == "__main__":
    r = Runner(sys.argv[1:])
    returncode = r.run()
    sys.exit(returncode)
