#!/usr/bin/python

import time
import datetime
import serial
import math as m
import csv
import sqlite3
conn = sqlite3.connect('/home/pi/Monitor/db.sqlite3')
c = conn.cursor()

start_time = 0
stop_time = 0
data_packet = []
data = 0
flag_data_packet_received = 0
temperature = 0.0
humidity = 0.0
co = 0.0
pm_2_5 = 0.0
node_id = 0
batt_voltage = 0.0
global ser

def writetotxt_file(writetotxt_filename,mode,string):
    '''
    # first para: filename
    2nd para: r-read mode,w-writemode,a-append(do not overwrite)
    
    f = open (filename,"r") or 
    with open(filename,"r") as f: -- better options as it closes the file without using f.close()
        
    f.name : give name of file
    f.mode : give mode in which file is opened
    
    '''

    # with open(writetotxt_filename,mode) as summaryfile:
        
    #     summaryfile.write(string)

    out = csv.writer(open(writetotxt_filename,"a"), delimiter=',',quoting=csv.QUOTE_ALL)
    out.writerow(string)

def send_wakeup_signal():
	ser.write(chr(49))
	print (chr(49))

def ppm(data):
	rl=2584
	ro=1677000
	volt = data*0.00073

	# print "volt", volt

	rs = (rl * (5-volt))/volt
	# print rs

	ppm = 27.42*m.pow(rs/ro,1.63)
	# print "ppm",ppm

	return ppm
##unsigned char to float##

def ug_m3(data):
	print "data", data
	concentration = (0.1 * data) - 0.1  #from matlab linear

	if concentration < 0:
		concentration = 0

	ugm3 = concentration*1000
	return ugm3

def tofloat(data):
	#print data
	#print (data)
	digit1=(data%100)%10
	digit2=(data/10)%10
	digit3=data/100
	# print (digit1,digit2,digit3)
	return (float ( digit3*10 + digit2 + digit1/10.0))

def data_conversion(data_packet):
	# temperature = 0, humidity = 0, co = 0, pm_2_5=0, node_id = 0
	data_log = []

	try:
		temperature = tofloat(ord(data_packet[2])*255 + ord(data_packet[3]))
		print ("temperature", temperature)
	except IndexError:
		temperature = 0
		print ("IndexError raised, temperature set to 0")

	try:
		humidity = tofloat(ord(data_packet[4])*255 + ord(data_packet[5]))
		print ("humidity", humidity)
	except IndexError:
		humidity = 0
		print ("IndexError raised, humidity set to 0")

	try:   
		# co = tofloat(ord(data_packet[6])*255 + ord(data_packet[7]))
		co_raw = ord(data_packet[6])*255 + ord(data_packet[7])
		co = round(ppm(co_raw), 3)
		print ("CO", co)
	except IndexError:
		co = 0
		print ("IndexError raised, co set to 0")

	try:
		pm_2_5_raw = (ord(data_packet[8])*255 + ord(data_packet[9]))/100
		pm_2_5 = round(ug_m3(pm_2_5_raw),3)
		print ("PM2.5", pm_2_5)
	except IndexError:
		pm_2_5 = 0
		print ("IndexError raised, PM2.5 set to 0")

	try:
		batt_voltage = round(3* 0.0007*(ord(data_packet[10])*255 + ord(data_packet[11])),2)
		print ("Battery Voltage", batt_voltage)
	except IndexError:
		batt_voltage = 0
		print ("IndexError raised, Battery Voltage set to 0")

	date, time, timestamp = get_date_time()
	# print "timestamp", timestamp
	node_id = ord(data_packet[1])
	# print "node_id", node_id

	data_log.append(temperature)
	data_log.append(humidity)
	data_log.append(co)
	data_log.append(pm_2_5)
	data_log.append(batt_voltage)
	data_log.append(date)
	data_log.append(time)
	data_log.append(timestamp)
	data_log.append(node_id)
        query1 = "INSERT INTO login_node_data (temperature, humidity, co, particulate_matter, node_id_id, created_date, created_time, timestamp) VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
	c.execute(query1, (temperature, humidity, co, pm_2_5, node_id, date, time,timestamp));
	conn.commit()
	
        query2 = "update login_live_data set temperature = ?, humidity = ?, co = ?, particulate_matter = ?,voltage=?, created_date=?, created_time=?, timestamp=?  where node_id_id = ?";
        c.execute(query2, (temperature, humidity, co, pm_2_5,batt_voltage, date, time,timestamp, node_id));
        conn.commit()
	print "data_log", data_log
	# return temperature, humidity, co, pm_2_5, batt_voltage, date, time, timestamp, node_id
	return data_log

def get_date_time():
	date = datetime.datetime.today().strftime('%Y-%m-%d')
	time = datetime.datetime.today().strftime('%H:%M')
	timestamp = datetime.datetime.today().strftime('%Y-%m-%d %H:%M')
	return date, time, timestamp

def main():
	start_time = 0
	stop_time = 0
	data_packet = []
	data = 0
	flag_data_packet_received = 0
	temperature = 0.0
	humidity = 0.0
	co = 0.0
	pm_2_5 = 0.0
	node_id = 0
	batt_voltage = 0.0
	#port='/dev/ttyAMA0',   # for raspberry pi + zigbee connection
	# port='/dev/ttyUSB0',    # for PC + zigbee connection
	global ser
	
	writetotxt_filename = str(datetime.datetime.today().strftime('%Y-%m-%d %H:%M')) + ".csv"

	# writetotxt_file(writetotxt_filename,'w',"start")

	ser = serial.Serial(port='/dev/ttyAMA0',
		baudrate=9600,
		parity=serial.PARITY_NONE,
		stopbits=serial.STOPBITS_ONE,
		bytesize=serial.EIGHTBITS,
		timeout=0
	)

	if ser.isOpen():
		print(ser.name + ' is open...')
		start_time = time.time()
		send_wakeup_signal()

	while 1:
		# start time
		# time.sleep(3)
		# send_wakeup_signal()

		# # Send wake up signal after every one minute - Read data every minute
		 
		while ((time.time()) - start_time <= 60):
			pass

		send_wakeup_signal()
		start_time = time.time()
		#print "start_time"
	  
		# while (flag_data_packet_received != 2 and ((time.time() - start_time) <= 60 )):
		#     # print "reading data"
		#     data = ser.read()

		#     if (data == '\r'):
		#         print "reading \r"
		#         flag_data_packet_received = 1
		#         data_packet.append(data)    
		#         #ser.write ('#')                 # acknowledgment
							
		#     elif ((data == '\n') and (flag_data_packet_received == 1)):
		#         flag_data_packet_received = 2
		#         data_packet.append(data)    
		#         ser.write ('#')                 # acknowledgment
		#         print " Done -------------------------"
		#         ##break
		#         #data_conversion(data_packet)

		#     elif (data != ""):
		#         # print "data", data
		#         if (data == '$'):
		#             data_packet.append(data)


		# print "hi", flag_data_packet_received
		# if (flag_data_packet_received == 2):
		#     flag_data_packet_received = 0
		#     data_conversion(data_packet)

			### To DO - check sum

	###############################################
		while (flag_data_packet_received != 2 and ((time.time() - start_time) <= 80 )):
			# print "reading data"
			data = ser.read()

			if (data == '$'):
				data_packet.append(data)

				while (flag_data_packet_received != 2):
					data=ser.read()

					if (flag_data_packet_received == 1 and data == '\n'):
						data_packet.append(data)
						# flag_data_packet_received = 2
						# ser.write('#')

						if ((len(data_packet) == 14) and (ord(data_packet[1]) == 1)):
							flag_data_packet_received = 2
							ser.write('#')
						elif ((len(data_packet) == 10) and (ord(data_packet[1]) == 2)):
							flag_data_packet_received = 2
							ser.write('#')
						else:
							flag_data_packet_received = 0

					elif (data == '\r'):
						data_packet.append(data)
						flag_data_packet_received = 1

					elif (data != ""):
						data_packet.append(data)
		
		#print "hi", flag_data_packet_received
		if (flag_data_packet_received == 2):
			flag_data_packet_received = 0
			data_log = data_conversion(data_packet)
			# for i in range (0, len(data_log)):
			# 	writetotxt_file(writetotxt_filename, 'a', data_log[i])
			# writetotxt_file(writetotxt_filename, 'a', '\n')
			writetotxt_file(writetotxt_filename,"wb", data_log)


	##########################################   
		# if (data == '$'):
		#     print "packet received ----------------------"
		#     data_packet.append(data)

		#     while (data != '\n'):
		#         data = ser.read()
		#         if (data != ""):
		#             data_packet.append(data)
		#             if (data == '\n'):
		#                 flag_end_of_packet = 1
		#                 data_packet.append(data)    
		#                 ser.write ('#')                 # acknowledgment
		#                 print " Done -------------------------"
		#                 data_conversion(data_packet)

		# for i in range(0, 11):
		#     x1 = ser.read()
		#     if x1 != "":
		#         data_packet.append(x1)


		if len (data_packet) > 1 :
			print (data_packet)

		# for i in range (0, len(data_packet)):
		#     print ord(data_packet[i])
			
		# print ord(x1), ord(x2)

		
		flag_data_packet_received = 0
		ser.flushInput()
		ser.flushOutput()
		
		data_packet = []
	ser.close()

if __name__ == '__main__':
	main()
