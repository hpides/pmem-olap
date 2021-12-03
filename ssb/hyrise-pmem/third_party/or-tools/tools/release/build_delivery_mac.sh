#!/usr/bin/env bash
set -x
set -e

if [ ! -f "$DOTNET_SNK" ]; then
  echo "DOTNET_SNK: not found !" | tee build.log
  exit 1
fi

# Clean everything
make clean
make clean_third_party

# Print version
make print-OR_TOOLS_VERSION | tee build.log

# Check all prerequisite
# cc
command -v clang
command -v clang | xargs echo "clang: " | tee -a build.log
command -v cmake
command -v cmake | xargs echo "cmake: " | tee -a build.log
command -v make
command -v make | xargs echo "make: " | tee -a build.log

command -v swig
command -v swig | xargs echo "swig: " | tee -a build.log

# python
PY=(3.6 3.7 3.8)
for i in "${PY[@]}"; do
  command -v "python$i"
  command -v "python$i" | xargs echo "python$i: " | tee -a build.log
  "python$i" -c "import distutils.util as u; print(u.get_platform())" | tee -a build.log
  "python$i" -m pip install --user wheel six virtualenv
done

# java
echo "JAVA_HOME: ${JAVA_HOME}" | tee -a build.log
command -v java
command -v java | xargs echo "java: " | tee -a build.log
command -v javac
command -v javac | xargs echo "javac: " | tee -a build.log
command -v jar
command -v jar | xargs echo "jar: " | tee -a build.log
java -version 2>&1 | head -n 1 | grep 1.8

# C#
command -v dotnet
command -v dotnet | xargs echo "dotnet: " | tee -a build.log

#########################
##  Build Third Party  ##
#########################
echo -n "Build Third Party..." | tee -a build.log
make third_party UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log

#####################
##  C++/Java/.Net  ##
#####################
echo -n "Build C++..." | tee -a build.log
make cc -l 4 UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log
#make test_cc -l 4 UNIX_PYTHON_VER=3.7
#echo "make test_cc: DONE" | tee -a build.log

echo -n "Build flatzinc..." | tee -a build.log
make fz -l 4 UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log

echo -n "Build Java..." | tee -a build.log
make java -l 4 UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log
#make test_java -l 4 UNIX_PYTHON_VER=3.7
#echo "make test_java: DONE" | tee -a build.log

echo -n "Build .Net..." | tee -a build.log
make dotnet -l 4 UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log
#make test_dotnet -l 4 UNIX_PYTHON_VER=3.7
#echo "make test_dotnet: DONE" | tee -a build.log

# Create Archive
rm -rf temp ./*.tar.gz
echo -n "Make archive..." | tee -a build.log
make archive UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log
echo -n "Test archive..." | tee -a build.log
make test_archive UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log

echo -n "Make flatzinc archive..." | tee -a build.log
make fz_archive UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log
echo -n "Test flatzinc archive..." | tee -a build.log
make test_fz_archive UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log

echo -n "Make Python examples archive..." | tee -a build.log
make python_examples_archive UNIX_PYTHON_VER=3.7
echo "DONE" | tee -a build.log

##################
##  Python 3.X  ##
##################
for i in "${PY[@]}"; do
  echo -n "Cleaning Python..." | tee -a build.log
  make clean_python UNIX_PYTHON_VER="$i"
  echo "DONE" | tee -a build.log

  echo -n "Build Python $i..." | tee -a build.log
  make python -l 4 UNIX_PYTHON_VER="$i"
  echo "DONE" | tee -a build.log
  #make test_python UNIX_PYTHON_VER=$i
  #echo "make test_python$i: DONE" | tee -a build.log
  echo -n "Build Python $i wheel archive..." | tee -a build.log
  make package_python UNIX_PYTHON_VER="$i"
  echo "DONE" | tee -a build.log
  echo -n "Test Python $i wheel archive..." | tee -a build.log
  make test_package_python UNIX_PYTHON_VER="$i"
  echo "DONE" | tee -a build.log

  cp "temp_python$i"/ortools/dist/*.whl .
done
