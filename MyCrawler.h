#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "winsock2.h"
#include <time.h>
#include <queue>
#include <hash_set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex>

#pragma comment(lib, "ws2_32.lib") 
using namespace std;
class CMyCrawler
{
private:
	queue<string> url;
	hash_set<string> visitedUrl;
	hash_set<string> visitedImg;
	void SearchUrl(const string &startUrl);
	bool GetHostAndResource(const string &url, string &host, string &resource);
	bool GetWebResponse(const string &url, string &txt);
	string ChangeToFileName(const string &startUrl);
	void GetImageUrl(vector<string> &imgUrls,const string &txt, const string &startUrl);
	void DownloadImage(vector<string> &imgUrls, const string &startUrl);
public:
	CMyCrawler(const string &startUrl);
	~CMyCrawler();
};

