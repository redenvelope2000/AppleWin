/*
AppleWin : An Apple //e emulator for Windows

Copyright (C) 1994-1996, Michael O'Brien
Copyright (C) 1999-2001, Oliver Schmidt
Copyright (C) 2002-2005, Tom Charlesworth
Copyright (C) 2006-2022, Tom Charlesworth, Michael Pohoreski, Nick Westgate

AppleWin is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

AppleWin is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with AppleWin; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* Description: Mockingboard/Phasor Card Manager
 *
 * Author: Various
 *
 */

#include "StdAfx.h"

#include "MockingboardCardManager.h"
#include "Core.h"
#include "CardManager.h"
#include "Mockingboard.h"

bool MockingboardCardManager::IsMockingboard(UINT slot)
{
	SS_CARDTYPE type = GetCardMgr().QuerySlot(slot);
	return type == CT_MockingboardC || type == CT_Phasor;
}

void MockingboardCardManager::InitializePostWindowCreate(void)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).MB_DSInit();
	}
}

void MockingboardCardManager::ReinitializeClock(void)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).ReinitializeClock();
	}
}

void MockingboardCardManager::InitializeForLoadingSnapshot(void)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).InitializeForLoadingSnapshot();
	}
}

void MockingboardCardManager::MuteControl(bool mute)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).MuteControl(mute);
	}
}

void MockingboardCardManager::SetCumulativeCycles(void)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).SetCumulativeCycles();
	}
}

void MockingboardCardManager::UpdateCycles(ULONG executedCycles)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).UpdateCycles(executedCycles);
	}
}

bool MockingboardCardManager::IsActive(void)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			if (dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).IsActive())
				return true;	// if any card is true then the condition for active is true
	}

	return false;
}

DWORD MockingboardCardManager::GetVolume(void)
{
	return m_userVolume;
}

void MockingboardCardManager::SetVolume(DWORD volume, DWORD volumeMax)
{
	m_userVolume = volume;

	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).SetVolume(volume, volumeMax);
	}
}

#ifdef _DEBUG
void MockingboardCardManager::CheckCumulativeCycles(void)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).CheckCumulativeCycles();
	}
}

void MockingboardCardManager::Get6522IrqDescription(std::string& desc)
{
	for (UINT i = SLOT0; i < NUM_SLOTS; i++)
	{
		if (IsMockingboard(i))
			dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(i)).Get6522IrqDescription(desc);
	}
}
#endif

void MockingboardCardManager::Update(void)
{
	if (!MockingboardVoice.lpDSBvoice)
		Init();

	UINT numSamples = MB_UpdateInternal1();
	if (numSamples)
		MB_UpdateInternal2(numSamples);
}

bool MockingboardCardManager::Init(void)
{
	const DWORD SAMPLE_RATE = 44100;	// Use a base freq so that DirectX (or sound h/w) doesn't have to up/down-sample

	HRESULT hr = DSGetSoundBuffer(&MockingboardVoice, DSBCAPS_CTRLVOLUME, g_dwDSBufferSize, SAMPLE_RATE, g_nMB_NumChannels, "MB");
	LogFileOutput("MB_DSInit: DSGetSoundBuffer(), hr=0x%08X\n", hr);
	if (FAILED(hr))
	{
		LogFileOutput("MB_DSInit: DSGetSoundBuffer failed (%08X)\n", hr);
		return false;
	}

	bool bRes = DSZeroVoiceBuffer(&MockingboardVoice, g_dwDSBufferSize);	// ... and Play()
	LogFileOutput("MB_DSInit: DSZeroVoiceBuffer(), res=%d\n", bRes ? 1 : 0);
	if (!bRes)
		return false;

	MockingboardVoice.bActive = true;

	// Volume might've been setup from value in Registry
	if (!MockingboardVoice.nVolume)
		MockingboardVoice.nVolume = DSBVOLUME_MAX;

	hr = MockingboardVoice.lpDSBvoice->SetVolume(MockingboardVoice.nVolume);
	LogFileOutput("MB_DSInit: SetVolume(), hr=0x%08X\n", hr);
	return true;
}

UINT MockingboardCardManager::MB_UpdateInternal1(void)
{
	UINT nNumSamples = 0;

	UINT numSamples0 = (UINT)-1;
	int numSamplesError0 = -1;
	int numSamplesErrorPre = -1;

	for (UINT slot = SLOT0; slot < NUM_SLOTS; slot++)
	{
		if (!IsMockingboard(slot))
			continue;

		MockingboardCard& MB = dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(slot));

		if (numSamplesErrorPre == -1)
			numSamplesErrorPre = nNumSamplesError;

		MB.SetNumSamplesError(nNumSamplesError);
		nNumSamples = MB.MB_UpdateInternal1();
		nNumSamplesError = MB.GetNumSamplesError();

#if 1 // debug
		if (numSamples0 == (UINT)-1)
		{
			numSamples0 = nNumSamples;
			numSamplesError0 = nNumSamplesError;
		}

		_ASSERT(numSamples0 == nNumSamples);
		_ASSERT(numSamplesError0 == nNumSamplesError);
#endif
	}

	//

	DWORD dwCurrentPlayCursor, dwCurrentWriteCursor;
	HRESULT hr = MockingboardVoice.lpDSBvoice->GetCurrentPosition(&dwCurrentPlayCursor, &dwCurrentWriteCursor);
	if (FAILED(hr))
		return 0;

	//static DWORD dwByteOffset = (DWORD)-1;	// Moved to class
	if (dwByteOffset == (DWORD)-1)
	{
		// First time in this func

		dwByteOffset = dwCurrentWriteCursor;
	}
	else
	{
		// Check that our offset isn't between Play & Write positions

		if (dwCurrentWriteCursor > dwCurrentPlayCursor)
		{
			// |-----PxxxxxW-----|
			if ((dwByteOffset > dwCurrentPlayCursor) && (dwByteOffset < dwCurrentWriteCursor))
			{
#ifdef DBG_MB_UPDATE
				double fTicksSecs = (double)GetTickCount() / 1000.0;
				LogOutput("%010.3f: [MBUpdt]    PC=%08X, WC=%08X, Diff=%08X, Off=%08X, NS=%08X xxx\n", fTicksSecs, dwCurrentPlayCursor, dwCurrentWriteCursor, dwCurrentWriteCursor - dwCurrentPlayCursor, dwByteOffset, nNumSamples);
#endif
				dwByteOffset = dwCurrentWriteCursor;
				nNumSamplesError = 0;
			}
		}
		else
		{
			// |xxW----------Pxxx|
			if ((dwByteOffset > dwCurrentPlayCursor) || (dwByteOffset < dwCurrentWriteCursor))
			{
#ifdef DBG_MB_UPDATE
				double fTicksSecs = (double)GetTickCount() / 1000.0;
				LogOutput("%010.3f: [MBUpdt]    PC=%08X, WC=%08X, Diff=%08X, Off=%08X, NS=%08X XXX\n", fTicksSecs, dwCurrentPlayCursor, dwCurrentWriteCursor, dwCurrentWriteCursor - dwCurrentPlayCursor, dwByteOffset, nNumSamples);
#endif
				dwByteOffset = dwCurrentWriteCursor;
				nNumSamplesError = 0;
			}
		}
	}

	int nBytesRemaining = dwByteOffset - dwCurrentPlayCursor;
	if (nBytesRemaining < 0)
		nBytesRemaining += g_dwDSBufferSize;

	// Calc correction factor so that play-buffer doesn't under/overflow
	const int nErrorInc = SoundCore_GetErrorInc();
	if (nBytesRemaining < g_dwDSBufferSize / 4)
		nNumSamplesError += nErrorInc;				// < 0.25 of buffer remaining
	else if (nBytesRemaining > g_dwDSBufferSize / 2)
		nNumSamplesError -= nErrorInc;				// > 0.50 of buffer remaining
	else
		nNumSamplesError = 0;						// Acceptable amount of data in buffer

#ifdef DBG_MB_UPDATE
	double fTicksSecs = (double)GetTickCount() / 1000.0;
	LogOutput("%010.3f: [MBUpdt]    PC=%08X, WC=%08X, Diff=%08X, Off=%08X, NS=%08X, NSE=%08X, Interval=%f\n", fTicksSecs, dwCurrentPlayCursor, dwCurrentWriteCursor, dwCurrentWriteCursor - dwCurrentPlayCursor, dwByteOffset, nNumSamples, nNumSamplesError, updateInterval);
#endif

	return nNumSamples;
}

void MockingboardCardManager::MB_UpdateInternal2(int nNumSamples)
{
//	const double fAttenuation = g_bPhasorEnable ? 2.0 / 3.0 : 1.0;
	const double fAttenuation = true ? 2.0 / 3.0 : 1.0;

	short** slotAYVoiceBuffers[NUM_SLOTS] = {0};

	for (UINT slot = SLOT0; slot < NUM_SLOTS; slot++)
	{
		if (IsMockingboard(slot))
			slotAYVoiceBuffers[slot] = dynamic_cast<MockingboardCard&>(GetCardMgr().GetRef(slot)).GetVoiceBuffers();
	}

	for (int i = 0; i < nNumSamples; i++)
	{
		// Mockingboard stereo (all voices on an AY8910 wire-or'ed together)
		// L = Address.b7=0, R = Address.b7=1
		int nDataL = 0, nDataR = 0;

		for (UINT j = 0; j < NUM_VOICES_PER_AY8913; j++)
		{
			for (UINT slot = SLOT0; slot < NUM_SLOTS; slot++)
			{
				if (!slotAYVoiceBuffers[slot])
					continue;

				short** ppAYVoiceBuffer = slotAYVoiceBuffers[slot];

				// Regular MB-C AY's
				nDataL += (int)((double)ppAYVoiceBuffer[0 * NUM_VOICES_PER_AY8913 + j][i] * fAttenuation);
				nDataR += (int)((double)ppAYVoiceBuffer[1 * NUM_VOICES_PER_AY8913 + j][i] * fAttenuation);

				// Extra Phasor AY's
				nDataL += (int)((double)ppAYVoiceBuffer[2 * NUM_VOICES_PER_AY8913 + j][i] * fAttenuation);
				nDataR += (int)((double)ppAYVoiceBuffer[3 * NUM_VOICES_PER_AY8913 + j][i] * fAttenuation);
			}
		}

		// Cap the superpositioned output
		if (nDataL < nWaveDataMin)
			nDataL = nWaveDataMin;
		else if (nDataL > nWaveDataMax)
			nDataL = nWaveDataMax;

		if (nDataR < nWaveDataMin)
			nDataR = nWaveDataMin;
		else if (nDataR > nWaveDataMax)
			nDataR = nWaveDataMax;

		g_nMixBuffer[i * g_nMB_NumChannels + 0] = (short)nDataL;	// L
		g_nMixBuffer[i * g_nMB_NumChannels + 1] = (short)nDataR;	// R
	}

	//

	DWORD dwDSLockedBufferSize0, dwDSLockedBufferSize1;
	SHORT* pDSLockedBuffer0, * pDSLockedBuffer1;

	HRESULT hr = DSGetLock(MockingboardVoice.lpDSBvoice,
		dwByteOffset, (DWORD)nNumSamples * sizeof(short) * g_nMB_NumChannels,
		&pDSLockedBuffer0, &dwDSLockedBufferSize0,
		&pDSLockedBuffer1, &dwDSLockedBufferSize1);
	if (FAILED(hr))
		return;

	memcpy(pDSLockedBuffer0, &g_nMixBuffer[0], dwDSLockedBufferSize0);
	if (pDSLockedBuffer1)
		memcpy(pDSLockedBuffer1, &g_nMixBuffer[dwDSLockedBufferSize0 / sizeof(short)], dwDSLockedBufferSize1);

	// Commit sound buffer
	hr = MockingboardVoice.lpDSBvoice->Unlock((void*)pDSLockedBuffer0, dwDSLockedBufferSize0,
		(void*)pDSLockedBuffer1, dwDSLockedBufferSize1);

	dwByteOffset = (dwByteOffset + (DWORD)nNumSamples * sizeof(short) * g_nMB_NumChannels) % g_dwDSBufferSize;

#ifdef RIFF_MB
	RiffPutSamples(&g_nMixBuffer[0], nNumSamples);
#endif
}
