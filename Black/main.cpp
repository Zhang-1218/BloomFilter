
#include<iostream>
#include"bloomfilter.h"

using namespace std;

int main()
{
	Bloomfilter* bloom = new Bloomfilter("./white_domain.txt");
	
	//Bloomfilter* bloom = new Bloomfilter(0.001, 6, "./redlist.txt");
	//bloom->filter_init();
	cout << "hash number: " << bloom->hashnum() << endl;
	cout << "the number of int: " << bloom->sizeofpool() << endl;
	cout << "www.baidu.com�ڼ�������: " << (bloom->is_contain("www.dubai.com") ? "��" : "����") << endl;
	cout << "www.baidu.com�ڼ�������: " << (bloom->is_contain("www.didi.com") ? "��" : "����") << endl;
	cout << "www.baidu.com�ڼ�������: " << (bloom->is_contain("www.aliyun.com") ? "��" : "����") << endl;
	cout << "www.qq.com�ڼ�������: " << (bloom->is_contain("www.qq.com") ? "��" : "����") << endl;

	bloom->savebloom();

	return 0;
}
