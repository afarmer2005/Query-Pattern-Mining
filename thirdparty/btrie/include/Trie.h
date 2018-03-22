// Copyright (c) 2018 BUPT. All rights reserved.
// pangwei pangweitf@163.com
// Beijing Key Lab of Intelligent Telecomm. Software and Multimedia,Beijing Univ. of Posts and Telecomm., Beijing 100876, China
/*
 * Tire.h
 */

#ifndef __TRIESDICT_H__
#define __TRIESDICT_H__

#include <inttypes.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>


namespace Trie
{

/////////////////////////////////////区域 开放给用户的定义///////////////////////////////////////////////////

#define DEF_TRIES_SEARCH_ALL 0X00 //全局匹配
#define DEF_TRIES_SEARCH_FMM 0X01 //正向最大匹配

////使用 protobuf 更好，兼容性好
typedef struct  _KeyWordInfo
{
	std::string keyword;
	unsigned int keyword_serial;
	uint64_t keyword_signature;
	unsigned int offset;
	void *keyword_feature;//optional data
	_KeyWordInfo():keyword_serial(0),keyword_signature(0),offset(0),keyword_feature(NULL)
	{
	}
	//get attribute
	const std::string & get_keyword()
	{
	    return this->keyword;
	}
	const unsigned int get_keyword_signature()
	{
	    return this->keyword_signature;
	}
	const unsigned int get_offset()
	{
	    return this->offset;
	}
	const unsigned int get_keyword_serial()
	{
		return this->keyword_serial;
	}
	const void *get_keyword_feature()
	{
	    return this->keyword_feature;
	}
	//set attribute
	int set_keyword(const char *keyword,const unsigned int keywordlen)
	{
		if( NULL == keyword || keywordlen <= 0 )
		{
			return -1;
		}
		this->keyword.assign(keyword,keywordlen);
		return 0;
	}
	void set_keyword_serial(const unsigned int serial)
	{
		this->keyword_serial = serial;
	}
	void set_keyword_signature(const uint64_t sig)
	{
		this->keyword_signature = sig;
	}
	void set_offset(unsigned int pos)
	{
		this->offset = pos;
	}
	int set_keyword_feature(void *p)
	{
		if(NULL == p)
		{
			return -1;
		}

		this->keyword_feature = p;
		return 0;
	}
}KeyWordInfo;
/////////////////////////////////////区域 开放给用户的定义 结束/////////////////////////////////////////////////


namespace //anonymous namespace
{
//本匿名空间内的常量、结构体定义 仅供Trie内部使用，对外透明

#define DEF_TRIES_NULL		0XFFFFFFFF
#define DEF_TRIES_FIRST		0XFFFFFFFE
#define DEF_TRIES_SUFFNULL	0XFFFFFFFF
#define DEF_TRIES_NODENULL	0XFFFFFFFF
#define DEF_TRIES_COMMNULL	0XFFFFFFFF

const unsigned int DictNodeCountDefault = 8192;//默认值，内部使用，对外透明
}//end of anonymous namespace


class Trie 
{
public:
	Trie();
	~Trie();

	//初始化
	//输入 keyword_count，keyword的数量（估计值，Trie内部会自动扩充内存空间）
	int init(unsigned int keyword_count_estimated = DictNodeCountDefault);
	
	//加载trie词典--二进制文件
	int load(const char *dict_file);
	//保存trie词典--二进制文件
	int save(const char *dict_file, const char *magic_string = NULL);
	
	//添加一个keyword
	int add(const char *keyword, unsigned int keyword_serial = 0,uint64_t keyword_signature = 0,void *keyword_feature = NULL);

	//匹配过程
	int search(std::vector<KeyWordInfo> &result,const char *query,int search_mode = DEF_TRIES_SEARCH_FMM);
    
//Trie内部使用的数据结构、成员方法
 private:
	struct TRIES_DICT_NODE
	{
		char *m_pString;//keywords
		unsigned int m_uiLen;//keywords length
		unsigned int m_uiSerial;//keywords serial number
		uint64_t m_uiAttr;//keywords attribute ,term signature
		unsigned m_uiBTPos;
		void* value;//optional data
		
		TRIES_DICT_NODE():m_pString(NULL),value(NULL)
		{
			m_uiLen = 0;
			m_uiAttr = 0;
			m_uiSerial = 0;
			m_uiBTPos = 0;
		}
	};

	struct TRIES_DICT_ENTRY
	{
		unsigned int m_uiValue;
		unsigned int m_uiTPos;
		unsigned int m_uiSPos;
	};

	struct DICT_HEAD_DEF
	{
		unsigned char m_byType;
		unsigned char m_byVersion;
		char m_szComment[16];
	};

	struct TRIES_INN_NODE
	{
		unsigned int m_uiLen;
		unsigned int m_uiSerial;
		uint64_t m_uiAttr;
		void *value;
		unsigned int m_uiBTPos;
	};

 	//稍后调整 函数名字命名方法,2013-11-20
	void destroy();
	void clean();

	unsigned int SeekString(char *apTerm, unsigned int aiLen);

	int ModifyEntry(unsigned int luiTPos, unsigned int auiAttr, void *value);
	int GetEntryAttr(unsigned int luiTPos, unsigned int& auiAttr, void *&value);

	int InsertDicEntry(unsigned int auiLastDEPos, unsigned int auiStrCode,
                       unsigned int &auiCurDEPos);
	unsigned int GetGB18030StrCode(const char *apString, const int aiLen, int *aiPos,
                            const char *apChrSet = NULL);

	//仅支持UTF-8编码
	unsigned int GetStrCode(const char *apString, const unsigned int aiLen, unsigned int *aiPos,
                            const char *apChrSet = NULL);
	int ResizeTNodeLst(unsigned int auiNewSize);
	int ResizeBuf(unsigned int auiNewSize);
	int AdjustSEInfo();
	int ResizeSEInfo(unsigned int auiNewSize);
	int ResizeDicEntry(unsigned int auiNewSize);
	int InitSuffEntry(unsigned int auiEntry, unsigned int auiHSize,unsigned int auiBTPos);
	unsigned int GetSuffEntry(unsigned int auiEntry, unsigned int auiHPos);
	unsigned int GetSuffHSize(unsigned int auiEntry);
	unsigned int GetBTEntry(unsigned int auiEntry);
	int SetSuffEntry(unsigned int auiEntry, unsigned int auiHPos,unsigned int auiEPos);
	unsigned int SeekEntry(unsigned int auiEPos, unsigned int auiStrCode);
	int PackResult(std::vector<KeyWordInfo> &result, TRIES_DICT_NODE *apNode,unsigned int auiOffset);
 private:
	char *pStrBuf_;
	unsigned int uiBufSize_;
	unsigned int uiBCurPos_;

	TRIES_DICT_ENTRY *pDicEntry_;
	unsigned int uiESize_;
	unsigned int uiECurPos_;

	unsigned int *pSEInfo_;
	unsigned int uiSESize_;
	unsigned int uiSECurPos_;

	TRIES_DICT_NODE *pTNodeLst_;
	unsigned int uiTSize_;
	unsigned int uiTCurPos_;

	unsigned int uiStartPos_;

	//No Copying Disabled
	Trie(const Trie&);
	Trie& operator=(const Trie&);
};//end of Trie

};//namespace end of Trie


#endif /* TRIESDICT_H_ */
