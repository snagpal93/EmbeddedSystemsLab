#!/bin/bash
FILES=images/*.jpg
if [ "$#" -ne 1 ]; then
    echo "usage run_all DMA[0|1]"
    exit
fi
if [ ! -d results ]; then
  mkdir -p results;
fi
for f in $FILES
do
  echo "Processing $f image"
  # take action on each file. $f store current file name. Generate Makefile, clean, run
  ./generateMake.py $f $1 "Makefile"
  make clean
  make run
  # Find the bare name of file $f. No extension/paths
  imgName=$(basename -- $f)
  imgName="${imgName%.jpg}"
  # Store results
  if [ ! -d results/$imgName ]; then
    mkdir -p results/$imgName;
  fi
  cp response.txt results/$imgName
  cp binary.pnm results/$imgName
  # Compare results with gold
  gold="${imgName}.pnm"
  OUTPUT="$(diff binary.pnm gold/$gold)"
  echo "${OUTPUT}" > results/$imgName/diff.txt
  echo "Finished processing $f ..."
done
echo "Finished all images"

