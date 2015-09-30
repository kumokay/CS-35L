#!/bin/python

import sys

f1 = set(open("testfile1.txt").readlines())
f2 = set(open("testfile2.txt").readlines())

onlyf1 = f1.difference(f2)
onlyf2 = f2.difference(f1)

bothf = f1.intersection(f2)

linelist = []

for f in onlyf1:
	linelist.append((0, f))
for f in onlyf2:
	linelist.append((1, f))
for f in bothf:
	linelist.append((2, f))

for lt in linelist:
	sys.stdout.write('\t'*lt[0])
	sys.stdout.write(lt[1])
	sys.stdout.write('\n')


