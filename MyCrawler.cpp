#include "MyCrawler.h"

CMyCrawler::CMyCrawler(const string &startUrl)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		return;
	}
	CreateDirectoryA("./img", 0);
	CreateDirectoryA("./html", 0);
	
	url.push(startUrl);
	while (!url.empty()){
		string tempUrl = url.front();
		visitedUrl.insert(tempUrl);
		SearchUrl(tempUrl);
		url.pop();
	}
}

bool CMyCrawler::GetWebResponse(const string &url, string &txt)
{
	string host, resource;
	if (!GetHostAndResource(url, host, resource)){
		cout << "wrong url." << endl;
		return false;
	}
	struct hostent * hp = gethostbyname(host.c_str());
	if (hp == NULL){
		cout << "Can not find host address" << endl;
		return false;
	}
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1 || sock == -2){
		cout << "Can not create sock." << endl;
		return false;
	}
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);
	memcpy(&sa.sin_addr, hp->h_addr, 4);
	if (0 != connect(sock, (SOCKADDR*)&sa, sizeof(sa))){
		cout << "Can not connect: " << url << endl;
		closesocket(sock);
		return false;
	}
	string request = "GET " + resource + " HTTP/1.1\r\nHost:" + host + "\r\nConnection:Close\r\n\r\n";
	if (SOCKET_ERROR == send(sock, request.c_str(), request.size(), 0)){
		cout << "send error" << endl;
		closesocket(sock);
		return false;
	}

	char *buf = new char[1024+1];
	int recvNum = 1;
	while (recvNum>0){
		recvNum = recv(sock, buf, 1024, 0);
		buf[recvNum] = '\0';
		txt = txt + string(buf, recvNum);//不能使用txt.append(buf)，因为在读取image时，里面可能又'\0'。
	}
	closesocket(sock);
	return true;
}

bool CMyCrawler::GetHostAndResource(const string &url, string &host, string &resource)
{
	string::size_type  index = url.find("http://");

	if (std::string::npos == index){
		return false;
	}
	string tempStr = url.substr(index+7);
	string::size_type index1 = tempStr.find('/');
	if (std::string::npos == index1){
		return false;
	}
	host = tempStr.substr(0, index1);
	resource = tempStr.substr(index1);
	return true;
}

CMyCrawler::~CMyCrawler()
{
	WSACleanup();
}

string CMyCrawler::ChangeToFileName(const string &startUrl)
{
	string resultStr;
	for (string::const_iterator iter = startUrl.begin(); iter != startUrl.end(); ++iter){
		if (*iter != '/'&&*iter != '//'&&*iter!='.'&&*iter!=':')
			resultStr.push_back(*iter);
	}
	return resultStr;
}

void CMyCrawler::GetImageUrl(vector<string> &imgUrls,const string &txt, const string &startUrl)
{
	const std::regex pattern("href=\"\\S+\"");
	const std::sregex_token_iterator end;
	ofstream outfile("url.txt", ios::app);
	for (std::sregex_token_iterator i(txt.begin(), txt.end(), pattern); i != end; ++i){
		string tempStr(*i);
		tempStr = tempStr.substr(6, tempStr.size() - 7);
		if (tempStr.find("http://") != 0){
			string host, resource;
			GetHostAndResource(startUrl, host, resource);
			host.insert(0, "http://");
			tempStr.insert(0, host);
		}
		if (visitedUrl.find(tempStr) == visitedUrl.end()){
			url.push(tempStr);
			visitedUrl.insert(tempStr);
			outfile << tempStr << endl;
		}
		
	}
	outfile.close();

	const std::regex pattern1("<img src=\"http:\\S+\"");
	const std::sregex_token_iterator end1;
	for (std::sregex_token_iterator i(txt.begin(), txt.end(), pattern1); i != end1; ++i){
		string tempStr(*i);
		tempStr = tempStr.substr(tempStr.find("http:"));
		tempStr.pop_back();
		if (visitedImg.find(tempStr) == visitedImg.end()){
			cout << tempStr << endl;
			imgUrls.push_back(tempStr);
			visitedImg.insert(tempStr);
		}
		
	}
}

void CMyCrawler::DownloadImage(vector<string> &imgUrls, const string &startUrl)
{
	string filename = ChangeToFileName(startUrl);
	filename = "./img/" + filename;
	CreateDirectoryA(filename.c_str(), 0);
	for (auto iter = imgUrls.begin(); iter != imgUrls.end(); ++iter){
		string tempStr(*iter);
		std::string::size_type index = tempStr.find_last_of('.');
		if (index == std::string::npos){
			continue;
		}
		string str = tempStr.substr(index + 1);
		if (str != "jpg"&&str != "bmp"&&str != "jpeg"&&str != "gif"&&str != "png") {
			continue;
		}
		string image;
		if (!GetWebResponse(*iter, image)){
			continue;
		}
		index = iter->find_last_of('/');
		if (index == std::string::npos){
			continue;
		}
		string imgName = iter->substr(index);
		ofstream outfile(filename + imgName,ios::binary);
		if (!outfile) {
			continue;
		}
		const char *p = image.c_str();
		const char * pos = strstr(p, "\r\n\r\n") + strlen("\r\n\r\n");
		outfile.write(pos, image.size() - (pos - p));
		outfile.close();
	}
	
}

void CMyCrawler::SearchUrl(const string &startUrl)
{
	string txt;
	if (!GetWebResponse(startUrl, txt))
		return;
	string filename=ChangeToFileName(startUrl);
	filename = "./html/" + filename + ".txt";
	ofstream outfile(filename.c_str());
	if (!outfile){
		cout << "error:unable to open file:" << filename << endl;
		return;
	}
	outfile << txt;
	outfile.close();
	vector<string> imgUrls;
	GetImageUrl(imgUrls, txt, startUrl);
	DownloadImage(imgUrls, startUrl);
}
