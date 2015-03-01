PyGLPK Readme
=============

========
Overview
========

PyGLPK is a Python module which encapsulates the functionality of the `GNU Linear Programming Kit (GLPK) <https://www.gnu.org/software/glpk/>`_. The GLPK allows one to specify linear programs (LPs) and mixed integer programs (MIPs), and to solve them with either simplex, interior-point, or branch-and-bound algorithms. The goal of PyGLPK is to give one access to all documented functionality of GLPK.

=======
License
=======

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
`GNU General Public License <http://www.gnu.org/copyleft/gpl.html>`_ for more details.

============
Availability
============

To get the latest version, see: https://github.com/bradfordboyle/pyglpk

=======================
Building and Installing
=======================

------------
Dependencies
------------

Building this module requires the following external dependencies:

* `Python <https://www.python.org/>`_ 2.6 or greater, or 3.2 or greater
* `GLPK <https://www.gnu.org/software/glpk/>`_ version 4.45 or greater

For compatibility with older versions of Python or GLPK, see `PyGLPK version 0.3 <http://tfinley.net/software/pyglpk/>`_.

On Linux (and other \*nix), GLPK can be built from source 

    .. code-block:: shell

        tar vxfz glpk-4.55.tar.bz
        cd glpk-4.55
        ./configure
        make
        sudo make install

On OS X, GLPK can be built from source (as above) or using `Homebrew <http://brew.sh/>`_.

    .. code-block:: shell

        brew install glpk

On Windows, the easiest way to install GLPK is using a pre-build binary from the `WinGLPK project <http://winglpk.sourceforge.net/>`_.

------------
Installation
------------

To install PyGLPK:

.. code-block:: shell

    python setup.py install

To test the installation:

.. code-block:: shell

    python tests/test_glpk.py

=======
Credits
=======

PyGLPK (versions 0.1 – 0.3) was written by Thomas Finley <`tfinley@gmail.com <mailto:tfinley@gmail.com>`_>.

PyGLPK (versions 0.4) is written and maintained by:

* Bradford Boyle <`bradford.d.boyle@gmail.com <mailto:bradford.d.boyle@gmail.com>`_>
* Joshua Arnott <`josh@snorfalorpagus.net <mailto:josh@snorfalorpagus.net>`_>
