#include "pch.h"
#include "framework.h"
#include "App.h"
#include "CMainDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment( lib , "winmm" )
#include "mmsystem.h"


CMainDlg::CMainDlg( CWnd * pParent /*=nullptr*/ )
	: CDialogEx( IDD_WAVEGEN2_DIALOG , pParent )
{
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME ) ;
}

void CMainDlg::DoDataExchange( CDataExchange * pDX )
{
	CDialogEx::DoDataExchange( pDX ) ;
	DDX_Control( pDX , IDC_SLIDER_FREQ1 , m_freqSilder1 ) ;
	DDX_Control( pDX , IDC_SLIDER_FREQ2 , m_freqSilder2 ) ;
}

BEGIN_MESSAGE_MAP( CMainDlg , CDialogEx )
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY( NM_CUSTOMDRAW , IDC_SLIDER_FREQ1 , &CMainDlg::OnNMCustomdrawSliderFreq1 )
	ON_NOTIFY( NM_CUSTOMDRAW , IDC_SLIDER_FREQ2 , &CMainDlg::OnNMCustomdrawSliderFreq2 )
	ON_BN_CLICKED( IDC_BUTTON_PLAY , &CMainDlg::OnBnClickedButtonPlay )
END_MESSAGE_MAP()



BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog() ;

	SetIcon( m_hIcon , TRUE  ) ;
	SetIcon( m_hIcon , FALSE ) ;

	m_freqSilder1.SetRange( 1635 , 790213 ) ;
	m_freqSilder2.SetRange( 1635 , 790213 ) ;

	return TRUE ;
}


void CMainDlg::OnPaint()
{
	if( IsIconic() )
	{
		CPaintDC dc( this ) ;

		SendMessage( WM_ICONERASEBKGND , reinterpret_cast< WPARAM >( dc.GetSafeHdc() ) , 0 ) ;

		int cxIcon = GetSystemMetrics( SM_CXICON ) ;
		int cyIcon = GetSystemMetrics( SM_CYICON ) ;
		CRect rect;
		GetClientRect( &rect );
		int x = ( rect.Width()  - cxIcon + 1 ) / 2 ;
		int y = ( rect.Height() - cyIcon + 1 ) / 2 ;

		// 아이콘을 그립니다.
		dc.DrawIcon( x , y , m_hIcon ) ;
	}
	else
	{
		CDialogEx::OnPaint() ;
	}
}


HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast< HCURSOR >( m_hIcon ) ;
}



void CMainDlg::OnNMCustomdrawSliderFreq1( NMHDR * pNMHDR , LRESULT * pResult )
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>( pNMHDR ) ;
	
	int pos = m_freqSilder1.GetPos() ;

	CString tmp ;
	tmp.Format( _T( "%d.%d" ) , pos / 100 , pos % 100 ) ;

	SetDlgItemTextW( IDC_STATIC_FREQ_VAL1 , tmp ) ;

	*pResult = 0 ;
}


void CMainDlg::OnNMCustomdrawSliderFreq2( NMHDR * pNMHDR , LRESULT * pResult )
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>( pNMHDR ) ;

	int pos = m_freqSilder2.GetPos() ;

	CString tmp ;
	tmp.Format( _T( "%d.%d" ) , pos / 100 , pos % 100 ) ;

	SetDlgItemTextW( IDC_STATIC_FREQ_VAL2 , tmp ) ;

	*pResult = 0;
}


void CMainDlg::OnBnClickedButtonPlay()
{
	double amp = 3000.0 ;

	int pos1 = m_freqSilder1.GetPos() ;
	int pos2 = m_freqSilder2.GetPos() ;

	double freq1 = pos1 / 100.0 ;
	double freq2 = pos2 / 100.0 ;
	
	WriteWaveOnMemory( freq1 , freq2 , amp ) ;
}



bool CMainDlg::WriteWaveOnMemory( double const & freq1 , double const & freq2 , double const & amp )
{
	uint16_t channel = 2 ;
	uint32_t sampleRate = 44100 ;
	uint8_t  bitRate = 16 ;
	uint8_t  duration = 1 ;   // second
	uint32_t rawDataSize = duration * sampleRate * channel * bitRate / 8 ;

	WAVEFORMATEX pwfx ;
	pwfx.wFormatTag = WAVE_FORMAT_PCM ;
	pwfx.nChannels = channel ; // 1 - mono , 2 - stereo
	pwfx.nSamplesPerSec = sampleRate ; // 44.1 kHz
	pwfx.wBitsPerSample = bitRate ;
	pwfx.nBlockAlign = channel * bitRate / 8 ;
	pwfx.nAvgBytesPerSec = sampleRate * channel * bitRate / 8 ; // bit rate
	pwfx.cbSize = 0 ;

	/*
	 *  _Out_opt_ LPHWAVEOUT      phwo ,
		 _In_     UINT            uDeviceID ,
		 _In_     LPCWAVEFORMATEX pwfx ,
		 _In_opt_ DWORD_PTR       dwCallback ,
		 _In_opt_ DWORD_PTR       dwInstance ,
		 _In_     DWORD           fdwOpen
	 ) ;
	 */
	CString tmp ;
	UINT numbersOfDevice = waveOutGetNumDevs() ;
	if( !numbersOfDevice )
	{
		//AfxMessageBox( _T( "No device available" ) ) ;
		return false ;
	}

	MMRESULT res = waveOutOpen( 0 , WAVE_MAPPER , & pwfx , 0 , 0 , WAVE_FORMAT_QUERY ) ;
	if( res )
	{
		tmp.Format( _T( "In this system, the specified format is not supported. cause number:[ %d ]" ) , res ) ;

		//AfxMessageBox( tmp ) ;
		return false ;
	}

	HWAVEOUT outHandle ;
	HWND hwnd = GetSafeHwnd() ;
	res = waveOutOpen( & outHandle , WAVE_MAPPER , & pwfx , /*( DWORD_PTR ) waveOutProc */ 0 , ( DWORD_PTR ) & hwnd , CALLBACK_WINDOW ) ;
	if( res )
	{
		tmp.Format( _T( "Fail to open - waveOutOpen(). cause number:[ %d ]" ) , res ) ;
		//AfxMessageBox( tmp ) ;
		return false ;
	}

	constexpr float PI = 3.141592653589793238462643f ;

	uint16_t val[ 400000 ] ;
	uint32_t valPos = 0 ;
	for( uint32_t pos = 0 ; pos < rawDataSize ; ++pos )
	{
		val[ valPos++ ] = static_cast< uint16_t >( amp * sin( 2 * PI * pos * freq1 / sampleRate ) ) ;
		val[ valPos++ ] = static_cast< uint16_t >( amp * sin( 2 * PI * pos * freq2 / sampleRate ) ) ;
	}

	WAVEHDR outHeader ;

	memset( & outHeader , 0 , sizeof( outHeader ) ) ;
	outHeader.dwBufferLength = rawDataSize ;
	outHeader.lpData = ( LPSTR ) val ;
	outHeader.dwLoops = 0 ;
	outHeader.dwFlags = WHDR_BEGINLOOP | WHDR_ENDLOOP ;

	res = waveOutPrepareHeader( outHandle , & outHeader , sizeof( outHeader ) ) ;
	if( res )
	{
		tmp.Format( _T( "Fail to open - waveOutPrepareHeader(). cause number:[ %d ]" ) , res ) ;
		//AfxMessageBox( tmp ) ;
		return false ;
	}

	res = waveOutWrite( outHandle , & outHeader , sizeof( outHeader ) ) ;
	if( res )
	{
		tmp.Format( _T( "Fail to open - waveOutWrite(). cause number:[ %d ]" ) , res ) ;
		//AfxMessageBox( tmp ) ;
		return false ;
	}

	waveOutUnprepareHeader( outHandle , & outHeader , sizeof( outHeader ) ) ;
	waveOutClose( outHandle ) ;

	return true ;
}



BOOL CMainDlg::PreTranslateMessage( MSG * pMsg )
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_RETURN )
		{
			OnPressEnter() ;
			return TRUE ;
		}
		else if( pMsg->wParam == VK_ESCAPE )
		{
			return TRUE ;
		}
	}

	return CDialogEx::PreTranslateMessage( pMsg ) ;
}


void CMainDlg::OnPressEnter()
{
	CString tmp ;

	if( GetDlgItem( IDC_EDIT_FREQ1 ) == GetFocus() )
	{
		GetDlgItemTextW( IDC_EDIT_FREQ1 , tmp ) ;

		int val = static_cast< int >( _ttof( tmp ) * 100.0 ) ;

		m_freqSilder1.SetPos( val ) ;
	}
	else if( GetDlgItem( IDC_EDIT_FREQ2 ) == GetFocus() )
	{
		GetDlgItemTextW( IDC_EDIT_FREQ2 , tmp ) ;

		int val = static_cast< int >( _ttof( tmp ) * 100.0 ) ;

		m_freqSilder2.SetPos( val ) ;
	}
}