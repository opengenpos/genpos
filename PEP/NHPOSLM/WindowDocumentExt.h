
#if !defined(WINDOWDOCUMENTEXT_H_INCLUDED)

#define WINDOWDOCUMENTEXT_H_INCLUDED

#include "WindowDocument.h"
#include "WindowItemExt.h"
#include "WindowTextExt.h"

class CWindowDocumentExt : public CWindowDocument
{
public:
	CWindowDocumentExt();
	virtual ~CWindowDocumentExt();

	virtual BOOL CreateDocument (CWnd *pParentWnd = NULL, UINT nID = 0, CRect *rectSize = 0);

	virtual void ClearDocument (void);
	void Serialize( CArchive& ar );
	void PopupDefaultDlg();
	virtual int SearchForItem (int row, int column, UINT id = 0);
	virtual BOOL SearchForWindow(int row, int column);
	virtual int SearchForItemCorner (int row, int column, UINT id = 0);
	BOOL DeleteWndItem(int row, int column, UINT id);
	BOOL IsValidGrid(int row, int column, UINT id, CRect rect, CWindowControl *wc );
	BOOL ValidateText(CWindowTextExt::CWindowTextType type);
	BOOL ValidateWindow( CRect rect, CWindowItemExt *wi );
	BOOL ValidateWindowDoc(CWindowControl *wi );
	BOOL ValidateWindowDoc(CRect &windowRect );
	virtual BOOL WindowCreate (CWindowControl *pParentWnd, UINT nID = 0, int scale = 100) { return FALSE; }
	
	virtual BOOL WindowDisplay (CDC* pDC) {return FALSE;}
	virtual BOOL WindowDestroy (void) {PostMessage(WM_QUIT, 0, 0); return FALSE;}
	virtual void ControlSerialize (CArchive &ar) {}

	CWindowControl *GetWindowControl(int row, int col);	// A-KSo

	BOOL bModified;

	
/********************* Begin Undo Functionality********************************/
    CObList m_undolist;    // Stores undo states
    CObList m_redolist;    // Stores redo states
    long    m_growsize;    // Adjust for faster saves
    long    m_undoLevels;  // Requested Undolevels 
    long    m_chkpt;

    /*void AddUndo(CMemFile*);
    void AddRedo(CMemFile *pFile); 
    void Load(CMemFile*);
    void Store(CMemFile*);
    void ClearRedoList();


    //virtual void DeleteContents() = 0;

    BOOL CanUndo();      // Returns TRUE if can Undo
    BOOL CanRedo();      // Returns TRUE if can Redo
    void Undo();         // Restore next Undo state
    void Redo();         // Restore next Redo state
    void CheckPoint();   // Save current state 
    void EnableCheckPoint();
    void DisableCheckPoint();*/


// Remove contents of the redo list
inline void CWindowDocumentExt::
ClearRedoList()
{
    // Clear redo list
    POSITION pos = m_redolist.GetHeadPosition(); 
    CMemFile* nextFile = NULL;
    while(pos) {
        nextFile = (CMemFile *) m_redolist.GetNext(pos);
        delete nextFile;
    }
    m_redolist.RemoveAll();
}


// Checks undo availability, may be used to enable menus
inline BOOL CWindowDocumentExt::
CanUndo() 
{
    return (m_undolist.GetCount() > 1);
}

// Checks redo availability, may be used to enable menus
inline BOOL CWindowDocumentExt::
CanRedo() 
{
    return (m_redolist.GetCount() > 0);
}

// Adds state to the beginning of undo list
inline void CWindowDocumentExt::
AddUndo(CMemFile* file) 
{
    // Remove old state if there are more than max allowed
    if (m_undolist.GetCount() > m_undoLevels) {
        CMemFile* pFile = (CMemFile *) m_undolist.RemoveTail();
        delete pFile;
    }
    // Add new state to head of undo list
    m_undolist.AddHead(file);
}

// Saves current object into CMemFile instance
inline void CWindowDocumentExt::
Store(CMemFile* file) 
{
    file->SeekToBegin();
    CArchive ar(file, CArchive::store);
    Serialize(ar); 
    ar.Close();
}

// Loads CMemfile instance to current object
inline void CWindowDocumentExt::
Load(CMemFile* file) 
{
    ClearDocument();//DeleteContents(); 
    file->SeekToBegin();
    CArchive ar(file, CArchive::load);
    Serialize(ar); 
    ar.Close();
}

// Save current object state to Undo list
inline void CWindowDocumentExt::
CheckPoint() 
{
	bModified = TRUE;
    if (m_chkpt <= 0) {
        CMemFile* file = new CMemFile(m_growsize);
        Store(file);
        AddUndo(file);
        ClearRedoList();
    }
}

// Save current object state from serialization upon saving file
inline void CWindowDocumentExt::
CheckPointWithNoChange() 
{
    if (m_chkpt <= 0) {
        CMemFile* file = new CMemFile(m_growsize);
        Store(file);
        AddUndo(file);
        ClearRedoList();
    }
}

inline void CWindowDocumentExt::
EnableCheckPoint()
{
    if (m_chkpt > 0) {
        m_chkpt--;
    }
}

inline void CWindowDocumentExt::
DisableCheckPoint()
{
    m_chkpt++;
}

// Place CMemFile instnace on Redo list
inline void CWindowDocumentExt::
AddRedo(CMemFile *file) 
{
    // Move state to head of redo list
    m_redolist.AddHead(file);
}

// Perform an Undo command
inline void CWindowDocumentExt::
Undo() 
{
    if (CanUndo()) {
        // Remember that the head of the undo list
        // is the current state. So we just move that
        // to the Redo list and load then previous state.
        CMemFile *pFile = (CMemFile *) m_undolist.GetHead();
        m_undolist.RemoveHead();
        AddRedo(pFile);
        pFile = (CMemFile *)m_undolist.GetHead();
        Load(pFile);
    }
}

//Perform a Redo Command
inline void CWindowDocumentExt::
Redo() 
{
    if (CanRedo()) {
        CMemFile *pFile = (CMemFile *) m_redolist.GetHead() ;
        m_redolist.RemoveHead();
        AddUndo(pFile);
        Load(pFile);
    }
}
/************************End Undo Functionality********************************/

// Implementation
public:
	DECLARE_DYNAMIC(CWindowDocumentExt);

// Attributes
public:
	BOOL DeleteWndItemFromID(UINT id);
	void InitializeDefaults();
	void VerifyVersion();
	CWindowControl* SearchForSubWinItem(int row, int column, CWindowItemExt* wi, UINT id);
	BOOL ValidateSubWinText(CWindowTextExt::CWindowTextType type, CWindowItemExt* wi);
	CWindowItemExt* GetSelectedWindow(CWindowItemExt *wi, UINT id);
	CWindowItemExt* GetSelectedDocWindow( UINT id);
	BOOL DeleteWndControl(CWindowItemExt* pScreenItem, int row, int column);
private:
	
	// disable the = operator.  We do not want C++ compiler generating one
	// for us and causing errors by creating copies of our data structures.
	CWindowDocumentExt & operator = (CWindowDocumentExt & junk);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowDocumentExt)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CWindowDocumentExt)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};


#endif
