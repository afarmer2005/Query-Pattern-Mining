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

def prefixspam_tranf(input_file,indx2term):
	output_file = input_file + ".prespan"
	if os.path.exists(output_file):os.remove(output_file)
	output = codecs.open(output_file, 'w', 'utf-8')
	indx = 1
	term2index = {}
	for line in open(input_file):
		tokens = line.strip().split(' ')
		ans = ""
		for item in tokens:
			if ans != "":ans += " "
			if term2index.has_key(item):
				ans += str(term2index[item])
			else:
				term2index[item] = indx
				indx2term[indx] = item
				ans += str(indx)
				indx += 1
		if ans != "":output.write('%s\n' %(ans))
	output.flush()
	output.close()
def prefixspam_post(outspan_file,output_file,indx2term):
	if os.path.exists(output_file):os.remove(output_file)
	output = codecs.open(output_file, 'w', 'utf-8')
	line_no = True
	for line in open(outspan_file):
		if line_no:
			line_no = False
			continue
		tokens = line.strip().split(' ')
		freq = tokens[-1]
		ans = ""
		for i in xrange(len(tokens) - 1):
			term = indx2term.get(int(tokens[i]))
			if ans != "":ans += " "
			ans += term
		if ans != "":output.write('%s\t%s\n' %(ans,freq))
	output.flush()
	output.close()
	os.remove(outspan_file)

def process(input_file,output_file):
	if os.path.exists(output_file):os.remove(output_file)
	cmd = "./prefixspan -min_sup 100 %s > %s" %(input_file,output_file)
	call_c(cmd)

def parse_args():
	parser = argparse.ArgumentParser(usage='')
	parser.add_argument('-i', '--input', dest='input',\
			default='click_query_20170829_20170830.prefixspan',help='')
	parser.add_argument('-o', '--output',dest='output',\
			default='./click_query_20170829_20170830_seg_prefixspan.txt',help='')
	args = parser.parse_args()
	return args

if __name__ == "__main__":
	args = parse_args()
	process(args.input,args.output)

