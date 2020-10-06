#ifndef _BLOOMFILTER_
#define _BLOOMFILTER_

#include<iostream>
#include<string>
#include<math.h>
#include<stdio.h>
#include <vector>
#include "hash.h"
#include <fstream>//ifstream���ļ���ofstreamд�ļ���fstream��д�ļ�

using namespace std;

class Bloomfilter{
public:
	Bloomfilter(double err_rate, int num, char* path);	//�����������ĵ�·��������������������ʧ����
	Bloomfilter(char* path);
	~Bloomfilter();
	bool is_contain(const char* str);	// �鿴Ԫ���Ƿ��ڲ�¡��
	int hashnum();						// ���ع�ϣ������Ŀ
	// double real_precision();			// ������ʵ��ʧ����
	int sizeofpool();					// ����len
	void filter_init();					// ��ʼ����¡������
	void savebloom();

private:
	void listinit();					// ��path·�����ĵ�������ÿһ���������ڴ�bitpool��
	Bloomfilter() = delete;				// ���û����ù��캯��

	int  hashtable_init();				// �ѹ�ϣ�������뵽hastable�����У���ϣ��������������ڼ��������k
	int len;							// ��¡��Ҫ���ڴ�ĳ���
	char* mypath;						// �ļ���·����ͨ�����캯������·��
	double precision;					// ʧ����
	int *bitpool;						// ��ʾ��Ҫ���ڴ�,�ڹ��캯��������
	int bitpoollen;						// ��Ҫ�Ķ�����λ��m
	int samplenum;						// �������������캯������
	int hashfuncnum;					// ��Ҫ�Ĺ�ϣ�����ĸ���k�� k <= hashtable.size();
	vector<unsigned int(*)(const char*)> hashtable;	//��ż����ַ�����ϣֵ�Ĺ�ϣ����
};

double lg2(double n)
{
	return log(n) / log(2);
}

// ����ϣ��������hashtable��
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


// ���캯������ɲ�¡�����ڴ桢��ϣ�����ļ���
Bloomfilter::Bloomfilter(double err_rate, int num, char* path)
	:mypath(path)
	,samplenum(num)
{
	bitpoollen = -((samplenum * log(err_rate)) / (log(2) * log(2)));		// ��������ı���λ
	hashfuncnum = 0.7 * (bitpoollen / samplenum);					// ��������Ĺ�ϣ������Ŀ
	len = bitpoollen / 32 + 1;
	bitpool = new int[len];
}

// ���ļ��г�ʼ����¡������
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

// ���ع�ϣ��������Ŀ
int Bloomfilter::hashnum()
{
	return hashfuncnum;
}

// ���ز�¡�������Ĵ�С����λΪ����
int Bloomfilter::sizeofpool()
{
	return len;
}

// ��¡��������ʼ��
void Bloomfilter::filter_init()
{
	hashtable_init();
	if (hashfuncnum > hashtable.size())
	{
		printf("���й�ϣ����%d���������ϣ����Ϊ%d��\n��ϵ�������еĺ�������,�����������%d��!", 
			hashtable.size(), hashfuncnum, hashfuncnum - hashtable.size());
		exit(1);
	}
	listinit();
}

// �鿴Ԫ���Ƿ��ڲ�¡��
bool Bloomfilter::is_contain(const char* str)
{
	int  hashval;
	for (int i = 0; i != hashfuncnum; i++)
	{
		hashval = hashtable[i](str);
		hashval = hashval % (len * 32); //len*32Ϊbitpool����λ��
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

// ������¡������
void Bloomfilter::listinit()
{
	FILE* fp;
	size_t length = 0;
	fp = fopen(mypath, "r+");
	if (fp == nullptr)
	{
		perror("open file error������");
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

		// ��Ԫ��ӳ�䵽��¡��������
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

// ����¡������д���ļ���
void Bloomfilter::savebloom()
{
	FILE* fp;
	fp = fopen("white_domain.txt", "w+");
	if (fp == nullptr)
	{
		perror("open write file error������");
		exit(1);
	}
	char str[100]{};
	_itoa(len, str, 10);
	fputs(str, fp);			// ����¡����������ֽ���Ŀд��
	fputc('\n', fp);
	
	_itoa(hashfuncnum, str, 10);
	fputs(str, fp);			// ����¡����Ĺ�ϣ������Ŀд��
	fputc('\n', fp);

	// ������Ϊ��λ����¡д���ļ�
	for (int i = 0; i < len; i++)
	{
		_itoa(bitpool[i], str, 10);
		fputs(str, fp);
		fputc('\n', fp);
	}
}

// ��������
Bloomfilter::~Bloomfilter()
{
	delete[] bitpool;
}


#endif
