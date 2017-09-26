#include <iostream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <algorithm>
#include <string>

using namespace std;
#define MAX_REFERER_DOMAIN_LENGTH 1024
int FindingString(const char* lpszSour, const char* lpszFind, int nStart /* = 0 */)
{
//	ASSERT(lpszSour && lpszFind && nStart >= 0);
	if(lpszSour == NULL || lpszFind == NULL || nStart < 0)
		return -1;
	int m = strlen(lpszSour);
	int n = strlen(lpszFind);
	if( nStart+n > m )
		return -1;
	if(n == 0)
		return nStart;
//KMP算法
	int* next = new int[n];
	//得到查找字符串的next数组
	{	n--;
		int j, k;
		j = 0;
		k = -1;
		next[0] = -1;
		while(j < n)
		{	if(k == -1 || lpszFind[k] == '?' || lpszFind[j] == lpszFind[k])
			{	j++;
				k++;
				next[j] = k;
			}
			else
				k = next[k];
		}
		n++;
	}
	int i = nStart, j = 0;
	while(i < m && j < n)
	{
		if(j == -1 || lpszFind[j] == '?' || lpszSour[i] == lpszFind[j])
		{	i++;
			j++;
		}
		else
			j = next[j];
	}
	delete []next;
	if(j >= n)
		return i-n;
	else
		return -1;
}
/*lpszMatch:pattern*/
bool match_string(const char* lpszSour, const char* lpszMatch, bool bMatchCase /*  = true */)
{
//	ASSERT(AfxIsValidString(lpszSour) && AfxIsValidString(lpszMatch));
	if(lpszSour == NULL || lpszMatch == NULL)
		return false;
	if(lpszMatch[0] == 0)//Is a empty string
	{
		if(lpszSour[0] == 0)
			return true;
		else
			return false;
	}
	int i = 0, j = 0;
	//生成比较用临时源字符串'szSource'
	char* szSource =
		new char[ (j = strlen(lpszSour)+1) ];
	if( bMatchCase )
	{	//memcpy(szSource, lpszSour, j);
		while( *(szSource+i) = *(lpszSour+i++) );
	}
	else
	{	//Lowercase 'lpszSour' to 'szSource'
		i = 0;
		while(lpszSour[i])
		{	if(lpszSour[i] >= 'A' && lpszSour[i] <= 'Z')
				szSource[i] = lpszSour[i] - 'A' + 'a';
			else
				szSource[i] = lpszSour[i];
			i++;
		}
		szSource[i] = 0;
	}
	//生成比较用临时匹配字符串'szMatcher'
	char* szMatcher = new char[strlen(lpszMatch)+1];
	//把lpszMatch里面连续的“*”并成一个“*”后复制到szMatcher中
	i = j = 0;
	while(lpszMatch[i])
	{
		szMatcher[j++] = (!bMatchCase) ?
								( (lpszMatch[i] >= 'A' && lpszMatch[i] <= 'Z') ?//Lowercase lpszMatch[i] to szMatcher[j]
										lpszMatch[i] - 'A' + 'a' :
										lpszMatch[i]
								) :
								lpszMatch[i];		 //Copy lpszMatch[i] to szMatcher[j]
		//Merge '*'
		if(lpszMatch[i] == '*')
			while(lpszMatch[++i] == '*');
		else
			i++;
	}
	szMatcher[j] = 0;
	//开始进行匹配检查
	int nMatchOffset, nSourOffset;
	bool bIsMatched = true;
	nMatchOffset = nSourOffset = 0;
	while(szMatcher[nMatchOffset])
	{
		if(szMatcher[nMatchOffset] == '*')
		{
			if(szMatcher[nMatchOffset+1] == 0)
			{	//szMatcher[nMatchOffset]是最后一个字符
				bIsMatched = true;
				break;
			}
			else
			{	//szMatcher[nMatchOffset+1]只能是'?'或普通字符
				int nSubOffset = nMatchOffset+1;
				while(szMatcher[nSubOffset])
				{	if(szMatcher[nSubOffset] == '*')
						break;
					nSubOffset++;
				}
				if( strlen(szSource+nSourOffset) <
						size_t(nSubOffset-nMatchOffset-1) )
				{	//源字符串剩下的长度小于匹配串剩下要求长度
					bIsMatched = false; //判定不匹配
					break;			//退出
				}
				if(!szMatcher[nSubOffset])//nSubOffset is point to ender of 'szMatcher'
				{	//检查剩下部分字符是否一一匹配
					nSubOffset--;
					int nTempSourOffset = strlen(szSource)-1;
					//从后向前进行匹配
					while(szMatcher[nSubOffset] != '*')
					{
						if(szMatcher[nSubOffset] == '?')
							;
						else
						{	if(szMatcher[nSubOffset] != szSource[nTempSourOffset])
							{	bIsMatched = false;
								break;
							}
						}
						nSubOffset--;
						nTempSourOffset--;
					}
					break;
				}
				else//szMatcher[nSubOffset] == '*'
				{	nSubOffset -= nMatchOffset;
					char* szTempFinder = new char[nSubOffset];
					nSubOffset--;
					memcpy(szTempFinder, szMatcher+nMatchOffset+1, nSubOffset);
					szTempFinder[nSubOffset] = 0;
					int nPos = ::FindingString(szSource+nSourOffset, szTempFinder, 0);
					delete []szTempFinder;
					if(nPos != -1)//在'szSource+nSourOffset'中找到szTempFinder
					{	nMatchOffset += nSubOffset;
						nSourOffset += (nPos+nSubOffset-1);
					}
					else
					{	bIsMatched = false;
						break;
					}
				}
			}
		}		//end of "if(szMatcher[nMatchOffset] == '*')"
		else if(szMatcher[nMatchOffset] == '?')
		{
			if(!szSource[nSourOffset])
			{	bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	//如果szMatcher[nMatchOffset]是最后一个字符，
				//且szSource[nSourOffset]不是最后一个字符
				bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
		else//szMatcher[nMatchOffset]为常规字符
		{
			if(szSource[nSourOffset] != szMatcher[nMatchOffset])
			{	bIsMatched = false;
				break;
			}
			if(!szMatcher[nMatchOffset+1] && szSource[nSourOffset+1])
			{	bIsMatched = false;
				break;
			}
			nMatchOffset++;
			nSourOffset++;
		}
	}
	delete []szSource;
	delete []szMatcher;
	return bIsMatched;
}
bool if_in(bool array[], unsigned long length)
{
    for(int i=0;i<(int)length;i++)
        if(array[i] == true) return true;
    return false;
}
int is_same_conf(list<string> whitelist, list<string> blacklist)
{
    whitelist.sort();
    blacklist.sort();
    list<string> result;
    std::set_intersection(
    whitelist.begin(),whitelist.end(),
    blacklist.begin(),blacklist.end(),
    std::inserter(result, result.end()));
    if (result.size() == 0 ) 
        return -1;//No intersection;
    else if (result.size() > 0)
        return 1;//intersection;
}
bool if_match_referer(string http_header_referer, list<string> referer)
{
    const char* patterns[] = {"http://","https://", NULL};
    const char* const url = http_header_referer.c_str();
    char* domain = (char*)malloc(sizeof(char)*MAX_REFERER_DOMAIN_LENGTH);
    int j=0; 
    size_t start = 0; 
    for(int i=0; patterns[i]; i++) 
    {    
        if( strncmp( url, patterns[i], strlen( patterns[i] ) ) == 0 )
        {
            start = strlen(patterns[i]);
        }
    }    
    for(int i=start;url[i]!='/'&&url[i]!='\0';i++,j++)
        domain[j] = url[i];
    domain[j] = '\0';
    const char* match_domain = domain;
    bool match_result[referer.size()];
    memset(match_result, 0, referer.size()*sizeof(bool));
    unsigned long i =0;
    list<string>::iterator iter;
    
    iter = find(referer.begin(), referer.end(), match_domain);
    if (iter != referer.end())
    {
        return true;
    }
    else /*if (iter == referer.end())*/
    {
        /*wildcard*/
        for(iter=referer.begin();iter!=referer.end()&&i<referer.size();++iter,i++)
            match_result[i] = match_string( match_domain, (*iter).c_str(), true  );
        bool if_match = if_in(match_result, referer.size());
        for(int j=0;j<(int)i;j++)
        {
            cout<<"match_result: \n"<<match_result[j]<<endl;
        }
        if (!if_match)
        {
            return false;
        }
        else
            return true;
    }
}

int verify_referer(string header_referer, list<string> b_list, list<string> w_list)
{    
    bool in_white_list, in_black_list;
    list<string> black_list = b_list;
    list<string> white_list = w_list;
    in_white_list = if_match_referer( header_referer, white_list  );
    in_black_list = if_match_referer( header_referer, black_list  );
    /*if whitelist and blacklist are same*/
    if (is_same_conf(white_list, black_list) > 0) 
    {    
        if (in_white_list)
            return 0;
        else
            return -1;
    }    
    if( !white_list.empty() && !black_list.empty() )
    {    
        if( in_white_list && !in_black_list )
        {
            cout<<"----> in_white_list, Not in_black_list"<<endl;
            return 0;
        }
        else if (!in_white_list && in_black_list)
        {
            cout<<"----> Not in_white_list, in black_list"<<endl;
            return -1;
        }
    }    
    if (!white_list.empty() && black_list.empty())
    {    
        if (in_white_list)
            return 0;
        else 
            return -1;
    }    
    if (white_list.empty() && black_list.empty())
        return 0;
    else if (white_list.empty() && !black_list.empty())
    {    
        if (in_black_list)
            return -1;
        else 
            return 0;
    }    
}
int main()
{
    /*
    list<string> wlist = {"www.baidu.com"};
    list<string> blist = {"www.baidu.com"};
    */
    /*
    list<string> wlist = {"www.baidu.com", "www.eayun.com"};                                                                                                                                              
    list<string> blist = {"www.baidu.com"};
    */
    
    list<string> wlist = {"www.baidu.*"};
    list<string> blist = {"www.baidu.com"};
    

    string referer = "http://www.baidu.com";
    /*
     * -1;deny
     *  0:allow
     *  */
    int result = verify_referer(referer,blist,wlist);
    if (result<0)
        cout<<"deny"<<endl;
    else if (result==0)
        cout<<"allow"<<endl;

    return 0;
}
