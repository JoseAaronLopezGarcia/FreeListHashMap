#! /usr/bin/env python

import random, time

def strTest():
	hashmap = {}
	print("Setting")
	for k in dir(str):
		hashmap[k] = k
	print("Getting")
	for k in dir(str):
		if hashmap[k] != k:
			print("Error with data: "+str(k))

def randTest():
	random.seed(1993)
	N = 15000000
	hashmap = {}
	values = []
	print("Inserting")
	start = time.time()
	for i in range(0, N):
		value = i*10 + random.randrange(0, 10)
		values.append(value)
		hashmap[value] = value
	end = time.time()
	print("Time: %f"%(end-start))
	print("Searching")
	start = time.time()
	for i in range(0, N):
		if hashmap[values[i]] != values[i]:
			print("ERROR: could not find key "+str(values[i]))
	end = time.time()
	print("Time: %f"%(end-start))

if __name__ == "__main__":
	#strTest()
	randTest()
