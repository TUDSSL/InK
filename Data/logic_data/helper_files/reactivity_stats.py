import csv

# DataParser is a class used by helper programs to parse csv files 
# produced by logic analyzer software.
# 
# This was developed for personal use during various stages of development and comes with no guarantee 
# of working.



class DataParser:
	"""This class parses a csv file to extract some useful information"""

	# avg execution time on high-prio
	hp_avg_time = 0 
	# avg execution time on low-prio
	lp_avg_time = 0 

	# power on time
	overall_on_time = 0.0

	# total execution time on high-prio
	hp_time = 0 
	
	# total execution time on low-prio
	lp_time = 0 
 	
 	#sleeping time
 	overall_sleep_time = 0.0

 	#overhead
 	overall_overhead = 0.0

	# power off time 
	overall_off_time = 0.0

	# number of power interrupts
	number_of_deaths = 0

	# avg reaction time on state changes 
	state_reaction_time = 0

	#avg reaction time on correctly recognized events
	event_reaction_time = 0
	
	# Total simulation time
	total_time = 0.0

	# last state change time
	sc_time = 0.0

	# state of the environment
	env_state_trace = 0

	# state of the environment event
	env_event_trace = 0

	# state of the sensor system
	sens_state_trace = 0

	# state of the events in the sensor system
	sens_event_trace = 0

	# correct transition of the sensor system
	__trans = 0

	# environment state (new == 1 , current == 0)
	__environment_state = 0

	# sensor state (new == 1, current == 0) 
	__sens_state = 0

	# classification counter state (correct == 1, false == 0)
	__counter_state = 0

	# events counted correctly
	counted_corr = 0

	# events counted incorrectly
	counted_incorr = 0
	
	# environment events counted 
	counted_events = 0

	# reaction time
	reaction_time = 0
	
	# env_st_ch: High priority interrupt channel  
	# high_prio_tsk_ch: High priority task execution channel
	# env_evnt_ch: Low priority interrupt channel
	# low_prio_tsk_ch: Low priority task execution channel
	# sens_st_ch: completion high priority task finish channel
	# sens_evnt_ch: completion low priority task finish channel
	# time_limit: expiration time of event recognition
	# sleep_ch: sleep monitor channel
	
	def __init__(self, file, env_st_ch, high_prio_tsk_ch, env_evnt_ch, low_prio_tsk_ch, sens_st_ch,sens_evnt_ch, time_limit,sleep_ch,power_ch,runtime):

		# total execution time on high-prio
		hp_time = 0 
		# avg execution time on high-prio
		hp_avg_time = 0 
		# total execution time on low-prio
		lp_time = 0 
		# avg execution time on low-prio
		lp_avg_time = 0 
		# sleep time
		sleep_time = 0
		avg_sleep_time = 0

		#avg reaction time on state changes 
		state_reaction_time = 0

		#avg reaction time on correctly recognized events
		event_reaction_time = 0
		
		# A GPIO set or clear is completed in 5 cproduced cycles for Coala (GCC),
		# and in 10 cproduced cycles for Alpaca (Clang)
		# Total simulation time
		total_time = 0.0
		# last state change time
		sc_time = 0.0
		# state of the environment
		env_state_trace = 0
		# state of the environment event
		env_event_trace = 0
		# state of the sensor system
		sens_state_trace = 0
		# state of the events in the sensor system
		sens_event_trace = 0
		# state of the high priority time counter
		high_prio_tsk_trace = 0
		# state of the low priority time counter
		low_prio_tsk_trace = 0
		# reaction time initialize
		event_reaction_time = 0.0
		state_reaction_time = 0.0
		# timer counter initialize
		hp_time = 	0.0
		hp_lt 	=	0.0
		lp_time = 	0.0
		lp_lt 	= 	0.0
		hp_time_cnt = 0
		lp_time_cnt = 0

		evnt_time 	= 0 
		state_time  = 0
		state_reaction_cnt = 0
		
		sleep_trace = 0
		sleep_lt = 0.0
		sleep_time_cnt = 0
		
		dead_trace = 0
		dead_lt = 0.0
		dead_time = 0.0
		dead_time_cnt =0
		
		power_trace = 0
		power_lt = 0.0
		power_time = 0.0
		power_time_cnt =0
		current_power_on_time = 0.0
		max_power_on_time = 0.0
		min_power_on_time = 100

		current_hp_time = 0.0
		min_hp_time = 100
		max_hp_time = 0.0
		current_lp_time = 0.0
		min_lp_time = 100
		max_lp_time = 0.0


		current_dead_time = 0.0
		min_dead_time = 100
		max_dead_time = 0.0

		# correct transition of the sensor system
		__trans = 0
		# environment state (new == 1 , current == 0)
		__environment_state = 0
		# sensor state (new == 1, current == 0) 
		__sens_state = 0
		# classification counter state (correct == 1, false == 0)
		__counter_state = 0
		# events counted correctly
		counted_corr = 0
		# events counted incorrectly
		counted_incorr = 0
		# environment events counted 
		counted_events = 0

		counted_state_ch = 0

		clk_freq = 1

		if runtime == 1:
			gpio_overhead = 5.0 / (clk_freq * 1000000)
		elif runtime == 2:
			gpio_overhead = 8.0 / (clk_freq * 1000000)
		
		last_timestamp = 0.0

		count_1 = 0;

		produced = 0

		wastefull = 0

		with open(file, 'r') as csvfile:
			reader = csv.reader(csvfile)
			for tokens in reader:
		    	# Skip header line and negative timestamps
				if not 'Time' in tokens[0] and float(tokens[0]) >= 0:
					# Compute total simulation time
					interval = float(tokens[0]) - last_timestamp
					last_timestamp = float(tokens[0])
					self.total_time += interval - gpio_overhead
					# print("added:%f"%(interval - gpio_overhead))
					# Environment State
					if int(tokens[env_st_ch]) == 1 and env_state_trace == 0:
						# low->high
						count_1 += 1
						__environment_state = 1
						sc_time = self.total_time
						state_time = last_timestamp
						__trans = 0
						env_state_trace = 1
						# print("state:%d"%(count_1))
					elif int(tokens[env_st_ch]) == 0 and env_state_trace == 1:
						# high->low
						env_state_trace = 0
					# count the events in the environment
					# Search for rising edge
					# print(tokens[env_evnt_ch],env_event_trace)
					if int(tokens[env_evnt_ch]) == 1 and env_event_trace == 0:
						# low->high
						counted_events += 1
						#("Event in environment:	%d in %f"%(counted_events,last_timestamp))
						env_event_trace = 1
						evnt_time = last_timestamp
						#producer consumer relationship on events
						produced = 1
					elif int(tokens[env_evnt_ch]) == 0 and env_event_trace == 1:
						# high->low
						env_event_trace = 0

					# keep track of the time of the environment state change
					if (self.total_time - sc_time > time_limit):
						__environment_state = 0					
					if int(tokens[sens_st_ch]) == 1 and sens_state_trace == 0:
						# low -> high
						__sens_state = 1
						sens_state_trace = 1

						#time to react to an environment change
						state_reaction_time += last_timestamp - state_time;
						state_reaction_cnt += 1
						#print(state_reaction_time)
					
					elif int(tokens[sens_st_ch]) == 0 and sens_state_trace == 1:
						# high -> low
						__sens_state = 0
						sens_state_trace = 0
					elif int(tokens[sens_st_ch]) == 0 and sens_state_trace == 1:
						#high -> high
						__sens_state = 0

					# classify the sensed data as valid or not
					if (__environment_state == 1) and __sens_state == 1:
						__counter_state = 1
						__trans = 1
						# print("trans:	%d"%(__trans))
						counted_state_ch += 1
					elif (__environment_state == 1) and __sens_state == 0:
						if  __trans == 1:
							__counter_state = 1
						else:
							__counter_state = 0
					elif (__environment_state == 0) and __sens_state == 0:
						if  __trans == 1:
							__counter_state = 1
						else:
							__counter_state = 0
					#print(__counter_state)
					if int(tokens[sens_evnt_ch]) == 1 and sens_event_trace == 0:
						# low -> high
						sens_event_trace = 1
						if (produced == 1) :
							if __counter_state == 1:
								counted_corr += 1
								# count the time until the event is successfully captured
								event_reaction_time += last_timestamp - evnt_time 
								# print("trans:	%d"%(__trans))
								# print("correct captured:	%d in %f"%(counted_corr,last_timestamp))
							else:
								counted_incorr += 1
								# print("trans:	%d"%(__trans))
								# print("in-correct captured:	%d in %f"%(counted_incorr,last_timestamp))
							produced = 0
						else:
							wastefull += 1
							#print("wasted:	%d"%(wastefull))
					elif int(tokens[sens_evnt_ch]) == 0 and sens_event_trace == 1:
						# low -> high
						sens_event_trace = 0
					
					# print("state:%d"%(__environment_state))
					#time spent on the high priority task execution 
					if high_prio_tsk_trace == 0:
						if int(tokens[high_prio_tsk_ch]) == 1: 
							#low ->high
							#keep track of all hp tasks execution time
							hp_lt = last_timestamp

							#keep track of task execution time
							current_hp_time = last_timestamp

							high_prio_tsk_trace = 1
					elif high_prio_tsk_trace == 1:
						if int(tokens[high_prio_tsk_ch]) == 1:
							#keep track of high priority task execution time
							current_hp_time += last_timestamp - hp_lt

							# keep track of high priority execution time
							hp_time += last_timestamp - hp_lt - gpio_overhead
							hp_lt = last_timestamp
							hp_time_cnt += 1
						elif int(tokens[high_prio_tsk_ch]) == 0:
							hp_time += last_timestamp - hp_lt - gpio_overhead
							high_prio_tsk_trace = 0   	
							
							current_hp_time += last_timestamp - hp_lt
							
							# keep track of the biggest high priority task execution
							if current_hp_time	>	max_hp_time:
								max_hp_time = current_hp_time
							# keep track of the smallest high priority execution
							if current_hp_time < min_hp_time:
								min_hp_time = current_hp_time
							
							current_hp_time = 0.0

					#time spent on the low priority task execution
					if low_prio_tsk_trace == 0:
						if int(tokens[low_prio_tsk_ch]) == 1: 
							#low ->low
							lp_lt = last_timestamp

							#keep track of task execution time
							current_lp_time = last_timestamp

							low_prio_tsk_trace = 1

					elif low_prio_tsk_trace == 1:
						if int(tokens[low_prio_tsk_ch]) == 1:
							#keep track of high priority task execution time
							current_lp_time += last_timestamp - lp_lt

							# keep track of high priority execution time
							lp_time += last_timestamp - lp_lt - gpio_overhead
							lp_time_cnt += 1
							lp_lt = last_timestamp
						elif int(tokens[low_prio_tsk_ch]) == 0:
							lp_time += last_timestamp - lp_lt - gpio_overhead
							low_prio_tsk_trace = 0
							lp_lt = last_timestamp
							
							current_lp_time += last_timestamp - lp_lt
							
							# keep track of the biggest high priority task execution
							if current_lp_time	>	max_lp_time:
								max_lp_time = current_lp_time
							# keep track of the smallest high priority execution
							if current_lp_time > 0 and current_lp_time < min_lp_time:
								min_lp_time = current_lp_time
							
							current_lp_time = 0.0
					#time spent sleeping
					if sleep_trace == 0:
						if int(tokens[sleep_ch]) == 1: 
							#low ->low
							sleep_lt = last_timestamp
							sleep_trace = 1
					elif sleep_trace == 1:
						if int(tokens[sleep_ch]) == 1:
							sleep_time += last_timestamp - sleep_lt - gpio_overhead
							sleep_time_cnt += 1
							sleep_lt = last_timestamp
						elif int(tokens[sleep_ch]) == 0:
							sleep_time += last_timestamp - sleep_lt - gpio_overhead
							sleep_trace = 0
							sleep_lt = last_timestamp
					#time spent dead
					if power_trace == 0:
						if int(tokens[power_ch]) == 1: 
							#low ->low
							power_lt = last_timestamp
							power_trace = 1
							dead_time += last_timestamp - dead_lt #- gpio_overhead
							current_dead_time += last_timestamp - dead_lt #- gpio_overhead
					 		dead_trace = 0
					 		start_counting_dead_time = 0
					 		# print("ended at:%f"%(last_timestamp))
					 		# current_power_on_time = last_timestamp
					 		# print("Started power on time on:%f"%last_timestamp)
					 		# keep track of the biggest high priority task execution
							if current_dead_time	>	max_dead_time:
								max_dead_time = current_dead_time
							# keep track of the smallest high priority execution
							if current_dead_time > 0 and current_dead_time < min_dead_time:
								min_dead_time = current_dead_time
							# print("death ended on:%f"%last_timestamp)
							# print("current_dead_time = %f - %f - %f = %f"%(last_timestamp,dead_lt,gpio_overhead,current_dead_time))
							# print(max_dead_time,min_dead_time)
							current_dead_time = 0.0
					elif power_trace == 1 :
						if int(tokens[power_ch]) == 1:
							power_time += last_timestamp - power_lt - gpio_overhead
							# print("added:%f in %f"%(last_timestamp - power_lt,last_timestamp))
							power_time_cnt += 1
							current_power_on_time += last_timestamp - power_lt - gpio_overhead
					 		# print("Added power on time of:%f - %f = %f"%(last_timestamp,power_lt,last_timestamp - power_lt - gpio_overhead))
							power_lt = last_timestamp

						elif int(tokens[power_ch]) == 0:
							power_time += last_timestamp - power_lt - gpio_overhead
							#print("added:%f in %f"%(last_timestamp - power_lt,last_timestamp))
							power_trace = 0
					 		current_power_on_time += last_timestamp - power_lt - gpio_overhead
							power_lt = last_timestamp
							#low ->low
							dead_lt = last_timestamp
							current_dead_time += last_timestamp - dead_lt #- gpio_overhead
					 		# print("Ended power on time of:%f - %f = %f"%(last_timestamp,power_lt,last_timestamp - power_lt - gpio_overhead))
							dead_time_cnt += 1
					 		dead_trace = 1
					 		# keep track of the biggest power on time
							if current_power_on_time	>	max_power_on_time:
								max_power_on_time = current_power_on_time
								# print("max:%f"%last_timestamp)
							# keep track of the smallest power on time
							if current_power_on_time > 0 and current_power_on_time < min_power_on_time:
								min_power_on_time = current_power_on_time
								# print("min:%f"%last_timestamp)
							current_power_on_time = 0.0
					# print("current:%f"%current_power_on_time)
					if dead_trace == 1 and start_counting_dead_time == 1:
						# print("death still in  %f"%last_timestamp)
					 	dead_time += last_timestamp - dead_lt #- gpio_overhead
						dead_lt = last_timestamp
						current_dead_time += last_timestamp - dead_lt #- gpio_overhead
					elif dead_trace == 1 and start_counting_dead_time == 0:
						# print("death in:%f"%last_timestamp)
						start_counting_dead_time = 1
		
		#overall time
		self.overall_on_time = power_time
		# print("total time:%f"%(self.total_time))		
		# print("on time:%f"%(power_time))
		# overall off time
		self.overall_off_time = dead_time
		# print("off time:%f"%(self.total_time-power_time))
		self.number_of_deaths = dead_time_cnt;
		self.biggest_death = max_dead_time;
		self.smallest_death = min_dead_time

		self.counted_events = counted_events
		self.counted_incorr = counted_incorr
		self.counted_corr = counted_corr
		# total execution time on high-prio
		self.hp_time = hp_time;
		# avg execution time on high-prio
		if hp_time_cnt == 0:
			hp_time_cnt = 1
		self.hp_avg_time = hp_time/hp_time_cnt
		# max hp task execution
		self.max_hp_time = max_hp_time
		# min hp task execution
		self.min_hp_time = min_hp_time
		# total execution time on high-prio
		self.lp_time = lp_time
		# max lp task execution
		self.max_lp_time = max_lp_time
		# min lp task execution
		self.min_lp_time = min_lp_time
		# avg execution time on high-prio
		if lp_time_cnt == 0:
			lp_time_cnt = 1
		self.lp_avg_time = lp_time/lp_time_cnt

		self.max_power_on_time = max_power_on_time
		#print(min_power_on_time)
		self.min_power_on_time = min_power_on_time
		#print(max_power_on_time)
		# total sleep time 
		self.sleep_time = sleep_time

		# avg sleepp time
		if sleep_time_cnt > 0:
			self.avg_sleep_time = sleep_time/sleep_time_cnt
		else:
			self.avg_sleep_time = 0
	
		#avg reaction time on state changes 
		if state_reaction_cnt > 0:
			self.state_reaction_time = state_reaction_time/state_reaction_cnt
		else:
			self.state_reaction_time = 0

		#avg reaction time on correctly recognized events
		if counted_corr > 0:
			self.event_reaction_time = event_reaction_time/counted_corr
		else:
			self.event_reaction_time = 0
		
		#self.total_time = total_time
		# sch_gpio_overhead = gpio_overhead * self.sch_cnt
		# mem_gpio_overhead = gpio_overhead * self.mem_cnt
		# self.sch_high_time -= sch_gpio_overhead
		# self.mem_high_time -= mem_gpio_overhead
		# self.total_high_time = self.sch_high_time + self.mem_high_time
		# self.total_low_time = self.total_time - self.total_high_time

		# # The scheduler channel is triggered twice per task: once for real
		# # scheduling, and once inside the coala_next_task() macro
		# if is_coala:
		# 	self.sch_cnt = self.sch_cnt / 2

		# if self.sch_cnt > 0:
		# 	self.sch_avg = int(1000000 * self.sch_high_time / self.sch_cnt)
		# if self.mem_cnt > 0:
		# 	self.mem_avg = int(1000000 * self.mem_high_time / self.mem_cnt)
		# if self.app_cnt > 0:
		# 	self.app_avg = int(1000 * self.app_time / self.app_cnt)

		# if self.total_time > 0:
		# 	self.sch_dc = 100 * self.sch_high_time / self.total_time
		# 	self.mem_dc = 100 * self.mem_high_time / self.total_time
		# 	self.usr_dc = 100 * self.total_low_time / self.total_time