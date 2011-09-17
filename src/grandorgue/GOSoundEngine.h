/*
 * GrandOrgue - free pipe organ simulator based on MyOrgan
 *
 * MyOrgan 1.0.6 Codebase - Copyright 2006 Milan Digital Audio LLC
 * MyOrgan is a Trademark of Milan Digital Audio LLC
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef GOSOUNDENGINE_H_
#define GOSOUNDENGINE_H_

#include "GOrgueSoundTypes.h"
#include "GOSoundSamplerPool.h"
#include "GrandOrgueDef.h"
#include "wx/thread.h"
#include <vector>
#include <string.h>

class GOrgueWindchest;
class GrandOrgueFile;

typedef struct
{
	unsigned current_polyphony;
	double meter_left;
	double meter_right;
} METER_INFO;

typedef GO_SAMPLER* SAMPLER_HANDLE;

class GOSoundEngine
{

private:

	typedef int sound_buffer[(MAX_FRAME_SIZE + BLOCKS_PER_FRAME) * MAX_OUTPUT_CHANNELS];

	typedef struct
	{
		GO_SAMPLER      *base_sampler;
		GOrgueWindchest *windchest;
	} windchest_entry;

	typedef struct
	{
		GO_SAMPLER   *sampler;
		sound_buffer  buff;
	} tremulant_data;

	GO_SAMPLER                   *m_DetachedRelease;
	unsigned                      m_PolyphonySoftLimit;
	bool                          m_PolyphonyLimiting;
	bool                          m_ScaledReleases;
	bool                          m_ReleaseAlignmentEnabled;
	int                           m_Volume;
	unsigned long                 m_CurrentTime;
	GOSoundSamplerPool            m_SamplerPool;
	std::vector<windchest_entry>  m_Windchests;
	std::vector<tremulant_data>   m_Tremulants;

	void ProcessAudioSamplers
		(GO_SAMPLER** list_start
		,unsigned int n_frames
		,int* output_buffer
		);

	void MIDICallback
		(std::vector<unsigned char>& msg
		,int which
		);

	/* Per sampler decode buffer */
	int            m_TempDecodeBuffer[(MAX_FRAME_SIZE + BLOCKS_PER_FRAME) * MAX_OUTPUT_CHANNELS];
	double         m_FinalBuffer[MAX_FRAME_SIZE * MAX_OUTPUT_CHANNELS];
	float          m_VolumeBuffer[MAX_FRAME_SIZE * MAX_OUTPUT_CHANNELS];

	/* Per sampler list decode buffer */
	sound_buffer   m_TempSoundBuffer;

	/* samplerGroupID:
	   -1 .. -n Tremulants
	   0 detached release
	   1 .. n Windchests
	*/
	void StartSampler(GO_SAMPLER* sampler, int sampler_group_id);

public:

	GOSoundEngine();
	void Reset();
	void Setup(GrandOrgueFile* organ_file);
	void SetVolume(int volume);
	void SetHardPolyphony(unsigned polyphony);
	void SetPolyphonyLimiting(bool limiting);
	unsigned GetHardPolyphony() const;
	int GetVolume() const;
	void SetScaledReleases(bool enable);

	unsigned GetCurrentTime() const;
	SAMPLER_HANDLE StartSample(const GOrguePipe *pipe);
	void StopSample(const GOrguePipe *pipe, SAMPLER_HANDLE handle);

	int GetSamples
		(float      *output_buffer
		,unsigned    n_frames
		,double      stream_time
		,METER_INFO *meter_info
		);

};

#endif /* GOSOUNDENGINE_H_ */
