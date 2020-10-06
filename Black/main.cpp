
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
	cout << "www.baidu.com在集合中吗: " << (bloom->is_contain("www.dubai.com") ? "在" : "不在") << endl;
	cout << "www.baidu.com在集合中吗: " << (bloom->is_contain("www.didi.com") ? "在" : "不在") << endl;
	cout << "www.baidu.com在集合中吗: " << (bloom->is_contain("www.aliyun.com") ? "在" : "不在") << endl;
	cout << "www.qq.com在集合中吗: " << (bloom->is_contain("www.qq.com") ? "在" : "不在") << endl;

	bloom->savebloom();

	return 0;
}
