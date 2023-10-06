#pragma once



class CMainDlg : public CDialogEx
{

public:
	CMainDlg( CWnd * pParent = nullptr ) ;


#ifdef AFX_DESIGN_TIME
	enum
	{
		IDD = IDD_WAVEGEN2_DIALOG
	} ;
#endif

protected :
	virtual void DoDataExchange( CDataExchange * pDX ) ;

	bool WriteWaveOnMemory( double const & freq1 , double const & freq2 , double const & amp ) ;

	BOOL PreTranslateMessage( MSG * pMsg ) ;
	void OnPressEnter() ;


protected :
	HICON m_hIcon ;

	virtual BOOL OnInitDialog() ;
	afx_msg void OnPaint() ;
	afx_msg HCURSOR OnQueryDragIcon() ;

	DECLARE_MESSAGE_MAP()
public :
	CSliderCtrl m_freqSilder1 ;
	CSliderCtrl m_freqSilder2 ;

	afx_msg void OnNMCustomdrawSliderFreq1( NMHDR * pNMHDR , LRESULT * pResult ) ;
	afx_msg void OnNMCustomdrawSliderFreq2( NMHDR * pNMHDR , LRESULT * pResult ) ;
	afx_msg void OnBnClickedButtonPlay();
} ;
