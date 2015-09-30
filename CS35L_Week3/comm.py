#!/usr/bin/python

#i run comm.py, pass in options first, then grab filename args

import random, sys, locale
from optparse import OptionParser

def main():
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]... FILE
Output randomly selected lines from FILE."""

parser = OptionParser()

#extra credit locale setting
locale.setlocale(locale.LC_ALL, '')

#single char options
parser.add_option('-1',action='store_true',dest='one',default=False,
help="suppress column 1 (lines unique to FILE1)")
parser.add_option('-2',action='store_true',dest='two',default=False,
help="suppress column 2 (lines unique to FILE2)")
parser.add_option('-3',action='store_true',dest='three',default=False,
help="suppress column 3 (lines that appear in both files)")
parser.add_option('-u',action='store_true',dest='unsorted',default=False,
help="prints out the columns regardless of if the files are sorted")
(options, args) = parser.parse_args()

#now options holds one, two, three, and unsorted boolean values
#args holds the non option inputs

#there must only be two arguments
if len(args) != 2:
	parser.error("Error: You must enter two arguments to compare")

firstfile=""
secondfile=""	
data1=""
data2=""

#first argument is stdin
#CTRL-D to stop reading in from stdin
#ONE BUG I HAVE IS THAT I DONT KNOW HOW TO PRESERVE THE ORDER
#THAT I GET THINGS FROM STDINPUT BECAUSE IM USING UNORDERED SETS
if args[0] == "-":
	secondfile = open(args[1],'r')
	data2 = set(secondfile.readlines())
	secondfile.close()
	data1 = set(sys.stdin.readlines())
#second argument is stdin
elif args[1] == "-":
	firstfile = open(args[0],'r')
	data1 = set(firstfile.readlines())
	firstfile.close()
	data2 = set(sys.stdin.readlines())
#both arguments are files
else:
	firstfile = open(args[0],'r')
	secondfile = open(args[1],'r')
	data1 = set(firstfile.readlines())
	data2 = set(secondfile.readlines())
	firstfile.close()
	secondfile.close()

#arrays to hold the lines to be displayed later
shared=[]
firstcol=[]
secondcol=[]
#getting unique to file1, file2, and shared
firstcol = data1.difference(data2)
secondcol = data2.difference(data1)
shared = data1.intersection(data2)
#loading what's to be printed
printing  = []

#setting up the tuples correctly
if (options.one & options.two & options.three):
	sys.stdout.write("") #print nothing
elif (options.one & options.two):
	for f in shared:
		printing.append((0,f))
elif (options.one & options.three):
	for f in secondcol:
		printing.append((0,f))
elif (options.two & options.three):
	for f in firstcol:
		printing.append((0,f))
elif (options.one):
	for f in secondcol:
		printing.append((0,f))
	for f in shared:
		printing.append((1,f))
elif (options.three):
	for f in firstcol:
		printing.append((0,f))
	for f in secondcol:
		printing.append((1,f))
elif (options.two):
	for f in firstcol:
		printing.append((0,f))
	for f in shared:
		printing.append((1,f))
else:
	for f in firstcol:
		printing.append((0,f))
	for f in secondcol:
		printing.append((1,f))
	for f in shared:
		printing.append((2,f))

	#print as is if -u option is on
if options.unsorted:
	for f in printing:
		sys.stdout.write('\t'*f[0])
		sys.stdout.write(f[1])
#print as a sorted thing	
else:		
	#sorting without case sensitive
	printing = sorted(printing, key=lambda x: x[1].lower(), reverse=False)
	#printing things out
	for f in printing:
		sys.stdout.write('\t'*f[0])
		sys.stdout.write(f[1])
		
		
if __name__ == "__main__":
    main()


