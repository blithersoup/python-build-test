import glob

with open('README.rst', encoding='utf-8') as readme:
    LONG_DESCRIPTION = readme.read()

EXTRAS = {}

METADATA = {
    "name": "build-test",
    "description": "test use only",
    "long_description": LONG_DESCRIPTION,
    "python_requires": '>=3.6',
}

import re
import sys
import os

# just import these always and fail early if not present
from setuptools import setup
import distutils


import os.path, glob, stat, shutil
from distutils.core import setup, Command
from distutils.extension import read_setup_file

extensions = read_setup_file('Setup')


# headers to install
headers = glob.glob(os.path.join('src_c', '*.h'))


# required. This will be filled if doing a Windows build.
cmdclass = {}


def add_command(name):
    def decorator(command):
        assert issubclass(command, distutils.cmd.Command)
        cmdclass[name] = command
        return command

    return decorator


# finally,
# call distutils with all needed info
PACKAGEDATA = {
    "cmdclass": cmdclass,
    "packages": ['testmod'],
    "package_dir": {'testmod': 'src_py'},
    "headers": headers,
    "ext_modules": extensions,
    "zip_safe": False,
}

try:
    setup(**PACKAGEDATA)
except:
    raise
