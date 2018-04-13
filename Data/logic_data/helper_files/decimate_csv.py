#!/usr/bin/python

# Simple script for avg-ing CSV data automatically
# Just averages each column (except time, since you cant average time), for n rows 
# this makes it easy to draw figures / graphs of the data quickly and with smaller footprint
# Time[s],Raw_Facing_Outside,Raw_Facing_Inside,HiLo_Outside,LoHi_Outside,HiLo_Inside,LoHi_Inside

import sys

if len(sys.argv) < 3:
    print "too few arguments, enter filename and number of rows to average"
    sys.exit(0)
AVERAGE_AMOUNT = int(sys.argv[2])
SKIP_AMOUNT = 2
filename = sys.argv[1]
ndx = 0
skip = 0
total_ndx = 0
sums = []

outfile = open(filename+'_dec'+str(AVERAGE_AMOUNT)+'.csv', 'w')
with open(filename) as f:
    for line in f:
        # Skip header, write to file though
        skip = skip + 1
        if skip <= SKIP_AMOUNT:
            nums = [x for x in line.split(',')]
            col_length = len(nums)
            sums = [0] * col_length
            outfile.write(line);
            continue

        ### Average columns (not time) ###
        nums = [float(x) for x in line.split(',')]
        for z in xrange(1,len(nums)):
            sums[z] = sums[z] + nums[z]
        
        ndx = ndx + 1
        if ndx >= AVERAGE_AMOUNT:
            avg = [0] * col_length
            for z in xrange(1,len(nums)):
                avg[z] = sums[z] / AVERAGE_AMOUNT
            
            # Get most recent time for the averaged values and set it
            avg[0] = nums[0];

            outfile.write(', '.join([str(x) for x in avg]) + '\n')

            sums = [0] * col_length
            ndx = 0
        
        
