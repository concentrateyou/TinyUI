#include "stdafx.h"
#include "MediaCaptureDlg.h"
#include "Resource.h"

namespace DXApp
{
	IMPLEMENT_DYNAMIC(MediaCaptureDlg, TinyCustomDialog);
	MediaCaptureDlg::MediaCaptureDlg()
		:m_audioName(NULL),
		m_videoName(NULL),
		m_audioParam(NULL),
		m_videoParam(NULL)
	{
		m_onRefreshClick.Reset(new Delegate<void(void*, INT)>(this, &MediaCaptureDlg::OnRefreshClick));
		m_button.EVENT_CLICK += m_onRefreshClick;

		m_onVideoSelectChange.Reset(new Delegate<void(INT)>(this, &MediaCaptureDlg::OnVideoSelectChange));
		m_video.EVENT_SELECTCHANGE += m_onVideoSelectChange;

		m_onVideoCapSelectChange.Reset(new Delegate<void(INT)>(this, &MediaCaptureDlg::OnVideoCapSelectChange));
		m_videoCap.EVENT_SELECTCHANGE += m_onVideoCapSelectChange;

		m_onAudioSelectChange.Reset(new Delegate<void(INT)>(this, &MediaCaptureDlg::OnAudioSelectChange));
		m_audio.EVENT_SELECTCHANGE += m_onAudioSelectChange;

		m_onAudioCapSelectChange.Reset(new Delegate<void(INT)>(this, &MediaCaptureDlg::OnAudioCapSelectChange));
		m_audioCap.EVENT_SELECTCHANGE += m_onAudioCapSelectChange;
	}


	MediaCaptureDlg::~MediaCaptureDlg()
	{
		m_button.EVENT_CLICK -= m_onRefreshClick;
		m_video.EVENT_SELECTCHANGE -= m_onVideoSelectChange;
		m_videoCap.EVENT_SELECTCHANGE -= m_onVideoCapSelectChange;
		m_audio.EVENT_SELECTCHANGE -= m_onAudioSelectChange;
		m_audioCap.EVENT_SELECTCHANGE -= m_onAudioCapSelectChange;
	}


	LRESULT MediaCaptureDlg::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			if (EndDialog(LOWORD(wParam)))
			{

			}
			break;
		}
		return TinyCustomDialog::OnCommand(uMsg, wParam, lParam, bHandled);

	}

	LRESULT MediaCaptureDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;

		m_button.SubclassDlgItem(IDC_BUTTON_REFRESH, m_hWND);
		m_video.SubclassDlgItem(IDC_COMBO_VIDEO, m_hWND);
		m_video.SubclassChildren();
		GetVideoDevices();
		m_videoCap.SubclassDlgItem(IDC_COMBO_VIDEO_CAPABILITY, m_hWND);
		m_audio.SubclassDlgItem(IDC_COMBO_AUDIO, m_hWND);
		GetAudioDevices();
		m_audioCap.SubclassDlgItem(IDC_COMBO_AUDIO_CAPABILITY, m_hWND);
		return FALSE;
	}

	void MediaCaptureDlg::OnRefreshClick(void*, INT)
	{
		m_audioCap.ResetContent();
		m_videoCap.ResetContent();
		m_audio.ResetContent();
		m_video.ResetContent();
		GetVideoDevices();
		GetAudioDevices();
	}

	DShow::AudioCapture::Name*  MediaCaptureDlg::GetAudioName()
	{
		return m_audioName;
	}
	DShow::VideoCapture::Name*	MediaCaptureDlg::GetVideoName()
	{
		return m_videoName;
	}

	DShow::AudioCaptureParam*	MediaCaptureDlg::GetAudioParam()
	{
		return m_audioParam;
	}
	DShow::VideoCaptureParam*	MediaCaptureDlg::GetVideoParam()
	{
		return m_videoParam;
	}

	void MediaCaptureDlg::GetVideoDevices()
	{
		m_videoNames.clear();
		m_videoParams.clear();
		DShow::VideoCapture::GetDevices(m_videoNames);
		for (UINT i = 0;i < m_videoNames.size();i++)
		{
			DShow::VideoCapture::Name& name = m_videoNames[i];
			m_video.SetItemData(m_video.AddString(name.name().c_str()), reinterpret_cast<INT_PTR>(&name));
		}
	}

	void MediaCaptureDlg::GetAudioDevices()
	{
		m_audioNames.clear();
		m_audioParams.clear();
		DShow::AudioCapture::GetDevices(m_audioNames);
		for (UINT i = 0;i < m_audioNames.size();i++)
		{
			DShow::AudioCapture::Name& name = m_audioNames[i];
			m_audio.SetItemData(m_audio.AddString(name.name().c_str()), reinterpret_cast<INT_PTR>(&name));
		}
	}


	void MediaCaptureDlg::OnVideoSelectChange(INT index)
	{
		m_videoCap.ResetContent();
		m_videoParams.clear();
		m_videoName = reinterpret_cast<DShow::VideoCapture::Name*>(m_video.GetItemData(index));
		DShow::VideoCapture::GetDeviceParams(*m_videoName, m_videoParams);
		for (UINT i = 0;i < m_videoParams.size();i++)
		{
			DShow::VideoCaptureParam& param = m_videoParams[i];
			m_videoCap.SetItemData(m_videoCap.AddString(param.ToString().c_str()), reinterpret_cast<INT_PTR>(&param));
		}
	}

	void MediaCaptureDlg::OnVideoCapSelectChange(INT index)
	{
		m_videoParam = reinterpret_cast<DShow::VideoCaptureParam*>(m_videoCap.GetItemData(index));
		m_videoParam->SetScale(m_videoParam->GetSize().cx / 4, m_videoParam->GetSize().cy / 4);
	}

	void MediaCaptureDlg::OnAudioSelectChange(INT index)
	{
		m_audioCap.ResetContent();
		m_audioParams.clear();
		m_audioName = reinterpret_cast<DShow::AudioCapture::Name*>(m_audio.GetItemData(index));
		DShow::AudioCapture::GetDeviceParams(*m_audioName, m_audioParams);
		for (UINT i = 0;i < m_audioParams.size();i++)
		{
			DShow::AudioCaptureParam& param = m_audioParams[i];
			m_audioCap.SetItemData(m_audioCap.AddString(param.ToString().c_str()), reinterpret_cast<INT_PTR>(&param));
		}
	}

	void MediaCaptureDlg::OnAudioCapSelectChange(INT index)
	{
		m_audioParam = reinterpret_cast<DShow::AudioCaptureParam*>(m_audioCap.GetItemData(index));
	}
}