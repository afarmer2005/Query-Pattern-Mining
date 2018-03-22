// @author: pangwei pangweitf@163.com
// Copyright (c) 2018 BUPT. All rights reserved.
// pangwei pangweitf@163.com
// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and         Telecomm., Beijing 100876, China
//
#include <getopt.h>
#include <libgen.h>
#include <pthread.h>
#include <sstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

#include "Trie.h"
#include "strtokenizer.h"

#include "ann.h"

//ANN Params
int g_ANN_knn = 20;
int g_ANN_dim = 200;
int g_ANN_maxPts = 0;
std::string g_ANN_data_file = "";

ANN *ann = NULL;

//input or output
int num_threads = 1;
std::string g_input_query_file = "";
std::string g_output_prefix = "";
std::string g_output_dir = "";
std::vector<std::string> train_data;

std::set<std::string> NounsPosDict; 

void parseArg(int argc,char *argv[]){
    const char *short_options = "s:d:x:t:n:k:f:i:o:h";
    struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    char chOpt;
    while( ( chOpt = getopt_long(argc, argv, short_options, long_options,NULL )) != -1 ){
        switch(chOpt){
            case 'i':g_input_query_file.assign(optarg);break;
            case 'o':g_output_prefix.assign(optarg);break;
			case 'd':g_output_dir.assign(optarg);break;
            case 'n':g_ANN_dim = atoi(optarg);break;
            case 'x':g_ANN_maxPts = atoi(optarg);break;
            case 'k':g_ANN_knn = atoi(optarg);break;
            case 'f':g_ANN_data_file.assign(optarg);break;
            case 'h':
                std::cout<<"Usage "<<basename(argv[0])<<" [options]"<<std::endl
                    <<"\t-i\tinput query file"<<std::endl
					<<"\t-d\toutput dir"<<std::endl
                    <<"\t-o\tprefix of output query file"<<std::endl
					<<"\t-n\tdimsnison of the vector file"<<std::endl
					<<"\t-x\tmax number points in  the vector file"<<std::endl
					<<"\t-k\tk nearest neighbour search"<<std::endl
					<<"\t-f\tdata file of the ANN vector file"<<std::endl
                    <<"\t-h\thelp info"<<std::endl;
                exit(0);
                break;
            default:
                std::cerr<<"Unknown Parameter:"<<optarg<<std::endl;
                break;
        }
    }
    if( g_output_dir.empty() || g_input_query_file.empty() || g_output_prefix.empty() ) {
        std::cerr<<"Confiure error: "<<std::endl;
        exit(-1);
    }
}

std::string to_string(unsigned int d) {
	std::ostringstream os;
	if (os << d) return os.str();
	return "failed";
}

int cmp(const std::pair<std::string, int> &x, const std::pair<std::string, int> &y) {
    return x.second > y.second;
}
int cmp_double(const std::pair<std::string,double> &x, const std::pair<std::string,double> &y) {
    return x.second > y.second;
}

void sortMapByValue(std::map<std::string, int>& tMap, std::vector<std::pair<std::string, int> >& tVector) {
	for (std::map<std::string, int>::iterator curr = tMap.begin(); curr != tMap.end(); curr++) {
		tVector.push_back(std::make_pair(curr->first, curr->second));
    }
    std::sort(tVector.begin(), tVector.end(), cmp);
}

void sortMapByDoubleValue(std::map<std::string, double>& tMap, std::vector<std::pair<std::string,double> >& tVector) {
    for (std::map<std::string, double>::iterator curr = tMap.begin(); curr != tMap.end(); curr++) {
        tVector.push_back(std::make_pair(curr->first, curr->second));
    }
    std::sort(tVector.begin(), tVector.end(), cmp_double);
}

int word_boundary(std::string &term,strtokenizer &strtok) {
	for(int i = 0;i < strtok.count_tokens();++i) {
		if( term == strtok.token(i) ) return 0;
	}
	return -1;
}

int cluster_pattern(Trie::Trie *loTries = NULL,const int line_no = 0) {
	if( NULL == loTries ) return -1;
	std::string output_file = g_output_dir + g_output_prefix+"_"+to_string(line_no)+".txt";
	std::ofstream output;
	output.open( output_file.c_str(), std::ofstream::trunc);
	if( output.fail() ) {
		std::cerr<<"open failed: "<<output_file<<std::endl;
		return -1;
	}
	std::ifstream sidfile(g_input_query_file.c_str(), std::ios_base::in | std::ios_base::binary);
	if(sidfile.fail()) {
		output.close();
		std::cerr<<"open "<<g_input_query_file<<" failed"<<std::endl;
		return -1;
	}
	long line_num = 0;
	std::string strLine;
	while( std::getline(sidfile, strLine, '\n') ) {
		strtokenizer strtok(strLine," ");
        if( strtok.count_tokens() < 2 ) continue;
        std::vector<Trie::KeyWordInfo> result;
        if( loTries->search(result,strLine.data(),DEF_TRIES_SEARCH_FMM) != 0 ) continue;
		if( result.size() < 1 ) continue;
        size_t reslen = result.size();
        std::string keyword = "";
        for( size_t j = 0;j < reslen;++j ) {
            if( word_boundary(result[j].keyword,strtok) ) continue;
            if( keyword != "" ) keyword.append(" ");
            keyword.append(result[j].keyword);
        }
        if( !keyword.empty() ) {
			output<<strLine<<std::endl;
		}
	}
	sidfile.close();
	output.close();
	
	return 0;
}

Trie::Trie * create_trie(std::vector<std::string> &tVec) {
	
	Trie::Trie *loTries = new Trie::Trie();
	if(NULL == loTries) {
		std::cerr<<"New Trie Object Failed."<<std::endl;
		return NULL;
	}
	if( loTries->init( tVec.size()+1 ) < 0 ){
		std::cerr<<"Init Trie Object Failed."<<std::endl;
		delete loTries;
		return NULL;
	}
	for(size_t i = 0;i < tVec.size();++i) {
		if( loTries->add( tVec[i].data(),0) < 0) {
			std::cerr<<"Tire insert failed:"<<tVec[i]<<std::endl;
		}
	}
	return loTries;
}

void *query_clustering(void *arg) {

	int thread_id = *(int *)arg;
	int begin = (train_data.size()*thread_id)/num_threads;
	int end   = (train_data.size()*(thread_id + 1))/num_threads;
	if( begin >= end ) pthread_exit(NULL);

	std::set<int> filter_set;
	for(int i = begin;i < end;++i) {

		if( filter_set.count(i) ) continue;

		strtokenizer strtok(train_data[i],"\t");
		strtokenizer wordtok(strtok.token(0)," ");
		strtokenizer postok(strtok.token(1)," ");

		std::set<std::string> nouns_set;
		for(int n = 0; n < postok.count_tokens();++n) {
			if( NounsPosDict.count(postok.token(n)) ) {
				nouns_set.insert(wordtok.token(n));
			}
		}
		std::vector<std::string> knn_vec;
		std::set<std::string>::iterator itt;
		for(itt = nouns_set.begin();itt != nouns_set.end();++itt) {
			ann->nearest(knn_vec,*itt,205);
		}
		if( knn_vec.empty() ) {
			filter_set.insert(i);
			continue;
		}
		std::string output_file = g_output_dir + g_output_prefix+"_"+to_string(i)+".txt";
		std::ofstream output;
		output.open( output_file.c_str(), std::ofstream::trunc);
		if( output.fail() ) {
			std::cerr<<"open failed: "<<output_file<<std::endl;
			pthread_exit(NULL);
		}
		filter_set.insert(i);
		output<<strtok.token(0)<<std::endl;

		Trie::Trie * loTries = create_trie(knn_vec);
		for(int j = i + 1;j < end;++j) {
			if(filter_set.count(j)) continue;
			std::vector<Trie::KeyWordInfo> result;
			strtokenizer substrtok(train_data[j],"\t");
			if( loTries->search(result,substrtok.token(0).data(),DEF_TRIES_SEARCH_FMM) != 0 || \
					result.size() < 1) {
				continue;
			}
			strtokenizer subwordtok(substrtok.token(0)," ");
			std::set<std::string> hitSet;
			for( size_t n = 0;n < result.size();++n ) {
				if( word_boundary(result[n].keyword,subwordtok) ) continue;
				hitSet.insert(result[n].keyword);
			}
			if( nouns_set.size() == hitSet.size() ) {
				output<<substrtok.token(0)<<std::endl;
				filter_set.insert(j);
			}
		}
		output.close();
		if( loTries ) delete loTries;
		loTries = NULL;
		if( i % 1000 == 0 ) std::cout<<"Processing "<<1.0*i/train_data.size()<<" .. "<<filter_set.size()<<" ..."<<std::endl;
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	parseArg(argc,argv);

    std::ifstream sidfile(g_input_query_file.c_str(), std::ios_base::in | std::ios_base::binary);
    if(sidfile.fail()) {
        std::cerr<<"open "<<g_input_query_file<<" failed"<<std::endl;
        return -1;
    }
	int line_no = 0;
    std::string strLine;
    while( std::getline(sidfile, strLine, '\n') ){
		train_data.push_back(strLine);
    }
    sidfile.close();

	ann = new ANN(g_ANN_knn,g_ANN_dim,g_ANN_maxPts);
	if( NULL == ann ) {
		std::cerr<<"new ANN Object failed"<<std::endl;
		return -1;
	}
	if( ann->init(g_ANN_data_file) ){
		std::cerr<<"init ANN Object failed: "<<g_ANN_data_file<<std::endl;
		return -1;
	}

	//'n','nd','nh','nl','ns','nt','nz','i','j'
	NounsPosDict.insert("n");NounsPosDict.insert("nt");NounsPosDict.insert("nz");
	NounsPosDict.insert("nd");NounsPosDict.insert("ns");NounsPosDict.insert("i");
	NounsPosDict.insert("nh");NounsPosDict.insert("nl");NounsPosDict.insert("j");

	std::vector<int> thread_ids;
	std::vector<pthread_t> pt;
	pt.resize(num_threads + 1);
	thread_ids.resize(num_threads + 1);
	for (int a = 0; a < num_threads; a++) {
		thread_ids[a] = a;
		pthread_create(&pt[a],NULL,query_clustering, (void *)&thread_ids[a]);
	}
	for (int a = 0; a < num_threads; a++) pthread_join(pt[a], NULL);

    return 0;
}

