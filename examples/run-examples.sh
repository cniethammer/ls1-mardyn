#!/bin/bash
# Testscript running all examples from example-list.txt
#
# Copyright (c) 2017      Christoph Niethammer <niethammer@hlrs.de>
#

MARDYN_EXE="mpirun -np 1 $PWD/../src/MarDyn"
MARDYN_OPTIONS="--steps 10 -v"
EXAMPLE_LIST_FILE=${EXAMPLE_LIST_FILE:=example-list.txt}
LOGFILE=${LOGFILE:=$PWD/run-examples.log}

#mapfile -t all_examples < $EXAMPLE_LIST_FILE # requires bash4 ...
declare -a all_examples
while IFS= read -r; do all_examples+=("$REPLY"); done < $EXAMPLE_LIST_FILE

failed_examples=()

# definitions for esear color output to display
Color_Off='\e[0m'       # Text Reset
IGreen='\e[0;92m'       # Intense Green
IRed='\e[0;91m'         # Intense Red
IMagenta='\e[0;95m'     # magenta

logfile=$LOGFILE
date > $logfile
for example in ${all_examples[@]}
do
  example_dir=$(dirname $example)
  example_inp_file=$(basename $example)
  echo -e -n "Running example ${IMagenta}$example_inp_file${Color_Off} in $example_dir ... "
  echo "Running example $example_inp_file in $example_dir" >> $logfile
  pushd "$example_dir" >/dev/null
  cmd="$MARDYN_EXE $MARDYN_OPTIONS $example_inp_file"
  echo $cmd >>$logfile
  { $cmd ; } >>$logfile 2>&1
  ret=$?
  if [ $ret -eq 0 ]; then
    echo -e "${IGreen}success${Color_Off}"
    echo "Running example $example_inp_file in $example_dir ... Result: success" >> $logfile
  else
    failed_examples=(${failed_examples[@]} $example)
    echo -e "${IRed}failed${Color_Off}"
    echo "Running example $example_inp_file in $example_dir ... Result: failed" >> $logfile
  fi
  popd >/dev/null
done

echo "----------------------------------------"
echo "Summary of tested examples:"
echo "----------------------------------------"
echo "num tested examples: ${#all_examples[@]}"
echo "num failed examples: ${#failed_examples[@]}"
echo "----------------------------------------"

date >> $logfile
exit ${#failed_examples[@]}

