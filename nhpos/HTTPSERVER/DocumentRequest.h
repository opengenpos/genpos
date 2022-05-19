

#if ! defined(DocumentRequest_h_Included)

#define DocumentRequest_h_Included

class CDocumentRequest {
public:
	CDocumentRequest ();
	~CDocumentRequest ();

	typedef enum { HeaderHttp = 0, HeaderContentType } HeaderEntry;

protected:
	CString RequestHeader [20];

private:

};

#endif