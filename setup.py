from __future__ import print_function

import os
try:
    import setuptools
except ImportError:
    pass
from distutils.core import setup, Extension, Distribution
from ctypes.util import find_library

def find_lib(name):
    """
    Attempt to find the full path to a shared library
    
    Parameters
    ----------
    name : string
        Name of the library, e.g. 'glpk'
    
    Returns
    -------
    Full path to the library, e.g. '/usr/lib64/libglpk.so'
    """
    # try ctypes.util.find_library
    path = find_library(name)
    if os.path.exists(path):
        return path
    # try whereis
    path = os.popen('whereis lib{}'.format(name)).read().strip()
    if path is not None:
        path = ' '.join(path.split(' ')[1:])
        if os.path.exists(path):
            return path
    # try ldconfig
    data = os.popen('/sbin/ldconfig -p').read()
    candidates = sorted([line for line in data.split('\n') if '{}.'.format(name) in line], key=lambda s: s.split(' ')[0])
    if candidates:
        path = candidates[0].split(' => ')[1]
        if os.path.exists(path):
            return path

glpk_lib_path = find_lib('glpk')
if not glpk_lib_path:
    raise RuntimeError("Couldn't find libglpk.")

lib_dir = os.path.dirname(glpk_lib_path)
prefix = os.path.dirname(lib_dir)
include_dir = os.path.join(prefix, 'include')

libraries = ['glpk']

# libgmp is an optional dependency of libglpk
gmp_lib_path = find_lib('gmp')
if gmp_lib_path is not None:
    libraries.append('gmp')

sources = 'glpk 2to3 lp barcol bar obj util kkt tree environment'
source_roots = sources.split()

_glpk = Extension(
    'glpk.glpk',
    sources=[os.path.join('src', '{}.c'.format(root)) for root in source_roots],
    library_dirs=[lib_dir,],
    include_dirs=[include_dir,],
    libraries=libraries,
)

class BinaryDistribution(Distribution):
    def is_pure(self):
        return False

ld = """The PyGLPK module gives one access to the functionality
of the GNU Linear Programming Kit.  
"""

setup_args = dict(
    name = 'glpk',
    version ='0.4',
    description ='PyGLPK, a Python module encapsulating GLPK.',
    long_description = ld,
    author = 'Thomas Finley',
    author_email = 'tfinley@gmail.com',
    url = 'http://tfinley.net/software/pyglpk/',
    maintainer = 'Bradford D. Boyle',
    maintainer_email = 'bradford.d.boyle@gmail.com',
    license = 'GPL',
    classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: GNU General Public License (GPL)',
        'Programming Language :: C',
        'Programming Language :: Python',
        'Operating System :: POSIX',
        'Operating System :: MacOS :: MacOS X',
        'Topic :: Scientific/Engineering :: Mathematics',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
    ext_modules = [_glpk,],
    distclass = BinaryDistribution,
    packages = ['glpk'],
)

setup(**setup_args)
