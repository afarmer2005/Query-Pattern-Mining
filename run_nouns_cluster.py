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

def nouns_cluster():
	nouns_dict = "query_nouns.dict"
	output_file = "query_nouns.cluster.new"
	vec = "../../word2vec/data/vectors_clickquery.bin"
	if os.path.exists(output_file):os.remove(output_file)
	cmd = "./ner_cluster -n 200 -x 934404 -k 200 -f %s -i %s -o %s" %(vec,nouns_dict,output_file)
	call_c(cmd)

def parse_args():
	parser = argparse.ArgumentParser(usage='')
	parser.add_argument('-i', '--input', dest='input',default='./data/',help='')
	parser.add_argument('-o', '--output',dest='output',default='./itemset/',help='')
	args = parser.parse_args()
	return args

if __name__ == "__main__":
	args = parse_args()
	nouns_cluster()

