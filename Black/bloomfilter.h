#ifndef _BLOOMFILTER_
#define _BLOOMFILTER_

#include<iostream>
#include<string>
#include<math.h>
#include<stdio.h>
#include <vector>
#include "hash.h"
#include <fstream>//ifstream读文件，ofstream写文件，fstream读写文件

using namespace std;

class Bloomfilter{
public:
	Bloomfilter(double err_rate, int num, char* path);	//传入样本的文档路径，样本个数，期望的失误率
	Bloomfilter(char* path);
	~Bloomfilter();
	bool is_contain(const char* str);	// 查看元素是否在布隆中
	int hashnum();						// 返回哈希函数数目
	// double real_precision();			// 返回真实的失误率
	int sizeofpool();					// 返回len
	void filter_init();					// 初始化布隆过滤器
	void savebloom();

private:
	void listinit();					// 打开path路径的文档，计算每一行样本到内存bitpool中
	Bloomfilter() = delete;				// 向用户禁用构造函数

	int  hashtable_init();				// 把哈希函数加入到hastable容器中，哈希函数个数必须大于计算所需的k
	int len;							// 布隆需要的内存的长度
	char* mypath;						// 文件的路径，通过构造函数传入路径
	double precision;					// 失误率
	int *bitpool;						// 表示需要的内存,在构造函数中申请
	int bitpoollen;						// 需要的二进制位数m
	int samplenum;						// 样本个数，构造函数传入
	int hashfuncnum;					// 需要的哈希函数的个数k， k <= hashtable.size();
	vector<unsigned int(*)(const char*)> hashtable;	//存放计算字符串哈希值的哈希函数
};

double lg2(double n)
{
	return log(n) / log(2);
}

// 将哈希函数加入hashtable中
int Bloomfilter::hashtable_init()
{
	hashtable.push_back(*JSHash);
	hashtable.push_back(*RSHash);
	hashtable.push_back(*SDBMHash);
	hashtable.push_back(*APHash);
	hashtable.push_back(*BKDRHash);
	hashtable.push_back(*Hash1);
	hashtable.push_back(*Hash2);
	hashtable.push_back(*Hash3);
	hashtable.push_back(*Hash4);
	hashtable.push_back(*Hash5);

	return hashtable.size();
}


// 构造函数：完成布隆所需内存、哈希函数的计算
Bloomfilter::Bloomfilter(double err_rate, int num, char* path)
	:mypath(path)
	,samplenum(num)
{
	bitpoollen = -((samplenum * log(err_rate)) / (log(2) * log(2)));		// 计算所需的比特位
	hashfuncnum = 0.7 * (bitpoollen / samplenum);					// 计算所需的哈希函数数目
	len = bitpoollen / 32 + 1;
	bitpool = new int[len];
}

// 从文件中初始化布隆过滤器
Bloomfilter::Bloomfilter(char* path)
{
	FILE* fp;
	fp = fopen(path, "r+");
	if (fp == nullptr)
	{
		perror("Bloom file open error!!!");
		exit(1);
	}

	char buf[256];
	char* p;

	p = fgets(buf, sizeof(buf), fp);
	if (!p)
		return;
	printf("%s", p);
	len = atoi(p);

	p = fgets(buf, sizeof(buf), fp);
	if (!p)
		return;
	printf("%s", p);
	hashfuncnum = atoi(p);
	bitpool = new int[len] {0};

	int i = 0;
	while (!feof(fp))
	{
		p = fgets(buf, sizeof(buf), fp);
		if (!p)
			break;

		printf("%d\n", atoi(p));
		bitpool[i] = atoi(p);
		i++;
	}

	hashtable_init();
}

// 返回哈希函数的数目
int Bloomfilter::hashnum()
{
	return hashfuncnum;
}

// 返回布隆过滤器的大小，单位为整型
int Bloomfilter::sizeofpool()
{
	return len;
}

// 布隆过滤器初始化
void Bloomfilter::filter_init()
{
	hashtable_init();
	if (hashfuncnum > hashtable.size())
	{
		printf("现有哈希函数%d个，所需哈希函数为%d个\n哈系函数表中的函数不足,请至少再添加%d个!", 
			hashtable.size(), hashfuncnum, hashfuncnum - hashtable.size());
		exit(1);
	}
	listinit();
}

// 查看元素是否在布隆中
bool Bloomfilter::is_contain(const char* str)
{
	int  hashval;
	for (int i = 0; i != hashfuncnum; i++)
	{
		hashval = hashtable[i](str);
		hashval = hashval % (len * 32); //len*32为bitpool的总位数
		cout << hashval << endl;

		if (bitpool[hashval / 32] & (0x1 << (hashval % 32)))
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

// 创建布隆过滤器
void Bloomfilter::listinit()
{
	FILE* fp;
	size_t length = 0;
	fp = fopen(mypath, "r+");
	if (fp == nullptr)
	{
		perror("open file error！！！");
		exit(1);
	}

	int hashval;
	char* p;

	char buf[256];
	while (!feof(fp))
	{
		p = fgets(buf, sizeof(buf), fp);
		if (!p)
			break;
		
		printf("%s", p);

		while (*p != '\n')
		{
			p++;
		}
		*p = '\0';

		// 将元素映射到布隆过滤器中
		for (int i = 0; i != hashfuncnum; i++)
		{
			hashval = hashtable[i](buf);
			hashval = hashval % (len * 32);
			//cout << hashval << endl;
			bitpool[hashval / 32] |= (0x1 << (hashval % 32));
		}
	}

	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
}

// 将布隆过滤器写入文件中
void Bloomfilter::savebloom()
{
	FILE* fp;
	fp = fopen("white_domain.txt", "w+");
	if (fp == nullptr)
	{
		perror("open write file error！！！");
		exit(1);
	}
	char str[100]{};
	_itoa(len, str, 10);
	fputs(str, fp);			// 将布隆所需的整型字节数目写入
	fputc('\n', fp);
	
	_itoa(hashfuncnum, str, 10);
	fputs(str, fp);			// 将布隆所需的哈希函数数目写入
	fputc('\n', fp);

	// 以整型为单位将布隆写入文件
	for (int i = 0; i < len; i++)
	{
		_itoa(bitpool[i], str, 10);
		fputs(str, fp);
		fputc('\n', fp);
	}
}

// 析构函数
Bloomfilter::~Bloomfilter()
{
	delete[] bitpool;
}


#endif
