#include "MyCrawler.h"

int main(void)
{
	string startUrl;//= "http://www.ivsky.com/tupian/meinv_t50/";
	//"http://www.wmpic.me/tupian";
	cout << "please input website you want to find pictures:" << endl;
	cin >> startUrl;
	CMyCrawler crawler(startUrl);
	return 0;
}
