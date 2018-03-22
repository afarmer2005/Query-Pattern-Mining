// @author: pangwei pangweitf@163.com
// Copyright (c) 2018 BUPT. All rights reserved.
// pangwei pangweitf@163.com
// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and         Telecomm., Beijing 100876, China

//
#include <getopt.h>
#include <libgen.h>
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

//input or output
std::string g_input_query_file = "";
std::string g_output_prefix = "";
std::string g_output_dir = "";
std::string g_nouns_file = "";

void parseArg(int argc,char *argv[]){
    const char *short_options = "s:d:x:n:k:f:i:o:h";
    struct option long_options[] = {
        {"help", 0, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    char chOpt;
    while( ( chOpt = getopt_long(argc, argv, short_options, long_options,NULL )) != -1 ){
        switch(chOpt){
            case 'i':g_input_query_file.assign(optarg);break;
            case 'o':g_output_prefix.assign(optarg);break;
            case 'f':g_nouns_file.assign(optarg);break;
			case 'd':g_output_dir.assign(optarg);break;
            case 'h':
                std::cout<<"Usage "<<basename(argv[0])<<" [options]"<<std::endl
                    <<"\t-f\tnouns cluster file"<<std::endl
                    <<"\t-i\tinput query file"<<std::endl
					<<"\t-d\toutput dir"<<std::endl
                    <<"\t-o\tprefix of output query file"<<std::endl
                    <<"\t-h\thelp info"<<std::endl;
                exit(0);
                break;
            default:
                std::cerr<<"Unknown Parameter:"<<optarg<<std::endl;
                break;
        }
    }
    if( g_output_dir.empty() || g_input_query_file.empty() || g_output_prefix.empty() || g_nouns_file.empty() ) {
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
	std::string strLine;
	while( std::getline(sidfile, strLine, '\n') ) {
		strtokenizer strtok(strLine,"\t");
		if( 2 != strtok.count_tokens() ) continue;
		strtokenizer subtok(strtok.token(1)," ");
		if( subtok.count_tokens() < 2 ) continue;

		std::vector<Trie::KeyWordInfo> result;
		if( loTries->search(result,strtok.token(1).data(),DEF_TRIES_SEARCH_FMM) != 0 ) continue;
		if( result.size() < 1 ) continue;
		size_t reslen = result.size();
		std::string keyword = "";
		for( size_t j = 0;j < reslen;++j ) {
			if( word_boundary(result[j].keyword,subtok) ) continue;
			if( keyword != "" ) keyword.append(" ");
			keyword.append(result[j].keyword);
		}
		if( !keyword.empty() ) output<<strtok.token(1)<<std::endl;
	}
	sidfile.close();
	output.close();

	return 0;
}

Trie::Trie * create_trie(strtokenizer &tVec) {
	
	Trie::Trie *loTries = new Trie::Trie();
	if(NULL == loTries) {
		std::cerr<<"New Trie Object Failed."<<std::endl;
		return NULL;
	}
	if( loTries->init( tVec.count_tokens()+1 ) < 0 ){
		std::cerr<<"Init Trie Object Failed."<<std::endl;
		delete loTries;
		return NULL;
	}
	for(int i = 0;i < tVec.count_tokens();++i) {
		if( loTries->add( tVec.token(i).data(),0) < 0) {
			std::cerr<<"Tire insert failed:"<<tVec.token(i)<<std::endl;
		}
	}
	return loTries;
}

int query_clustering() {

    std::ifstream sidfile(g_nouns_file.c_str(), std::ios_base::in | std::ios_base::binary);
    if(sidfile.fail()) {
        std::cerr<<"open "<<g_nouns_file<<" failed"<<std::endl;
        return -1;
    }
	int line_no = 0;
    std::string strLine;
    while( std::getline(sidfile, strLine, '\n') ){
		line_no++;
		strtokenizer subtok(strLine,"\t");
		Trie::Trie * loTries = create_trie(subtok);
		cluster_pattern(loTries,line_no);
		if( loTries ) delete loTries;
    }
    sidfile.close();

    return 0;
}

int main(int argc, char *argv[]) {
    parseArg(argc,argv);
    
    query_clustering();
	
    return 0;
}

