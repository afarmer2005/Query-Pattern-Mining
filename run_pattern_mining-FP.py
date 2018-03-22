#!/usr/bin/env python
# -*- coding:utf-8 -*-

"""
// Copyright (c) 2018 BUPT. All rights reserved.
// pangwei pangweitf@163.com
// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and         Telecomm., Beijing 100876, China
"""

import os
import sys
import re
import math
import argparse
import codecs
from os import listdir
reload(sys)
sys.setdefaultencoding('utf-8')

def call_c(command):
	status = os.system(command)
	if status == 0:
		print command + "\t" + "successed."
	else:
		print command + "\t" + "failed."

def process(input_dir,output_dir):
	if not os.path.isdir(output_dir):os.mkdir(output_dir)
	for d in listdir(input_dir):
		if len(d) < 2 and d[0] == ".":continue
		#3.0,2.0,1.0,0.8,0.6,0.3,0.1
		#for supp in support:
		input_file = input_dir + d
		output_file = output_dir + d # + "_item_"+str(supp)
		print "mining "+input_file+" ..."
		cmd = "./fpgrowth -ts -s50.0 -c80 -f' ' %s %s" %(input_file,output_file)
		call_c(cmd)
		if os.path.exists(output_file):
			lens = os.path.getsize(output_file)
			if lens < 10 or lens > 2054745:os.remove(output_file)

def parse_args():
	parser = argparse.ArgumentParser(usage='')
	parser.add_argument('-i', '--input', dest='input',default='./cluster_wenda/',help='')
	parser.add_argument('-o', '--output',dest='output',default='./itemset_wenda/',help='')
	args = parser.parse_args()
	return args

if __name__ == "__main__":
	args = parse_args()
	process(args.input,args.output)

