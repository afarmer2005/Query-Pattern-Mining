// @author: pangwei pangweitf@163.com
// Copyright (c) 2018 BUPT. All rights reserved.
// pangwei pangweitf@163.com
// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and         Telecomm., Beijing 100876, China
//
#include <getopt.h>
#include <sstream>
#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <map>
#include <vector>


std::string to_string(unsigned int d) {
	std::ostringstream os;
	if (os << d) return std::string("A")+os.str();
	return "failed";
}

int cmp(const std::pair<std::string, int> &x, const std::pair<std::string, int> &y) {
    return x.second > y.second;
}
void sortMapByValue(std::map<std::string, int>& tMap, std::vector<std::pair<std::string, int> >& tVector) {
    for (std::map<std::string, int>::iterator curr = tMap.begin(); curr != tMap.end(); curr++) {
        tVector.push_back(std::make_pair(curr->first, curr->second));
    }   
    std::sort(tVector.begin(), tVector.end(), cmp);
}


int main(int argc, char *argv[]) {

	std::map<std::string,int> tMap;
	for(unsigned int i = 1;i < 20;++i) {
		tMap[to_string(i)] = i;
	}

	std::vector<std::pair<std::string, int> > tVec;
	sortMapByValue(tMap,tVec);
	for(size_t i = 0;i < tVec.size();++i) {
		std::cout<<tVec[i].second<<"\t"<<tVec[i].first<<std::endl;
	}

    return 0;
}

