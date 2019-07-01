#include "stdafx.h"

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>

#include "InetUtils.h"

#define NSIS_MAX_STRLEN 1024

int _tmain(int argc, _TCHAR* argv[])
{
	//TCHAR url[NSIS_MAX_STRLEN] = TEXT("https://korzuno.s3.eu-north-1.amazonaws.com/__argv.exe?hello=89");
	TCHAR url[NSIS_MAX_STRLEN] = TEXT("http://192.168.11.143/get.php");
	TCHAR file[NSIS_MAX_STRLEN] = TEXT("_https.exe");
	DWORD dwTotal = NSIS_MAX_STRLEN;
	DWORD dwResult;
	//dwResult= InetUtils::InternetRequestDownload(url, file);
	dwResult = InetUtils::InternetRequestFeedback(url, file, &dwTotal);
	_tprintf("Result: %d\n", dwResult);
	return 0;
}
