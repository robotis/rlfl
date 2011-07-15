"""
    RLF test suite.
"""
import unittest, os, re

import sys
here = os.path.dirname(__file__)
sys.path.append(here + '/..') 
del sys

def find():
    svn = re.compile('\.svn')
    test = re.compile('test.+\.py$')
    alltests = unittest.TestSuite()
    for path, dirs, files in os.walk(here):
        if svn.search(path):
            continue
        for f in files:
            if test.search(f):
                module = __import__(f.split('.')[0])
                alltests.addTest(unittest.findTestCases(module))
    return alltests

if __name__ == '__main__':
    unittest.main(defaultTest='find')