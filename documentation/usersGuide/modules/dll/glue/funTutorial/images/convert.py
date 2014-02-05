#!/usr/bin/env python

import os
import subprocess

currentDir = os.getcwd()

for file in os.listdir(currentDir):
    if file.endswith('.fig'):
        print currentDir + '/' + file, currentDir + '/' + file.rstrip('.fig')
        subprocess.call(['/usr/bin/fig2dev', '-m1.5', '-Lpng', currentDir + '/' + file, currentDir + '/' + file.rstrip('.fig') + '.png'])
