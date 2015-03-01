Discussion Around PyGLPK
========================

=============
Existing work
=============

A Python binding to GLPK already exists in the form of `Python-GLPK <http://www.ncc.up.pt/~jpp/code/python-glpk/>`_, but as it is automatically created through `SWIG <http://www.swig.org/>`_ it is not very `Pythonic <http://en.wikipedia.org/wiki/Python_%28programming_language%29>`_.

=====================================
High Level Comparison of C and PyGLPK
=====================================

To use the C API, one would first `#include "glpk.h"`, create an LPX structure through `lpx_create_prob()`, and thence manipulate this structure with `lpx_*` functions to set the data, run the optimization, and retrieve the desired values.

To use this Python module, one would import the `glpk` module, create an LPX Python object through `glpk.LPX()`, and thence manipulate this object and the objects it contains to set the data, run the optimization, and retrieve the desired values. The Python module calls the C API to support these operations.

=================
Design Philosophy
=================

PyGLPK has objects floating around everywhere, and very few actual methods. Wherever possible and sensible, one gets and sets traits of the problem by accessing a method and directly assigning those traits. An example of this showing how PyGPLK works and how it does not work might be interesting.

PyGLPK is like this:

.. code-block:: python

    lp.maximize = True
    lp.cols.add(10)
    for col in lp.cols:
    	col.name = 'x%d' % col.index
    	col.bounds = 0.0, None
    	lp.obj[col.index] = 1.0
    del lp.cols[2,4,5]

PyGLPK is **not** like this:

.. code-block:: python

    lp.set_maximize()
    lp.add_cols(10)
    for cnum in xrange(lp.num_cols()):
    	lp.set_col_name(cnum, 'x%d' % cnum)
    	lp.set_col_bounds(cnum, 0.0, None)
    	lp.set_obj_coef(cnum, 1.0)
    lp.del_cols([2,4,5])

Both design strategies would accomplish the same thing, but there are advantages in the first way. For example, if I tell you only that columns of an LP are stored in a sequence cols, for free you already know a lot (assuming you're familiar with Python):

* You know how to get the number of columns in the LP.
* You know how to get a particular column or a range of columns.
* You know they're indexed from 0.
* You know how to delete them.

=========================================
Differences Between C GLPK API and PyGLPK
=========================================

--------
Indexing
--------

Unlike the C API, everything is indexed from `0`, not `1`: the user does not pass in arrays (or lists!) where the first element is ignored. Further, one indexes (for example) the first row by asking for row `0`, not `1`.

In the comparative examples of parallel C and Python code, wherever possible and appropriate I sprinkle `+1` in the C code. Of course, only a lunatic would really write code that way, but I do this to highlight this difference, and second, make it more obvious which portions of C and Python correspond to each other: it's far easier to see the relation between `[7, 3, 1, 8, 6]` and `[7+1, 3+1, 1+1, 8+1, 6+1]`, versus `[8, 4, 2, 9, 7]`.

PyGLPK also honors Python's quirk of "negative indexing" used to count from the end of a sequence, e.g., where index `-1` refers to the last item, `-2` second to last item, and so forth. This can be convenient. For example, after adding a row, you can refer to this row by `lp.rows[-1]` rather than having to be aware of its absolute index.

--------------
Error Handling
--------------

The GLPK's approach to errors in arguments is deeply peculiar. It writes an error message and terminate the process, in contrast with many APIs that instead set or return an error code which can be checked. The PyGLPK takes the more Pythonic approach of throwing catchable exceptions for illegal arguments. Unfortunately, to avoid the process being terminated, this requires that absolutely every argument be vetted, requiring that PyGLPK have the additional overhead of doing sometimes rather detailed checks of arguments (which are, of course, checked yet again when the GLPK has access to them). It seems unlikely that GLPK's design will be improved in this area.
