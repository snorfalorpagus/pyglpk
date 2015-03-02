========================
Hamiltonian Path Example
========================

In this section we show a simple example of how to use PyGLPK to solve the `Hamiltonian path problem <http://en.wikipedia.org/wiki/Hamiltonian_path_problem>`_. This particular example is intended to be much more high level for those frustrated by lengthly explanations with excessive hand holding.

How to Solve
============

Suppose we have an undirected graph. We want to find a path from any node to any other node such that we visit each node exactly one. This is the Hamiltonian path problem!

This is our strategy of how to solve this with a mixed integer program:

* For each edge in the graph we define a structural variable (a column). This will be a binary variable with 1 indicating that this edge is part of the path.
* All nodes must have either 1 or 2 incident edges selected as part of the path, no more, no fewer. Therefore we introduce a row for each node to encode this constraint.
* Exactly as many edges should be selected as the number of nodes minus one. This constraint requires another node.

For the purpose of our function, we encode graphs as a list of two element tuples. Each tuple represents an edge between two vertices. Vertices can be anything Python can hash. We assume we can infer the vertex set from this edge set, i.e., there are no isolated vertices. (If there are, then the problem is trivial anyway.) For example, `[(1,2), (2,3), (3,4), (4,2), (3,5)]` would represent a graph over five nodes, containing a :math:`K_3` subgraph (among vertices 2,3,4) with additional vertices 1 attached to 2 and 5 attached to 3.

The function accepts such a list of edges, and return the sublist comprising the path, or `None` if it could not find a path.

The Implementation
==================

Here is the implementation of that function:

.. code-block:: python

    def hamiltonian(edges):
        node2colnums = {} # Maps node to col indices of incident edges.
        for colnum, edge in enumerate(edges):
            n1, n2 = edge
            node2colnums.setdefault(n1, []).append(colnum)
            node2colnums.setdefault(n2, []).append(colnum)

        lp = glpk.LPX()                     # A new empty linear program
        glpk.env.term_on = False            # Stop annoying messages.
        lp.cols.add(len(edges))             # A struct var for each edge
        lp.rows.add(len(node2colnums)+1)    # Constraint for each node

        for col in lp.cols:                 # Go over all struct variables
            col.kind = bool                 # Make binary, not continuous

        # For each node, select at least 1 and at most 2 incident edges.
        for row, edge_column_nums in zip(lp.rows, node2colnums.values()):
            row.matrix = [(cn, 1.0) for cn in edge_column_nums]
            row.bounds = 1, 2

        # We should select exactly (number of nodes - 1) edges total
        lp.rows[-1].matrix = [1.0]*len(lp.cols)
        lp.rows[-1].bounds = len(node2colnums)-1

        assert lp.simplex() == None         # Should not fail this way.
        if lp.status != 'opt': return None  # If no relaxed sol., no exact sol.

        assert lp.integer() == None         # Should not fail this way.
        if lp.status != 'opt': return None  # Count not find integer solution!

        # Return the edges whose associated struct var has value 1.
        return [edge for edge, col in zip(edges, lp.cols) if col.value > 0.99]

The Explanation
===============

We shall now go over this function section by section, but not quite in such exhaustive detail as before.

.. code-block:: python

    node2colnums = {} # Maps node to col indices of incident edges.
    for colnum, edge in enumerate(edges):
        n1, n2 = edge
        node2colnums.setdefault(n1, []).append(colnum)
        node2colnums.setdefault(n2, []).append(colnum)

This is pure Python non-PyGLPK code. It is simply computing a mapping of nodes to a list of column numbers corresponding to each node's incident edges. Note that each edge will have the same index in both the column collection, as well as the input edges list.

.. code-block:: python

    lp = glpk.LPX()                     # A new empty linear program
    glpk.env.term_on = False            # Stop annoying messages.
    lp.cols.add(len(edges))             # A struct var for each edge
    lp.rows.add(len(node2colnums)+1)    # Constraint for each node

In this section, we create a new empty linear program, make it quiet, and add as many columns as there are edges, and as many rows as there are vertices, plus one additional row to encode the constraint that exactly as many edges should be selected as there are vertices minus one.

.. code-block:: python

    for col in lp.cols:                 # Go over all struct variables
        col.kind = bool                 # Make binary, not continuous

Here, we set our LP as a MIP, and going over the columns set the associated structural variable to be binary (i.e., have 0 to 1 bounds and be an integer variables).

.. code-block:: python

    # For each node, select at least 1 and at most 2 incident edges.
    for row, edge_column_nums in zip(lp.rows, node2colnums.values()):
        row.matrix = [(cn, 1.0) for cn in edge_column_nums]
        row.bounds = 1, 2

These are the constraints that say for each node, either one or two edges should be selected. For each node, we set a row to have a constraint which sums over all of the structural variables representing edges incident to that node, and forces this sum to be between 1 and 2.

.. code-block:: python

    # We should select exactly (number of nodes - 1) edges total
    lp.rows[-1].matrix = [1.0]*len(lp.cols)
    lp.rows[-1].bounds = len(node2colnums)-1

Similarly, have the last row in the constraint matrix encode the constraint that the sum of all the structural variables (i.e., the number of edges selected) should be the number of vertices minus one.

Note how in this case we do not specify the column indices in our matrix assignment: we just assign a long list of 1.0 values, and use how matrix assignments will implicitly assume that each single value will be placed in the entry directly after the last assigned value.

.. code-block:: python

    assert lp.simplex() == None         # Should not fail this way.
    if lp.status != 'opt': return None  # If no relaxed sol., no exact sol.

    assert lp.integer() == None         # Should not fail this way.
    if lp.status != 'opt': return None  # Count not find integer solution!

As in the SAT example, we run the simplex solver to come up with an initial continuous relaxed basic solution to this problem. We fail, we miss the assertion, and if the optimal solution was not found, we return that there is no solution. We then run the integer optimizer.

.. code-block:: python

    # Return the edges whose associated struct var has value 1.
    return [edge for edge, col in zip(edges, lp.cols) if col.value > 0.99]

Finally, in this case, we select out those columns which have a value close to 1 (indicating this edge was selected) and return the associated edge using our `colnum2edge` map we constructed at the start of the function.

Example Run
===========

Suppose we have, after this function definition, these calls.

.. figure:: ../_static/ex_ham_fig1.png
   :scale: 30 %
   :alt: This graph has one Hamiltonian path!
   
   This graph has one Hamiltonian path!

.. code-block:: python

    g1 = [(1,2), (2,3), (3,4), (4,2), (3,5)]
    print hamiltonian(g1)

.. figure:: ../_static/ex_ham_fig2.png
   :scale: 30 %
   :alt: This graph has no Hamiltonian path!
   
   This graph has no Hamiltonian path!

.. code-block:: python

    g2 = [(1,2), (2,3), (1,4), (2,5), (3,6)]
    print hamiltonian(g2)

.. figure:: ../_static/ex_ham_fig3.png
   :scale: 30 %
   :alt: This graph has two Hamiltonian paths!
   
   This graph has two Hamiltonian paths!

.. code-block:: python

    g3 = g2 + [(5,6)]
    print hamiltonian(g3)

This will produce this output.

.. code-block:: python

    [(1, 2), (3, 4), (4, 2), (3, 5)]
    None
    [(1, 2), (1, 4), (2, 5), (3, 6), (5, 6)]

Fun TSP Variant
===============

Note that we did not define an objective function value. If we wanted, we could solve the `traveling salesman problem <http://en.wikipedia.org/wiki/Traveling_salesman_problem>`_ (with symmetric weights) by making the following modifications:

* Providing each edge with a cost of taking this edge. (Perhaps as a three element tuple instead of a two element tuple.) We could then set the associated edge's objective function value to this edge, and set this to a minimization problem.
* Given that the TSP computes cycles and not paths, change the 1 or 2 bounds to equality on 2 (each node has exactly 2 incident selected edges), and further refine the last constraint that as many edges as nodes must be selected.

Here is the resulting function.

.. code-block:: python

    def tsp(edges):
        node2colnums = {} # Maps node to col indices of incident edges.
        for colnum, edge in enumerate(edges):
            n1, n2, cost = edge
            node2colnums.setdefault(n1, []).append(colnum)
            node2colnums.setdefault(n2, []).append(colnum)

        lp = glpk.LPX()                     # A new empty linear program
        glpk.env.term_on = False            # Stop annoying messages.
        lp.cols.add(len(edges))             # A struct var for each edge
        lp.rows.add(len(node2colnums)+1)    # Constraint for each node

        lp.obj[:] = [e[-1] for e in edges]  # Try to minimize the total costs.
        lp.obj.maximize = False

        for col in lp.cols:                 # Go over all struct variables
            col.kind = bool                 # Make binary, not continuous

        # For each node, select two edges, i.e.., an arrival and a departure.
        for row, edge_column_nums in zip(lp.rows, node2colnums.values()):
            row.matrix = [(cn, 1.0) for cn in edge_column_nums]
            row.bounds = 2

        # We should select exactly (number of nodes) edges total
        lp.rows[-1].matrix = [1.0]*len(lp.cols)
        lp.rows[-1].bounds = len(node2colnums)

        assert lp.simplex() == None         # Should not fail this way.
        if lp.status != 'opt': return None  # If no relaxed sol., no exact sol.

        assert lp.integer() == None         # Should not fail this way.
        if lp.status != 'opt': return None  # Count not find integer solution!

        # Return the edges whose associated struct var has value 1.
        return [edge for edge, col in zip(edges, lp.cols) if col.value > 0.99]
