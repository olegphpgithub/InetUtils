
class InetUtils
{
public:

	DWORD DownloadFileEx(
		LPCTSTR url_download,
		LPCTSTR url_verify,
		LPCTSTR url_report,
		LPCTSTR file_code,
		LPCTSTR file_name,
		LPCTSTR result_good,
		LPCTSTR result_bad
	);

	DWORD VerifyDownloadedFile(
		LPCTSTR url_verify,
		LPCTSTR file_code,
		LPCTSTR file_name
	);
	
	DWORD InternetRequestFeedback(
		LPCTSTR url,
		LPTSTR buf,
		const DWORD count
	);

	DWORD InternetRequestDownload(
		LPCTSTR url,
		LPCTSTR dest
	);
	
	DWORD inetTransfer(
		LPCTSTR url,
		LPCTSTR dest
	);

};
