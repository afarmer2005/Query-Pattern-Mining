// @author: pangwei (pangweitf@163.com)
// Copyright (c) 2018 BUPT. All rights reserved.
// pangwei pangweitf@163.com
// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and         Telecomm., Beijing 100876, China

#include "ann.h"

ANN::ANN(int knn,int dim,int maxPts) {
    this->kdTree_ = NULL;
    this->knn_ = knn;
    this->dimension_ = dim;
    this->maxPts_ = maxPts;
    this->error_bound_ = 0.0;
}
ANN::~ANN() {
    if ( NULL != this->kdTree_ ) delete this->kdTree_;
    if ( NULL != this->nnIdx_ ) delete [] this->nnIdx_;
    if ( NULL != this->dists_ ) delete [] this->dists_;
    if ( NULL != this->queryPt_ ) delete this->queryPt_;
    this->nnIdx_ = NULL; this->dists_ = NULL; this->kdTree_ = NULL;
    annClose();// done with ANN
}
int ANN::init(const std::string &datafile) {
    
    this->queryPt_ = annAllocPt(this->dimension_);           // allocate query point
    this->dataPts_ = annAllocPts(this->maxPts_,this->dimension_);// allocate data points
    this->nnIdx_ = new ANNidx[this->knn_];             // allocate near neigh indices
    this->dists_ = new ANNdist[this->knn_];            // allocate near neighbor dists
    if( NULL == this->nnIdx_ || NULL == this->dists_ ) {
        std::cerr<<" ANN allocate memory failed "<<std::endl;
        return -1;
    }
    this->nPts_ = 0;                              // read data points
    if( load(datafile) ) {
        std::cerr<<"read data failed: "<<datafile<<std::endl;
        return -1;
    }
    this->kdTree_ = new ANNkd_tree(                    // build search structure
            this->dataPts_,                    // the data points
            this->nPts_,                       // number of points
            this->dimension_);                     // dimension of space
    if( NULL == this->kdTree_ ) {
        std::cerr<<"new kd_tree failed"<<std::endl;
        return -1;
    }
    return 0;
}
int ANN::knn( std::stringstream &ss,const std::string &term ) {
    __gnu_cxx::hash_map<std::string,ANNpoint>::iterator it = this->word2point_hash_.find(term);
    if( it == this->word2point_hash_.end() ) {
        std::cout<<"ANN Not Found : "<<term<<std::endl;
        return -1;
    }
    this->queryPt_ = it->second;
    this->kdTree_->annkSearch(this->queryPt_,this->knn_,this->nnIdx_,this->dists_,this->error_bound_);

    ss << "NN:         Word        Index     Distance(relative)  Cosine<br>";
    for (int i = 0; i < this->knn_; ++i) {
        __gnu_cxx::hash_map<ANNidx,std::string>::const_iterator it_term = idx2word_hash_.find(this->nnIdx_[i]);
        __gnu_cxx::hash_map<ANNidx,ANNpoint>::const_iterator it_idx2point = this->idx2point_hash_.find(this->nnIdx_[i]);
        double cosine_dist = cosine(this->queryPt_,it_idx2point->second);
        ss << " \t  " << i << "  \t  " <<it_term->second << "  \t   " << this->nnIdx_[i] << "  \t  " << this->dists_[i] <<" \t  "<<cosine_dist<< "<br>";
    }
    return 0;
}
int ANN::nearest( std::vector<std::string> &ss,const std::string &query,const int num ) {
    __gnu_cxx::hash_map<std::string,ANNpoint>::iterator it = this->word2point_hash_.find(query);
    if( it == this->word2point_hash_.end() ) {
        std::cout<<"ANN Not Found : "<<query<<std::endl;
        return -1;
    }
    this->queryPt_ = it->second;
    this->kdTree_->annkSearch(this->queryPt_,this->knn_,this->nnIdx_,this->dists_,this->error_bound_);
    int nn = this->knn_ > num ? num : this->knn_;
    for(int i = 0;i < nn;++i){
        __gnu_cxx::hash_map<ANNidx,std::string>::const_iterator it_term = idx2word_hash_.find(this->nnIdx_[i]);
        __gnu_cxx::hash_map<ANNidx,ANNpoint>::const_iterator it_idx2point = this->idx2point_hash_.find(this->nnIdx_[i]);
		ss.push_back(it_term->second);
    }
    return 0;
}

int ANN::load( const std::string &datafile ) {
    std::ifstream sidfile(datafile.c_str(), std::ios_base::in | std::ios_base::binary);
    if(sidfile.fail()){
        std::cerr<<"open "<<datafile<<" failed"<<std::endl;
        return -1;
    }
    std::string strLine;
    while( std::getline(sidfile, strLine, '\n') ){
        strtokenizer strtok(strLine, " \t\r\n");
        int dim_cnt = strtok.count_tokens();
        if( dim_cnt - 1 != this->dimension_ ){
            std::cerr<<strtok.token(0)<<" dim error:"<<strtok.count_tokens() - 1<<" != "<<this->dimension_<<std::endl;
            continue;
        }
        ANNpoint p = this->dataPts_[this->nPts_];
        char **endptr = NULL;
        for(int i = 1;i <= dim_cnt; ++i){
            *p++ = strtod(strtok.token(i).c_str(), endptr);
        }
        this->word2point_hash_.insert( std::pair<std::string,ANNpoint>(strtok.token(0),this->dataPts_[this->nPts_]));
        this->idx2word_hash_.insert( std::pair<ANNidx,std::string>(this->nPts_,strtok.token(0)));
        this->idx2point_hash_.insert( std::pair<ANNidx,ANNpoint>(this->nPts_,this->dataPts_[nPts_]) );
        ++this->nPts_;
    }
    sidfile.clear();sidfile.close();
    return 0;
}

bool ANN::readPt(std::istream &in, ANNpoint p) {
	for (int i = 0; i < this->dimension_; i++) {
		if(!(in >> p[i])) return false;
	}
	return true;
}

void ANN::printPt(std::ostream &out, ANNpoint p) {
	out << "(" << p[0];
	for (int i = 1; i < this->dimension_; i++) {
		out << ", " << p[i];
	}
	out << ")\n";
}
double ANN::cosine(ANNpoint a,ANNpoint b) {
    if( NULL == a || NULL == b ) return -0.1;
    double inner_product = 0.0;
    double a_root_sum = 0.0,b_root_sum = 0.0;
    for( int i = 0;i < this->dimension_; ++i ) {
        inner_product += a[i] * b[i];
        a_root_sum += a[i] * a[i];
        b_root_sum += b[i] * b[i];
    }
    a_root_sum = sqrt(a_root_sum);
    b_root_sum = sqrt(b_root_sum);
    return inner_product / (a_root_sum * b_root_sum);
}

