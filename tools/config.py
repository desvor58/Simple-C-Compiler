BUILD_COMPILER       = "gcc"  # compiler for compiling scc
BUILD_COMPILER_FLAGS = "-Iinclude -std=c99"  # options for compiler, witch compile scc
BUILD_DST            = "./bin/"  # where to build scc

TESTS_DIR        = "./tests"  # tests directory
TEST_ALL_MODULES = [
    "common",
    "preproc"
]