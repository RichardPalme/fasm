## Frequent Approximate Subgraph Mining
Installation instructions for Linux systems (this repo can only be installed on Linux)

### Install Gurobi
First, you need to install Gurobi. To help you with the installation, we list the necessary steps here:

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
grbgetkey will ask where to store the license key. Make sure to store the license key in $HOME/gurobi950
7. In a terminal, navigate to $HOME/gurobi951/linux64/src/build and write the following command:  
`make`
8. Copy libgurobi_c++.a into the following folder: $HOME/gurobi951/linux64/lib

### Install Gedlib
Make sure that external/gedlib/src/CMakelists.txt does not add the
mtypesgedlib library
1. `cd external/gedlib`
2. `python3 install.py --gurobi ~/gurobi951/linux64 --lib mytypes,size_t,int,int`

### Install this Repo

`mkdir build && cd build && cmake .. && cmake --build . -j32`  

After executing this command, the executables can be found in the build directory
