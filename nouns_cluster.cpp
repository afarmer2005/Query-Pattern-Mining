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

#include "ann.h"

//ANN Params
int g_ANN_knn = 20;
int g_ANN_dim = 200;
int g_ANN_maxPts = 0;
std::string g_ANN_data_file = ""; 

//input or output
std::string g_input_query_file = "";
std::string g_output_query_cluster_file = "";
std::string g_nouns_file = "";

std::vector<std::string> g_nouns_dict;

void parseArg(int argc,char *argv[]){
    const char *short_options = "s:d:x:n:k:f:i:o:h";
    struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    char chOpt;
    while( ( chOpt = getopt_long(argc, argv, short_options, long_options,NULL )) != -1 ){
        switch(chOpt){
            case 'i':
                g_input_query_file.assign(optarg);break;
            case 'o':
                g_output_query_cluster_file.assign(optarg);break;
            case 'n':g_ANN_dim = atoi(optarg);break;
            case 'x':g_ANN_maxPts = atoi(optarg);break;
            case 'k':g_ANN_knn = atoi(optarg);break;
            case 'f':g_ANN_data_file.assign(optarg);break;
            case 'h':
                std::cout<<"Usage "<<basename(argv[0])<<" [options]"<<std::endl
                    <<"\t-n\tdimsnison of the vector file"<<std::endl
                    <<"\t-x\tmax number points in  the vector file"<<std::endl
                    <<"\t-k\tk nearest neighbour search"<<std::endl
                    <<"\t-f\tdata file of the ANN vector file"<<std::endl
                    <<"\t-i\tinput query file"<<std::endl
                    <<"\t-o\toutput query cluster file"<<std::endl
                    <<"\t-h\thelp info"<<std::endl;
                exit(0);
                break;
            default:
                std::cerr<<"Unknown Parameter:"<<optarg<<std::endl;
                break;
        }
    }
    if( g_ANN_data_file.empty() || g_input_query_file.empty() || g_output_query_cluster_file.empty() ) {
        std::cerr<<"Confiure error: "<<std::endl;
        exit(-1);
    }
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


int read_nouns() {
	std::ifstream sidfile(g_input_query_file.c_str(), std::ios_base::in | std::ios_base::binary);
	if(sidfile.fail()) {
		std::cerr<<"open "<<g_input_query_file<<" failed"<<std::endl;
		return -1;
	}
	std::string strLine;
	while( std::getline(sidfile, strLine, '\n') ){
		g_nouns_dict.push_back(strLine);
	}
	sidfile.close();
	return 0;
}

int nouns_extract(ANN * ann) {

	std::ofstream output;
	output.open( g_output_query_cluster_file.c_str(), std::ofstream::trunc);
	if( output.fail() ) {
		std::cerr<<"open failed: "<<g_output_query_cluster_file<<std::endl;
		return -1;
	}
	int lines = 0;
	std::set<std::string> filter_set;
	std::string strLine;
    for(std::vector<std::string>::iterator it = g_nouns_dict.begin();\
			it != g_nouns_dict.end();++it) {
        ++lines;
        std::string ans;
		strLine.assign(*it);
		if( filter_set.count(strLine) ) continue;
        if( ann->nearest(ans,strLine,200) ) {
            std::cout<<"query_tda_search_failed:\t"<<strLine<<std::endl;
            continue;
        }
		std::map<std::string,int> cluster_map;
		cluster_map[strLine]++;
		strtokenizer wordtok(ans,"\t");
		for(int i = 0;i < wordtok.count_tokens();++i) {
			std::string res;
			//expand only once
			if( ann->nearest(res,wordtok.token(i),100) ) continue;
			cluster_map[wordtok.token(i)]++;
			
			strtokenizer subtok(res,"\t");
			for(int j = 0;j < subtok.count_tokens(); ++j) {
				cluster_map[subtok.token(j)]++;
			}
		}
		std::vector<std::pair<std::string, int> > tVec;
		sortMapByValue(cluster_map,tVec);
		std::string cluster;
		for(size_t i = 0;i < tVec.size();++i) {
			if( tVec[i].second < 2 ) break;
			if( !cluster.empty() ) cluster.append("\t");
			cluster.append(tVec[i].first);
			filter_set.insert(tVec[i].first);
		}
		if( !cluster.empty() ) output<<cluster<<std::endl;
		if( lines % 1000 == 0 )
			std::cout<<"Process "<<1.0*lines/g_nouns_dict.size()<<" ..."<<std::endl;
    }
    output.close();

    return 0;
}

int main(int argc, char *argv[]) {
    parseArg(argc,argv);
    ANN * ann = new ANN(g_ANN_knn,g_ANN_dim,g_ANN_maxPts);
    if( NULL == ann ) {
        std::cerr<<"new ANN Object failed"<<std::endl;
        return -1;
    }
    if( ann->init(g_ANN_data_file) ) {
        std::cerr<<"init ANN Object failed: "<<g_ANN_data_file<<std::endl;
        return -1;
    }
	read_nouns();    
	/*
	 *extracting Nouns and Nominal Phrases in the query
	 */
    nouns_extract(ann);
	
    return 0;
}

