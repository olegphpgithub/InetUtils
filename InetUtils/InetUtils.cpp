
#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>

#include "InetUtils.h"

#pragma comment(lib, "wininet.lib")

#define USERAGENT TEXT("NSIS_Inetc (Mozilla)")
#define BUFFER_SIZE 1024
#define NSIS_MAX_STRLEN 1024
#define MY_REDIR_FLAGS INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS
#define MY_HTTPS_FLAGS (MY_REDIR_FLAGS | INTERNET_FLAG_SECURE)


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


DWORD InetUtils::VerifyDownloadedFile(
	LPCTSTR url_verify,
	LPCTSTR file_code,
	LPCTSTR file_name
	)
{
	DWORD dwLastError = ERROR_SUCCESS;
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
			TEXT(USERAGENT),
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

									if (bRead == FALSE || dwBytesReaded == 0)
										break;

									dwBytesLeft = *cchBuffer - dwBytesWrittenAccum;
									dwBytesToWrite = (dwBytesLeft > dwBytesReaded) ? dwBytesReaded : dwBytesLeft;
									errno_t err = memcpy_s(
										lpszBuffer + dwBytesWrittenAccum,
										dwBytesLeft,
										szData,
										dwBytesToWrite
									);
									dwBytesWrittenAccum += dwBytesToWrite;

									if(dwBytesToWrite == dwBytesLeft) {
										lpszBuffer[dwBytesWrittenAccum - 1] = 0;
										break;
									}

								}

							} else {
								dwLastError = ERROR_INVALID_PARAMETER;
							}

						}

						if(type == Download) {

							if(dest == NULL) {

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
