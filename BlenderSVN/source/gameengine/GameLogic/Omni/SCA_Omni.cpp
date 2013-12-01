/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): snailrose.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/GameLogic/Omni/SCA_Omni.cpp
 *  \ingroup gamelogic
 */

#ifdef WITH_SDL
#  include <SDL.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "SCA_Omni.h"
#include "SCA_OmniPrivate.h"

#include "BLI_path_util.h"

SCA_Omni::SCA_Omni(short int index)
	:
	m_omniindex(index),
	m_prec(3200),
	m_axismax(-1),
	m_buttonmax(-1),
	m_hatmax(-1),
	m_isinit(0),
	m_istrig_axis(0),
	m_istrig_button(0),
	m_istrig_hat(0)
{
	for (int i=0; i < JOYAXIS_MAX; i++)
		m_axis_array[i] = 0;
	
	for (int i=0; i < JOYHAT_MAX; i++)
		m_hat_array[i] = 0;
	
#ifdef WITH_SDL
	m_private = new PrivateData();
#endif
}


SCA_Omni::~SCA_Omni()

{
#ifdef WITH_SDL
	delete m_private;
#endif
}

SCA_Omni *SCA_Omni::m_instance[JOYINDEX_MAX];
int SCA_Omni::m_joynum = 0;
int SCA_Omni::m_refCount = 0;

SCA_Omni *SCA_Omni::GetInstance( short int joyindex )
{
#ifndef WITH_SDL
	return NULL;
#else  /* WITH_SDL */
	if (joyindex < 0 || joyindex >= JOYINDEX_MAX) {
		Omni_ECHO("Error-invalid Omni index: " << joyindex);
		return NULL;
	}

	if (m_refCount == 0) 
	{
		int i;

		/* The video subsystem is required for Omni input to work. However,
		 * when GHOST is running under SDL, video is initialized elsewhere. We
		 * also need to set the videodriver to dummy, and do it here to avoid
		 * interfering with addons that may use SDL too.
		 *
		 * We also init SDL once only. */
#  ifdef WITH_GHOST_SDL
		int success = (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != -1 );
#  else
		/* set and restore environment variable */
		char *videodriver = getenv("SDL_VIDEODRIVER");
		BLI_setenv("SDL_VIDEODRIVER", "dummy");

		int success = (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) != -1 );

		BLI_setenv("SDL_VIDEODRIVER", videodriver);
#  endif

		if (!success) {
			Omni_ECHO("Error-Initializing-SDL: " << SDL_GetError());
			return NULL;
		}
		
		m_joynum = SDL_NumJoysticks();
		
		for (i=0; i<JOYINDEX_MAX; i++) {
			m_instance[i] = new SCA_Omni(i);
			m_instance[i]->CreateOmniDevice();
		}
		m_refCount = 1;
	}
	else
	{
		m_refCount++;
	}
	return m_instance[joyindex];
#endif /* WITH_SDL */
}

void SCA_Omni::ReleaseInstance()
{
	if (--m_refCount == 0)
	{
#ifdef WITH_SDL
		int i;
		for (i=0; i<JOYINDEX_MAX; i++) {
			if (m_instance[i]) {
				m_instance[i]->DestroyOmniDevice();
				delete m_instance[i];
			}
			m_instance[i] = NULL;
		}

		/* The video subsystem is required for Omni input to work. However,
		 * when GHOST is running under SDL, video is freed elsewhere.
		 * Do this once only. */
#  ifdef WITH_GHOST_SDL
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
#  else
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO);
#  endif
#endif /* WITH_SDL */
	}
}

void SCA_Omni::cSetPrecision(int val)
{
	m_prec = val;
}


bool SCA_Omni::aAxisPairIsPositive(int axis)
{
	return (pAxisTest(axis) > m_prec) ? true:false;
}

bool SCA_Omni::aAxisPairDirectionIsPositive(int axis, int dir)
{

	int res;

	if (dir==JOYAXIS_UP || dir==JOYAXIS_DOWN)
		res = pGetAxis(axis, 1);
	else /* JOYAXIS_LEFT || JOYAXIS_RIGHT */
		res = pGetAxis(axis, 0);
	
	if (dir==JOYAXIS_DOWN || dir==JOYAXIS_RIGHT)
		return (res > m_prec) ? true : false;
	else /* JOYAXIS_UP || JOYAXIS_LEFT */
		return (res < -m_prec) ? true : false;
}

bool SCA_Omni::aAxisIsPositive(int axis_single)
{
	return abs(m_axis_array[axis_single]) > m_prec ? true:false;
}

bool SCA_Omni::aAnyButtonPressIsPositive(void)
{
#ifdef WITH_SDL
	/* this is needed for the "all events" option
	 * so we know if there are no buttons pressed */
	for (int i=0; i<m_buttonmax; i++)
		if (SDL_JoystickGetButton(m_private->m_Omni, i))
			return true;
#endif
	return false;
}

bool SCA_Omni::aButtonPressIsPositive(int button)
{
#ifndef WITH_SDL
	return false;
#else
	bool result;
	SDL_JoystickGetButton(m_private->m_Omni, button)? result = true:result = false;
	return result;
#endif
}


bool SCA_Omni::aButtonReleaseIsPositive(int button)
{
#ifndef WITH_SDL
	return false;
#else
	bool result;
	SDL_JoystickGetButton(m_private->m_Omni, button)? result = false : result = true;
	return result;
#endif
}


bool SCA_Omni::aHatIsPositive(int hatnum, int dir)
{
	return (GetHat(hatnum)==dir) ? true : false;
}

int SCA_Omni::GetNumberOfAxes()
{
	return m_axismax;
}


int SCA_Omni::GetNumberOfButtons()
{
	return m_buttonmax;
}


int SCA_Omni::GetNumberOfHats()
{
	return m_hatmax;
}

bool SCA_Omni::CreateOmniDevice(void)
{
#ifndef WITH_SDL
	m_isinit = true;
	m_axismax = m_buttonmax = m_hatmax = 0;
	return false;
#else /* WITH_SDL */
	if (m_isinit == false) {
		if (m_omniindex>=m_joynum) {
			/* don't print a message, because this is done anyway */
			//Omni_ECHO("Omni-Error: " << SDL_NumOmnis() << " avaiable Omni(s)");
			
			/* Need this so python args can return empty lists */
			m_axismax = m_buttonmax = m_hatmax = 0;
			return false;
		}

		m_private->m_Omni = SDL_JoystickOpen(m_omniindex);
		SDL_JoystickEventState(SDL_ENABLE);
		m_isinit = true;
		
		Omni_ECHO("Omni " << m_joyindex << " initialized");
		
		/* must run after being initialized */
		m_axismax     = SDL_JoystickNumAxes(m_private->m_Omni);
		m_buttonmax   = SDL_JoystickNumButtons(m_private->m_Omni);
		m_hatmax      = SDL_JoystickNumHats(m_private->m_Omni);

		if      (m_axismax > JOYAXIS_MAX) m_axismax = JOYAXIS_MAX;  /* very unlikely */
		else if (m_axismax < 0)           m_axismax = 0;
		
		if      (m_hatmax > JOYHAT_MAX) m_hatmax = JOYHAT_MAX;  /* very unlikely */
		else if (m_hatmax < 0)          m_hatmax = 0;
		
		if (m_buttonmax < 0) m_buttonmax = 0;
		
	}
	return true;
#endif /* WITH_SDL */
}


void SCA_Omni::DestroyOmniDevice(void)
{
#ifdef WITH_SDL
	if (m_isinit) {
#if SDL_VERSION_ATLEAST(2, 0, 0)
		if (SDL_JoystickGetAttached(m_private->m_Omni)) {
#else
		if (SDL_JoystickOpened(m_omniindex)) {
#endif
			Omni_ECHO("Closing-Omni " << m_joyindex);
			SDL_JoystickClose(m_private->m_Omni);
		}
		m_isinit = false;
	}
#endif /* WITH_SDL */
}

int SCA_Omni::Connected(void)
{
#ifdef WITH_SDL
	if (m_isinit
#if SDL_VERSION_ATLEAST(2, 0, 0)
		&& SDL_JoystickGetAttached(m_private->m_Omni))
#else
	    && SDL_JoystickOpened(m_omniindex))
#endif
		return 1;
#endif
	return 0;
}

int SCA_Omni::pGetAxis(int axisnum, int udlr)
{
#ifdef WITH_SDL
	return m_axis_array[(axisnum*2)+udlr];
#endif
	return 0;
}

int SCA_Omni::pAxisTest(int axisnum)
{
#ifdef WITH_SDL
	/* Use ints instead of shorts here to avoid problems when we get -32768.
	 * When we take the negative of that later, we should get 32768, which is greater
	 * than what a short can hold. In other words, abs(MIN_SHORT) > MAX_SHRT. */
	int i1 = m_axis_array[(axisnum * 2)];
	int i2 = m_axis_array[(axisnum * 2) + 1];
	
	/* long winded way to do:
	 * return max_ff(absf(i1), absf(i2))
	 * ...avoid abs from math.h */
	if (i1 < 0) i1 = -i1;
	if (i2 < 0) i2 = -i2;
	if (i1 <i2) return i2;
	else        return i1;
#else /* WITH_SDL */
	return 0;
#endif /* WITH_SDL */
}

const char *SCA_Omni::GetName()
{
#ifdef WITH_SDL
#if SDL_VERSION_ATLEAST(2, 0, 0)
	return SDL_JoystickName(m_private->m_Omni);
#else
	return SDL_JoystickName(m_omniindex);
#endif
#else /* WITH_SDL */
	return "";
#endif /* WITH_SDL */
}
