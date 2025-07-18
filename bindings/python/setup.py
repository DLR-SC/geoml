import sys
import os
import subprocess
from setuptools import setup, find_packages

try:
    from skbuild import setup
except ImportError:
    print('scikit-build is required to build from source.')
    print('Installation:  python -m pip install scikit-build')
    subprocess.check_call(
        [sys.executable, "-m", "pip", "install", "scikit-build"])
    from skbuild import setup

__version__ = '1.3.1'

setup(
    name='geoml',
    version=__version__,
    author='Martin Siggel, DLR AT-TWK',
    author_email='martin.siggel@dlr.de',
    maintainer_email='martin.siggel@dlr.de',
    url='https://gitlab.dlr.de/sigg_ma/geoml',
    description='Python interface to geoml',
    cmake_args=["-DBUILD_SHARED_LIBS=OFF"],
    packages=find_packages(where=os.path.dirname(os.path.abspath(__file__))),
    long_description=''
)
