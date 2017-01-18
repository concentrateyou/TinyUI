#include "stdafx.h"
#include "Mixer.h"

namespace DShow
{
	Mixer::Mixer()
		:m_hMixer(NULL)
	{
	}
	Mixer::~Mixer()
	{
		Close();
	}
	BOOL Mixer::Close()
	{
		MMRESULT hRes = MMSYSERR_NOERROR;
		if (m_hMixer != NULL)
		{
			hRes = mixerClose(m_hMixer);
			m_hMixer = NULL;
		}
		return  hRes == MMSYSERR_NOERROR;
	}
	BOOL Mixer::Initialize(DWORD dstType, DWORD srcType)
	{
		DWORD dwCount = mixerGetNumDevs();
		if (dwCount <= 0)
			return FALSE;
		MMRESULT hRes = MMSYSERR_NOERROR;
		for (DWORD i = 0;i < dwCount;i++)
		{
			hRes = mixerOpen(&m_hMixer, i, 0, 0, MIXER_OBJECTF_MIXER);
			if (hRes != MMSYSERR_NOERROR)
				continue;
			ZeroMemory(&m_ml, sizeof(m_ml));
			m_ml.cbStruct = sizeof(MIXERLINE);
			m_ml.dwComponentType = dstType;
			hRes = mixerGetLineInfo((HMIXEROBJ)m_hMixer, &m_ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
			if (hRes != MMSYSERR_NOERROR)
			{
				mixerClose(m_hMixer);
				m_hMixer = NULL;
				continue;
			}
			for (DWORD i = 0; i < m_ml.cConnections; i++)
			{
				MIXERLINE ml = { 0 };
				ml.cbStruct = sizeof(MIXERLINE);
				ml.dwSource = i;
				ml.dwDestination = m_ml.dwDestination;
				hRes = mixerGetLineInfo((HMIXEROBJ)m_hMixer, &ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE);
				if (hRes != MMSYSERR_NOERROR)
					return FALSE;
				if (ml.dwComponentType == srcType)
				{
					memcpy(&m_ml, &ml, sizeof(MIXERLINE));
					return TRUE;
				}
			}
		}
		return FALSE;
	}
	BOOL Mixer::SetMute(BOOL bMute)
	{
		ASSERT(m_hMixer);
		MIXERCONTROL mc;
		MIXERLINECONTROLS mlc;
		mlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mlc.dwLineID = m_ml.dwLineID;
		mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
		mlc.cControls = 1;
		mlc.cbmxctrl = sizeof(MIXERCONTROL);
		mlc.pamxctrl = &mc;
		MMRESULT hRes = mixerGetLineControls((HMIXEROBJ)m_hMixer, &mlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		MIXERCONTROLDETAILS_BOOLEAN mute;
		mute.fValue = bMute;
		MIXERCONTROLDETAILS mds;
		mds.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mds.dwControlID = mlc.cControls;
		mds.cChannels = 1;
		mds.cMultipleItems = 0;
		mds.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		mds.paDetails = &mute;
		hRes = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mds, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		return TRUE;
	}
	BOOL Mixer::GetMute(BOOL& bMute)
	{
		ASSERT(m_hMixer);
		MIXERCONTROL mc;
		MIXERLINECONTROLS mlc;
		mlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mlc.dwLineID = m_ml.dwLineID;
		mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
		mlc.cControls = 1;
		mlc.cbmxctrl = sizeof(MIXERCONTROL);
		mlc.pamxctrl = &mc;
		MMRESULT hRes = mixerGetLineControls((HMIXEROBJ)m_hMixer, &mlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		MIXERCONTROLDETAILS_BOOLEAN mute;
		MIXERCONTROLDETAILS mds;
		mds.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mds.dwControlID = mlc.cControls;
		mds.cChannels = 1;
		mds.cMultipleItems = 0;
		mds.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
		mds.paDetails = &mute;
		hRes = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mds, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		bMute = (BOOL)mute.fValue;
		return TRUE;
	}
	BOOL Mixer::GetVolume(DWORD& volume)
	{
		ASSERT(m_hMixer);
		MIXERCONTROL mc;
		MIXERLINECONTROLS mlc;
		mlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mlc.dwLineID = m_ml.dwLineID;
		mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mlc.cControls = 1;
		mlc.cbmxctrl = sizeof(MIXERCONTROL);
		mlc.pamxctrl = &mc;
		MMRESULT hRes = mixerGetLineControls((HMIXEROBJ)m_hMixer, &mlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		MIXERCONTROLDETAILS_UNSIGNED vol;
		MIXERCONTROLDETAILS mds;
		mds.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mds.dwControlID = mlc.cControls;
		mds.cChannels = 1;
		mds.cMultipleItems = 0;
		mds.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		mds.paDetails = &vol;
		hRes = mixerGetControlDetails((HMIXEROBJ)m_hMixer, &mds, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		volume = vol.dwValue;
		return TRUE;
	}
	BOOL Mixer::SetVolume(DWORD volume)
	{
		ASSERT(m_hMixer);
		MIXERCONTROL mc;
		MIXERLINECONTROLS mlc;
		mlc.cbStruct = sizeof(MIXERLINECONTROLS);
		mlc.dwLineID = m_ml.dwLineID;
		mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		mlc.cControls = 1;
		mlc.cbmxctrl = sizeof(MIXERCONTROL);
		mlc.pamxctrl = &mc;
		MMRESULT hRes = mixerGetLineControls((HMIXEROBJ)m_hMixer, &mlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		MIXERCONTROLDETAILS_UNSIGNED vol;
		vol.dwValue = volume;
		MIXERCONTROLDETAILS mds;
		mds.cbStruct = sizeof(MIXERCONTROLDETAILS);
		mds.dwControlID = mlc.cControls;
		mds.cChannels = 1;
		mds.cMultipleItems = 0;
		mds.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
		mds.paDetails = &vol;
		hRes = mixerSetControlDetails((HMIXEROBJ)m_hMixer, &mds, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE);
		if (hRes != MMSYSERR_NOERROR)
			return FALSE;
		return TRUE;
	}
}