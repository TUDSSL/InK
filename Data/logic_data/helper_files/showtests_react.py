# This is a helper program that displays some useful info 
# for experiments we conducted with intermittent systems
# Data are parsed with DataParser class which parses csv files 
# produced by a logic analyzer software.
# 
# 
# This was developed for personal use during various stages of development and comes with no guarantee 
# of working.





import sys
import reactivity_stats as dm

import numpy as np
import matplotlib.pyplot as plt

# env_st_ch: High priority interrupt channel
env_st_ch 			= 7
# high_prio_tsk_ch: High priority task execution channel
high_prio_tsk_ch 	= 1
# env_evnt_ch: Low priority interrupt channel
env_evnt_ch 		= 6
# low_prio_tsk_ch: Low priority task execution channel
low_prio_tsk_ch 	= 5
# sens_st_ch: completion high priority task finish channel
sens_st_ch 			= 0
# sens_evnt_ch: completion low priority task finish channel
sens_evnt_ch		= 3
# time_limit: expiration time of event recognition
time_limit 			= 0.75
# sleep channel 
sleep_ch			= 4
# power channel 	
power_ch 			= 2
# runtime
runtime 			= 2 	# 1 ink
					    	# 2 alpaca
data = dm.DataParser("alpaca_run2_batch2.csv",
	env_st_ch + 1,
 	high_prio_tsk_ch + 1,
	env_evnt_ch + 1, 
	low_prio_tsk_ch + 1,
	sens_st_ch + 1,
	sens_evnt_ch + 1,
	time_limit,
	sleep_ch + 1,
	power_ch+1,
	runtime)

print("")
print("/=============STATS 4 NERDS============/")
print("")
print("Time limit for successful capture:%fs"%(time_limit))
print("")
print("Total events:%d"%(data.counted_events))
print("|_Successfully Captured:%d"%(data.counted_corr))
print("|__Falsely Captured:%d"%(data.counted_incorr))
print("|___Missed:%d"%(data.counted_events -(data.counted_corr + data.counted_incorr)))
print("")
print("Success Rate:	%f"%(float(data.counted_corr)/float(data.counted_events)))
print("")
print("False Rate: 	%f"%(float(data.counted_incorr)/float(data.counted_events)))
print("")
print("Miss Rate: 	%f"%(float(data.counted_events - (data.counted_corr + data.counted_incorr))/float(data.counted_events)))
print("")
print("/=============END====================/")

