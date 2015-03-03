===========
SAT Example
===========

In this section we show a simple example of how to use PyGLPK to build a `SAT solver <http://en.wikipedia.org/wiki/Boolean_satisfiability_problem>`_.

How to Solve
============

Suppose one has a CNF expression, that is, a conjunction (and-ing) of several disjunctions (or-ing) of logical literals, e.g.:

.. math::

    ( ¬x_1 ∨ ¬x_3 ∨ ¬x_4 ) ∧ ( x_2 ∨ x_3 ∨ ¬x_4 ) ∧ ( x_1 ∨ ¬x2 ∨ x_4 ) ∧ ( x_1 ∨ x_3 ∨ x_4 ) ∧ ( ¬x_1 ∨ x_2 ∨ ¬x_3 )

Suppose we want to find truth values to all four :math:`x_i` variables so that the CNF expression is true. This problem has been viewed from many different ways, but we'll see how to encode and (we hope) solve it within a mixed-linear program. We will build a function `solve_sat` to satisfy a given CNF.

First, we need to define how we encode our input CNF expressions that we want to satisfy:

* Each logical literal is represented as either a positive or negative integer, where i and -i correspond to the logical literals :math:`x_i` and :math:`¬x_i`, respectively.
* Each clause in the expression, i.e., disjunction of literals, is represented as a tuple of such encoding of literals, e.g., `(-1, 2, -3)` represents the disjunction :math:`( ¬x_1 ∨ x_2 ∨ ¬x_3 )`.
* The entire conjunctive expression is a list of such tuples, e.g., the expression above would have encoding:
* `[(-1, -3, -4), (2, 3, -4), (1, -2, 4), (1, 3, 4), (-1, 2, -3)]`

The function will return either None if it could not find a satisfying assignment, or a list of booleans assignment representing the satisfying assignment, where the truth of each logical variable :math:`x_i` is held in `assignment[i-1]`.

This is our strategy of how to solve this with a mixed integer program:

* For each logical variable :math:`x_i`, have a structural variable (column) representing both its positive and negative literals :math:`x_i` and :math:`¬x_i`. These structural variables should be either 0 or 1 depending on whether the corresponding literal is false or true, respectively.
* Because we want literal consistency, we specify that the sum of all literal pair structural variables must be 1. This forbids literals for a given logical variable from being set both false or both true.
* For each clause, we define a constraint specifying that the sum of all its literal structural variables must be at least 1. This forces each clause to be true.
* First we run the simplex solver (implying a relaxed problem where the structural variables can range from 0 to 1). Then we run the integer solver (the structural variables can be either 0 or 1).
* If the integer solver finds an optimal solution, excellent. (If not we return `None`.) If a positive literal has a corresponding structural variable with value 1, then we assign its logical variable to true.

The Implementation
==================

Here is the implementation of that function:

.. code-block:: python

    def solve_sat(expression):
        if len(expression)==0: return [] # Trivial case.  Otherwise count vars.
        numvars = max([max([abs(v) for v in clause]) for clause in expression])
        lp = glpk.LPX()                  # Construct an empty linear program.
        glpk.env.term_on = False         # Stop the annoying output.
        lp.cols.add(2*numvars)           # As many columns as there are literals.
        for col in lp.cols:              # Literal must be between false and true.
            col.bounds = 0.0, 1.0
        def lit2col(lit):                # Function to compute column index.
            return [2*(-lit)-1,2*lit-2][lit>0]
        for i in xrange(1, numvars+1):   # Ensure "oppositeness" of literals.
            lp.rows.add(1)
            lp.rows[-1].matrix = [(lit2col(i), 1.0), (lit2col(-i), 1.0)]
            lp.rows[-1].bounds = 1.0     # Must sum to exactly 1.
        for clause in expression:        # Ensure "trueness" of each clause.
            lp.rows.add(1)
            lp.rows[-1].matrix = [(lit2col(lit), 1.0) for lit in clause]
            lp.rows[-1].bounds = 1, None # At least one literal must be true.
        retval = lp.simplex()            # Try to solve the relaxed problem.
        assert retval == None            # Should not fail in this fashion.
        if lp.status!='opt': return None # If no relaxed solution, no exact sol.

        for col in lp.cols:
            col.kind = int
        retval = lp.integer()            # Try to solve this integer problem.
        assert retval == None            # Should not fail in this fashion.
        if lp.status != 'opt': return None
        return [col.value > 0.99 for col in lp.cols[::2]]

The Explanation
===============

We shall now go over this function section by section!

.. code-block:: python

    if len(expression)==0: return [] # Trivial case.  Otherwise count vars.
    numvars = max([max([abs(v) for v in clause]) for clause in expression])

Pretty straightforward non-PyGLPK Python code. The first line just takes care of the boundary case where we have an empty expression. In the second line, from the expression, we find the maximum indexed logical variable we have, and use that as our count of the number of logical variables.

.. code-block:: python

    lp = glpk.LPX()                  # Construct an empty linear program.

Calls the LPX constructor to construct an empty linear program.

.. code-block:: python

    glpk.env.term_on = False         # Stop the annoying output.

Within the glpk module member env, of type Environment. By assigning to various attributes contained within env, you can affect behavior of the GLPK object. In this case, we are assigning `False` to the `term_on` (terminal output on) parameter, to suppress all output.

.. code-block:: python

    lp.cols.add(2*numvars)           # As many columns as there are literals.

Recall that we want as many structural variables (columns) in the linear program as there are possible literals over all our logical variables. Each logical variable xi has two possible literals: itself (:math:`x_i`), and its negation (:math:`¬x_i`).

Initially we have no columns at all. So, we get the `lp.cols` object, the LP's column container, and call its add method, telling it to add as many columns as there are twice the number of logical variables.

.. code-block:: python

    for col in lp.cols:              # Literal must be between false and true.
        col.bounds = 0.0, 1.0

In addition to creating new columns, the lp.cols collection object is used to access individual columns. In this case, we are iterating over every column. Once we have each column, we assign its bounds to be between 0 and 1. This will force the structural variable associated with this column to fall between these values.

These `lp.cols` objects act like sequences (albeit with restrictions on their content). In order to access their elements (in this case, columns), we can either iterate over the columns as we do here, or index into them directly as `lp.cols[colnum]`.

.. code-block:: python

    def lit2col(lit):                # Function to compute column index.
        return [2*(-lit)-1,2*lit-2][lit>0]

This is just a helper function for our own benefit. Recall that we have a structural variable for each possible literal. This function merely maps a literal code to a column index. This function maps literal code 1 to column index 0, -1 to column index 1, 2 to 2, -2 to 3, 3 to 4, -3 to 5, 4 to 6, and so forth.

.. code-block:: python

    for i in xrange(1, numvars+1):   # Ensure "oppositeness" of literals.
        lp.rows.add(1)
        lp.rows[-1].matrix = [(lit2col(i), 1.0), (lit2col(-i), 1.0)]
        lp.rows[-1].bounds = 1.0     # Must sum to exactly 1.

These are our consistency constraints to make sure two opposite literals are not both true or not both false. For each logical variable, we add one new row (what will be a consistency constraint). Notice that we are now using the `lp.rows` object! This is similar to the `lp.cols` object (in reality they are the same type), except it holds rows instead of columns.

Then we get the last row, which is the one we just added (note the use of the -1 index to address the last row), and assign to its matrix attribute. The matrix attribute for any row or column corresponds to the entries of the row or column vector in our constraint matrix. In this case, we are setting the two locations of this constraint matrix row corresponding to the two structural variables for :math:`x_i` and :math:`¬xi` to 1.0.

Finally, we set the bounds attribute for this row's auxiliary variable to 1.0. Note that this differs from the previous bound definition: here we use only one number. This indicates we want an equality constraint. (It would have been equivalent to assign 1.0, 1.0 .)

.. code-block:: python

    for clause in expression:        # Ensure "trueness" of each clause.
        lp.rows.add(1)
        lp.rows[-1].matrix = [(lit2col(lit), 1.0) for lit in clause]
        lp.rows[-1].bounds = 1, None # At least one literal must be true.

These are our clause satisfiability constraints, to make sure that at least one literal in each clause is true. For each clause we, again, add a single row.

We access this last added row, and assign to its matrix attribute. In this case, we are specifying that the row's constraint coefficients should be 1.0 for each structural variable corresponding to each literal within this clause.

Finally, we set the bounds attribute for this row, establishing the lower bound 1 and upper bound `None`. An assignment of `None` indicates unboundedness in this direction.

.. code-block:: python

    retval = lp.simplex()            # Try to solve the relaxed problem.
    assert retval == None            # Should not fail in this fashion.

Now we employ the simplex solver to attempt to solve a relaxed version of the problem. (Relaxed in the sense that the variables can be non-integers.) We do this because, at the point it is called, the integer optimization method requires an existing optimal basic solution.

The method returns `None` unless the method was unable to start the search due to a fault in the problem definition (which returns the string `'fault'`), or because the simplex search terminated prematurely (due to one of several possible conditions).

In a real application we would probably be interested in seeing what went wrong, and try to fix it. However, for this toy example, we just noisily fail with an exception.

.. code-block:: python

    if lp.status!='opt': return None # If no relaxed solution, no exact sol.

Note that "not terminating prematurely" does not mean "an optimal solution was found!" It merely means that the search did not terminate abnormally. In order to check whether we found an optimal solution (as opposed to, say, having determined that the problem is infeasible), we check the status attribute. If it does not hold `'opt'`, then we return `None` to indicate that we could not find a satisfying assignment.

At this point we hold an optimal basic solution to the relaxed problem. We now go about turning this into a mixed-integer program.

.. code-block:: python

    for col in lp.cols:
        col.kind = int

We first assign the columns the kind of int to indicate that we want this to be an integer program. The kind attribute can be either `float`, `int`, or `bool`. (What a horrible abuse of types!)

.. code-block:: python

    retval = lp.integer()            # Try to solve this integer problem.
    assert retval == None            # Should not fail in this fashion.
    if lp.status != 'opt': return None

This is very similar to our invocation of the simplex solver, except this time we are using the integer solver. Again, we fail noisily if we encounter something unexpected, and quietly return `None` if we could not find a satisfying assignment.

.. code-block:: python

    return [col.value > 0.99 for col in lp.cols[::2]]

This function is supposed to return a satisfying truth assignment to all our variables if such an assignment is possible. Since we have gotten this far without returning None, we know we have one: a variable is true if its positive literal has a corresponding structural variable of 1.

Note that literal :math:`x_1` corresponds to column 0, :math:`x_2` to column 2, :math:`x_3` to column 4, and so forth. We go over each of the even columns (using the slice ::2 to indicate every column from beginning to end, counting by 2s), test whether the value of this columns variable is 1, and return the resulting list as our satisfying assignment.

We are done!

Example Run
===========

So, how does this work? Recall our CNF formula.

.. math::

    ( ¬x_1 ∨ ¬x_3 ∨ ¬x_4 ) ∧ ( x_2 ∨ x_3 ∨ ¬x_4 ) ∧ ( x_1 ∨ ¬x2 ∨ x_4 ) ∧ ( x_1 ∨ x_3 ∨ x_4 ) ∧ ( ¬x_1 ∨ x_2 ∨ ¬x_3 )

This has the encoding

.. code-block:: python

    [(-1, -3, -4), (2, 3, -4), (1, -2, 4), (1, 3, 4), (-1, 2, -3)]

Suppose we run this in our Python interpreter.

.. code-block:: python

    exp = [(-1, -3, -4), (2, 3, -4), (1, -2, 4), (1, 3, 4), (-1, 2, -3)]
    print solve_sat(exp)

This prints out:

.. code-block:: python

    [True, True, False, False]

So, :math:`x_1=T`, :math:`x_2=T`, :math:`x_3=F`, and :math:`x_4=F`. Is this a satisfying assignment? The first and second clauses are true because :math:`¬x_4`. The third and fourth clauses are true because :math:`x_1`. The fifth (last) clause is true because :math:`x_2`.

Now suppose we input the expression :math:`x_1 ∧ ¬x_1`, which is plainly unsatisfiable.

.. code-block:: python

    exp = [(-1,), (1,)]
    print solve_sat(exp)

This prints out:

.. code-block:: python

    None

Success! Or, at least, what we should expect.

Fun Variants
============

This problem is a little unusual in that we did not specify an objective function, leaving it to its default constant 0 value. We do not care which assignment we get. However, what if we wanted as many of our logical variables to be true as possible?

Suppose, right after the `lp.cols.add(2*numvars)` statement in our function, we added the following snippet.

.. code-block:: python

    lp.obj[::2] = 1
    lp.obj.maximize = True

We assign all even indexed objective coefficients (i.e., those corresponding to the positive literal structural variables) to 1, and say we want our LP to maximize this objective function. In other words, we want to maximize the sum of structural variables corresponding to positive assignments. If we repeat our run, we get

.. code-block:: python

    [True, True, True, False]

Different, but still a satisfying assignment! Now we have 3 logical variables true instead of 2. Suppose now we say we want to minimize this function, that is, we edit the snippet so now it reads

.. code-block:: python

    lp.obj[::2] = 1
    lp.obj.maximize = False

Repeating our run again, we get

.. code-block:: python

    [False, False, True, False]

Now only one logical variable is true!
