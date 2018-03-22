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

def process(input_dir,itemset_dir,output_dir):
	if not os.path.isdir(output_dir):os.mkdir(output_dir)
	for d in listdir(itemset_dir):
		if len(d) < 2 and d[0] == ".":continue
		rawquery_file = input_dir + d
		itemset_file = itemset_dir + d
		output_file = output_dir + d
		#1102390887
		if os.path.getsize(itemset_file) <= 1102390887:
			transform(rawquery_file,itemset_file,output_file)

def parse_args():
	parser = argparse.ArgumentParser(usage='')
	parser.add_argument('-i', '--raw', dest='raw',default='./cluster_wenda/',help='query cluster')
	parser.add_argument('-f', '--itemset', dest='itemset',default='./itemset_wenda/',help='itemset')
	parser.add_argument('-o', '--output',dest='output',default='./pattern_wenda/',help='output')
	args = parser.parse_args()
	return args

if __name__ == "__main__":
	args = parse_args()
	process(args.raw,args.itemset,args.output)

