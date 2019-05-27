#! /usr/bin/env python

import random, time
import FLHashMap

def strTest():
	hashmap = FLHashMap.Map()
	print("Setting")
	for k in dir(str):
		hashmap.set(k, k)
	print("Getting")
	for k in dir(str):
		if hashmap.get(k) != k:
			print("Error with data: "+str(k))
	hashmap.printStats()

def randTest():
	random.seed(1993)
	N = 15000000
	hashmap = FLHashMap.Map()
	values = []
	print("Inserting")
	start = time.time()
	for i in range(0, N):
		value = i*10 + random.randrange(0, 10)
		values.append(value)
		hashmap.set(value, value)
	end = time.time()
	print("Time: %f"%(end-start))
	print("Searching")
	start = time.time()
	for i in range(0, N):
		if hashmap.get(values[i]) != values[i]:
			print("ERROR: could not find key "+str(values[i]))
	end = time.time()
	print("Time: %f"%(end-start))
	hashmap.printStats()

if __name__ == "__main__":
	#strTest()
	randTest()
