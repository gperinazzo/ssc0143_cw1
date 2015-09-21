#!/bin/bash

# Run a series of tests to generate necessary data for analysis

# Clear directories
rm -rf output

sh ./script/run.sh matriz250
sh ./script/run.sh matriz500
sh ./script/run.sh matriz1000
sh ./script/run.sh matriz1500
sh ./script/run.sh matriz2000
sh ./script/run.sh matriz3000
sh ./script/run.sh matriz4000