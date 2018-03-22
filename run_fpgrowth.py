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

def process(input_file,output_file):
	supp_resu = {}
	#support = [20.0,15.0,10.0,5.0,3.0,2.0,1.0,0.8,0.6,0.3,0.1]
	support = [1.0]
	for supp in support:
		output_file = "./fpwenda_"+str(supp)+".txt"
		if os.path.exists(output_file):os.remove(output_file)
		cmd = "./fpgrowth -ts -s%f -c80 -f' ' %s %s" %(supp,input_file,output_file)
		print cmd
		call_c(cmd)
		"""
		if os.path.exists(output_file):
			totaline = 0
			for line in open(output_file):totaline += 1
			supp_resu[supp] = totaline
			os.remove(output_file)
		"""
	"""
	for key in supp_resu:
		print str(key)+"\t"+str(supp_resu[key])
	"""

def transform(rawquery_file,itemset_file,pattern_file):
	itemset_list = []
	for line in open(itemset_file):
		tokens = line.strip().split(' ')
		itemset = set(tokens[0:-1])
		itemset_list.append(itemset)
	pattern_freq_dict = {}
	for line in open(rawquery_file):
		tokens = line.strip().split(' ')
		raw_set = set(tokens)
		for itemset in itemset_list:
			if len(raw_set.intersection(itemset)) < 1:continue
			diff_set = raw_set.difference(itemset)
			ans = ""
			for token in tokens:
				if ans != "" and token in diff_set and ans[-1] == "#": continue
				if ans != "":ans += " "
				if token in diff_set:ans += "#"
				else:ans += token
			if ans == "":continue
			pattern_freq_dict.setdefault(ans,0)
			pattern_freq_dict[ans] += 1
	if len(pattern_freq_dict) < 1:return
	qlist = sorted(pattern_freq_dict.iteritems(),key=lambda d:d[1],reverse=True)
	if os.path.exists(pattern_file):os.remove(pattern_file)
	output = codecs.open(pattern_file, 'w', 'utf-8')
	for item in qlist:output.write('%s\t%d\n' %(item[0],item[1]))
	output.flush()
	output.close()

def transfer(rawquery_file):
	input_file = "./data/fpwenda_1.0.txt"
	output_file = "./data/fpwenda_1.0_pattern.txt"
	#input_file = "fpresult_0.1.txt"
	#output_file = "fpresult_0.1_pattern.txt"
	if os.path.exists(output_file):os.remove(output_file)
	transform(rawquery_file,input_file,output_file)

def parse_args():
	parser = argparse.ArgumentParser(usage='')
	parser.add_argument('-i', '--input', dest='input',\
			default='./data/question_experiment_seg.dat',help='')
	parser.add_argument('-o', '--output',dest='output',\
			default='query_experiment_frequent_itemset.txt',help='')
	args = parser.parse_args()
	return args

if __name__ == "__main__":
	args = parse_args()
	#process(args.input,args.output)
	transfer(args.input)

