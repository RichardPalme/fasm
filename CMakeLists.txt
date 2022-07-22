cmake_minimum_required(VERSION 3.10)

project(FASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_subdirectory(external/dlib-19.23/dlib dlib_build)

include_directories(
    "src"
    "external/gedlib"
    "external/gedlib/ext/boost.1.69.0"
    "external/gedlib/ext/eigen.3.3.4/Eigen"
    "external/gedlib/ext/nomad.3.8.1/src"
    "external/gedlib/ext/nomad.3.8.1/ext/sgtelib/src"
    "external/gedlib/ext/lsape.5/include"
    "external/gedlib/ext/lsape.5/cpp/include"
    "external/gedlib/ext/libsvm.3.22"
    "external/gedlib/ext/fann.2.2.0/include"
    "$ENV{GUROBI_HOME}/include"
    "external/nauty27r3"
)

link_directories(
    "external/gedlib/lib"
    "external/gedlib/ext/fann.2.2.0/lib"
    "external/gedlib/ext/nomad.3.8.1/lib"
    "external/gedlib/ext/libsvm.3.22"
    "$ENV{GUROBI_HOME}/lib"
)

link_libraries(
    mytypesgedlib
    doublefann
    svm
    nomad
    gurobi_c++
    gurobi95
    stdc++fs # for experimental/filesystem
    dlib::dlib
)


add_executable(
    unit_test
    executables/unit_test.cpp
    src/call_FSG.cpp
    src/gi_nauty.cpp
    src/gi_weisfeiler_leman.cpp
    src/graph.cpp
    src/pattern_growth.cpp
    src/read_FSG_file.cpp
    src/read_TUDataset.cpp
    src/sged_gedlib.cpp
    src/utils.cpp
    external/nauty27r3/nausparse.c
    external/nauty27r3/nauty.c
    external/nauty27r3/nautil.c
    external/nauty27r3/naugraph.c
    external/nauty27r3/schreier.c
    external/nauty27r3/naurng.c
)

add_executable(
    create_sged_matrix
    executables/create_sged_matrix.cpp
    src/call_FSG.cpp
    src/cross_validation.cpp
    src/gi_nauty.cpp
    src/gi_weisfeiler_leman.cpp
    src/graph.cpp
    src/pattern_growth.cpp
    src/read_FSG_file.cpp
    src/read_TUDataset.cpp
    src/sged_gedlib.cpp
    src/utils.cpp
    external/nauty27r3/nausparse.c
    external/nauty27r3/nauty.c
    external/nauty27r3/nautil.c
    external/nauty27r3/naugraph.c
    external/nauty27r3/schreier.c
    external/nauty27r3/naurng.c
)

add_executable(
    ecml
    executables/ecml.cpp
    src/call_FSG.cpp
    src/cross_validation.cpp
    src/gi_nauty.cpp
    src/gi_weisfeiler_leman.cpp
    src/graph.cpp
    src/pattern_growth.cpp
    src/read_FSG_file.cpp
    src/read_TUDataset.cpp
    src/sged_gedlib.cpp
    src/utils.cpp
    external/nauty27r3/nausparse.c
    external/nauty27r3/nauty.c
    external/nauty27r3/nautil.c
    external/nauty27r3/naugraph.c
    external/nauty27r3/schreier.c
    external/nauty27r3/naurng.c
)

add_executable(
    exact_fasm
    executables/exact_fasm.cpp
    src/call_FSG.cpp
    src/cross_validation.cpp
    src/gi_nauty.cpp
    src/gi_weisfeiler_leman.cpp
    src/graph.cpp
    src/pattern_growth.cpp
    src/read_TUDataset.cpp
    src/sged_gedlib.cpp
    src/utils.cpp
    external/nauty27r3/nausparse.c
    external/nauty27r3/nauty.c
    external/nauty27r3/nautil.c
    external/nauty27r3/naugraph.c
    external/nauty27r3/schreier.c
    external/nauty27r3/naurng.c
)

add_executable(
    test_patgrow
    executables/test_patgrow.cpp
    src/call_FSG.cpp
    src/gi_nauty.cpp
    src/gi_weisfeiler_leman.cpp
    src/graph.cpp
    src/pattern_growth.cpp
    src/read_TUDataset.cpp
    src/sged_gedlib.cpp
    src/utils.cpp
    external/nauty27r3/nausparse.c
    external/nauty27r3/nauty.c
    external/nauty27r3/nautil.c
    external/nauty27r3/naugraph.c
    external/nauty27r3/schreier.c
    external/nauty27r3/naurng.c
)

target_compile_options(test_patgrow PRIVATE -g)
#target_compile_options(unit_test PRIVATE -Wall -Wextra -Wpedantic)
#target_compile_options(exact_fasm PRIVATE -Wall -Wextra -Wpedantic)
