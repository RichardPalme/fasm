# Frequent Generalized Subgraph Mining
## About this repository
The code in this repository can be used to mine the frequent generalized subgraphs in a graph
database of undirected labeled graphs.

## Installation
Installation instructions for Linux systems. This repo has only been tested on Ubuntu Linux systems. To compile all requirements and the source, Python 3, Cmake, and Make are required.

### Install Gurobi
First, install Gurobi. To help with the installation, we list the necessary steps here:

1. Download Gurobi from https://www.gurobi.com/downloads/gurobi-software/
2. Extract the downloaded file, for example into your $HOME directory
3. Save the following lines into your .bashrc  
`export GUROBI_HOME="${HOME}/gurobi951/linux64"`  
`export PATH="${PATH}:${GUROBI_HOME}/bin"`  
`export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${GUROBI_HOME}/lib"`  
`export GRB_LICENSE_FILE="${HOME}/gurobi951/gurobi.lic"`  
4. Register at gurobi.com and get your free academic license
5. Log in and visit https://www.gurobi.com/downloads/licenses/
6. Copy-paste the grbgetkey command into a terminal and hit enter.  
grbgetkey will ask where to store the license key. Make sure to store the license key in `$HOME/gurobi951/gurobi.lic`
7. In a terminal, navigate to `$HOME/gurobi951/linux64/src/build` and write the following command:  
`make`
8. Copy `libgurobi_c++.a` into the following folder: `$HOME/gurobi951/linux64/lib`

### Install Gedlib
1. `cd external/gedlib`
2. `python3 install.py --gurobi ~/gurobi951/linux64 --lib mytypes,size_t,int,int`

Note: Make sure that external/gedlib/src/CMakelists.txt does not add the
mtypesgedlib library before the commands above have been executed.

### Install this Repo

`mkdir build && cd build && cmake .. && cmake --build . -j32`  

After executing this command, the executables can be found in the build directory

### Possible errors
If `cmake --build .` gives `fatal error: gurobi_c++.h: No such file or directory`,
then: `source ~/.bashrc && cmake .. && cmake --build .` may fix it.

## Using this repo
This repository can be used to mine the frequent generalized subgraphs in a graph
database of undirected labeled graphs.
The graphs in the database can have vertex and/or edge
labels. The mining process uses a taxonomy on the vertex labels.

The executable `generalized_FSM` is used to mine the freq. generalized subgraphs. Options for `generalized_FSM`:

`-dataset_name`: Name of a folder in the /data directory. Must contain the graph
database in TUDataset format. Must also contain a file edit_costs.txt
containing a comma-separated Boolean matrix C, with C[a,b] = 0 if and only if (a=b or b is
more general than a in the vertex label taxonomy). The rows and columns of C
must correspond to the vertex labels.

`-max_size`: maximum number of edges of a graph in the output.

`-exact_gi`: 1 if graph isomorphism shall be computed with an exact algorithm (using
Nauty), 0 if graph isomorphism shall be computed with a heuristic algorithm
(Weisfeiler-Leman).

`-ged_method`: 0 if graph edit distance shall be computed with an exact algorithm
(using Gurobi), 1-5 if graph edit distance shall be computed with a heuristic
algorithm (see also: GEDMethod defined in src/sged_gedlib.h)

`-num_new_labels`: Number of vertex labels that do appear in the taxonomy, but not
in the graph database.

`-t`: relative frequency threshold for the graph mining. Must be an integer
between 1 and 100.

### Example
    generalized_FSM -dataset_name MUTAG -max_size 12 -exact_gi 1 -ged_method 0 -num_new_labels 3 -t 42

