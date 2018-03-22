// @author: pangwei (pangweitf@163.com)
// @time: Tue Jan 23 11:36:44 CST 2018

// Copyright (c) 2018 BUPT. All rights reserved.
// pangwei pangweitf@163.com
// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and         Telecomm., Beijing 100876, China



#ifndef __ANN_KNN_H__
#define __ANN_KNN_H__

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <cstdlib>						// C standard library
#include <cstdio>						// C I/O (for sscanf)
#include <cstring>						// string manipulation
#include <fstream>						// file I/O
#include <vector>
#include <sstream>
#include <ext/hash_map>

#include "ANN/ANN.h"					// ANN declarations
#include "strtokenizer.h"

#ifndef __GNU_CXX_HASH__
#define __GNU_CXX_HASH__

namespace __gnu_cxx {
     template<> struct hash<const std::string> {
         size_t operator()(const std::string& s) const {
            return hash<const char *>()(s.c_str());
         }
     };
     template<> struct hash<std::string> {
        size_t operator()(const std::string &s) const {
            return hash<const char *>()(s.c_str());
        }
     };
};
#endif //__GNU_CXX_HASH__

class ANN {
    public:
        ANN(int knn,int dim,int maxPts);
        ~ANN();

        int init( const std::string &datafile );
        int knn(std::stringstream &ss,const std::string &term);
        int nearest( std::vector<std::string> &ss,const std::string &query,const int num );
    private:
        int load(const std::string &datafile);
        bool readPt(std::istream &in, ANNpoint p);// read point (false on EOF)
        void printPt(std::ostream &out, ANNpoint p);// print point
        double cosine(ANNpoint a,ANNpoint b);//compute cosine distance of two points
    private:
        int knn_; //number of nearest neighbors
        int dimension_;//dimension per point
        int maxPts_;//maximum number of data points
        int nPts_;// actual number of data points
        double error_bound_;//error bound

        ANNpointArray dataPts_;// data points
        ANNpoint queryPt_;// query point
        ANNidxArray nnIdx_;// near neighbor indices
        ANNdistArray dists_;// near neighbor distances
        ANNkd_tree* kdTree_;// search structure

        __gnu_cxx::hash_map<std::string,ANNpoint> word2point_hash_;//term string project to ann point
        __gnu_cxx::hash_map<ANNidx,std::string> idx2word_hash_; //the idex of ann point project to term string
        __gnu_cxx::hash_map<ANNidx,ANNpoint> idx2point_hash_;
};
#endif //__ANN_KNN_H__

