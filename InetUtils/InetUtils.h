
class InetUtils
{
public:

	enum InetType {
		Feedback = 1,
		SSLCert = 2,
		Download = 4
	};

	static DWORD DownloadFileEx(
		LPCTSTR url_download,
		LPCTSTR url_verify,
		LPCTSTR url_report,
		LPCTSTR file_code,
		LPCTSTR file_name,
		LPCTSTR result_good,
		LPCTSTR result_bad
	);

	static DWORD VerifyDownloadedFile(
		LPCTSTR url_verify,
		LPCTSTR file_code,
		LPCTSTR file_name
	);
	
	static DWORD InternetRequestFeedback(
		LPCTSTR url,
		LPTSTR buf,
		LPDWORD count
	);

	static DWORD InternetRequestDownload(
		LPCTSTR url,
		LPCTSTR dest
	);
	
	static DWORD InetTransfer(
		LPCTSTR url,
		InetType type,
		LPTSTR lpszBuffer,
		LPDWORD cchBuffer,
		LPTSTR lpszCertificate,
		LPDWORD cchCertificate,
		LPCTSTR dest
	);

};
