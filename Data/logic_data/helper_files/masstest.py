# This is a helper program that displays some useful info 
# for experiments we conducted with intermittent systems

# Data are parsed with DataParser class which parses csv files 
# produced by a logic analyzer software.
# 
# The output is a csv friendly output for "big data collection"
# 
# 
# This was developed for personal use during various stages of development and comes with no guarantee 
# of working.




import sys
import reactivity_stats as dm

import numpy as np
import matplotlib.pyplot as plt

import time

start_time = time.time()



# Channels corresponding to physical channels on logic analyzer software

# # env_st_ch: High priority interrupt channel
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
time_limit 			= 1
# sleep channel/waiting/scheduling 
sleep_ch			= 4
# power channel 	
power_ch 			= 2
# runtime
# different runtimes have different gpio pin overhead 
# take that into account before adding more supported 
# runtimes. Currently:
# Ink = 1 
# Alpaca = 2
# Mayfly = 3


#relative paths / filenames / runtimes have to be specified manually 
header_str = "Itteration,File,Total events,Successfully Classified,Falsely Classified,Completely Missed,Success Rate, False Rate,Miss Rate,Total Time,On Time,High Priority Execution,Low Priority Execution,Wait Time,Overhead,Off Time,Death Time(overall),Death count(times of death),Max death duration,Min death duration,Max Power-On duration,Min Power-On duration,UF-hp on,UF-hp all,UF-lp on,UF-lp all,UF on,UF all";
for runtime in xrange(1,3):
		
	# print(header_str)
	for folders in xrange(1,2):
		# choose folder 
		if folders == 3:
			folder_str = "mayfly/"
			if runtime == 1:
				runtime_str = "mayfly"
			end_str = ".csv"		
		elif folders == 1:
			folder_str = "high_death_curve_300s/"
			if runtime == 2:
				runtime_str = "alpaca_run"
			else:
				runtime_str = "ink_run"
			end_str = ".csv"		
		#run for folder
		i = 1
		for i in xrange(1,6):
			if runtime == 2:
				in_str = (folder_str + runtime_str +"%d" + end_str)% (i)
			elif runtime == 1:
				in_str = (folder_str + runtime_str +"%d" + end_str)% (i)
			# print("Itteration:%d"%(i))
			data = dm.DataParser(in_str,
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
			row_print = "%d,"%(i)
			row_print += in_str + ","
			row_print += "%d,"%(data.counted_events)
			row_print += "%d,"%(data.counted_corr)
			row_print += "%d,"%(data.counted_incorr)
			row_print += "%d,"%(data.counted_events -(data.counted_corr + data.counted_incorr))
			row_print += "%6.3f,"%((float(data.counted_corr)/float(data.counted_events)))
			row_print += "%6.3f,"%((float(data.counted_incorr)/float(data.counted_events)))
			row_print += "%6.3f,"%((float(data.counted_events - (data.counted_corr + data.counted_incorr))/float(data.counted_events)))
			row_print += "%6.3f,"%(data.total_time)
			row_print += "%6.3f,"%(data.overall_on_time)
			row_print += "%6.3f,"%(data.hp_time)
			#print(data.hp_time)
			row_print += "%6.3f,"%(data.lp_time)
			row_print += "%6.3f,"%(data.sleep_time)
			if runtime == 2:
				row_print += "%6.3f,"%((data.overall_on_time - data.hp_time - data.lp_time - data.sleep_time))
			elif runtime == 1:
				row_print += "%6.3f,"%((data.overall_on_time - data.hp_time - data.lp_time - data.sleep_time))
			
			# print(data.total_time)
			row_print += "%6.3f,"%((data.total_time - data.overall_on_time))
			# print(data.overall_on_time)
			# print(data.total_time - data.overall_on_time)
			row_print += "%6.3f,"%(data.overall_off_time)
			row_print += "%6.3f,"%(data.number_of_deaths)
			row_print += "%6.3f,"%(data.biggest_death)
			row_print += "%6.3f,"%(data.smallest_death)
			row_print += "%6.3f,"%(data.max_power_on_time)
			row_print += "%6.3f,"%(data.min_power_on_time)
			row_print += "%6.3f,"%(float(data.hp_time / data.overall_on_time))
			row_print += "%6.3f,"%(float(data.hp_time / data.total_time))
			row_print += "%6.3f,"%(float(data.lp_time / data.overall_on_time))
			row_print += "%6.3f,"%(float(data.lp_time / data.total_time))
			row_print += "%6.3f,"%(float((data.hp_time + data.lp_time) / data.overall_on_time))
			row_print += "%6.3f"%(float((data.hp_time + data.lp_time) / data.total_time))

			print(row_print)


print("%s s to parse this file," % (time.time() - start_time))
print("overhead is wastefull computation + runtime overhead,")

