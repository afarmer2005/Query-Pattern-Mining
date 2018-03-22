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

def process(input_dir,output_dir):
	if not os.path.isdir(output_dir):os.mkdir(output_dir)
	pattern_dict = {}
	for d in listdir(input_dir):
		if len(d) < 2 and d[0] == ".":continue
		pattern_file = input_dir + d
		for line in open(pattern_file):
			tokens = line.strip().split('\t')
			if len(tokens) != 2:continue
			pattern = tokens[0]
			freq = int(tokens[1])
			pattern_dict.setdefault(pattern,0)
			pattern_dict[pattern] += freq
	qlist = sorted(pattern_dict.iteritems(),key=lambda d:d[1],reverse=True)                  
	output_file = output_dir + "pattern.txt"
	if os.path.exists(output_file):os.remove(output_file)
	output = codecs.open(output_file, 'w', 'utf-8')
	for item in qlist:output.write('%s\t%d\n' %(item[0],item[1]))
	output.flush()
	output.close()

def parse_args():
	parser = argparse.ArgumentParser(usage='')
	parser.add_argument('-i', '--input', dest='input',default='./pattern_wenda/',help='pattern')
	parser.add_argument('-o', '--output',dest='output',default='./result/',help='output')
	args = parser.parse_args()
	return args

if __name__ == "__main__":
	args = parse_args()
	process(args.input,args.output)

