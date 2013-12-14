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

/** \file SCA_Omni.h
 *  \ingroup gamelogic
 */

#ifndef __SCA_OMNI_H__
#define __SCA_OMNI_H__

#include "SCA_OmniDefines.h"
#ifdef WITH_SDL
#  include "SDL.h"
#endif

#ifdef WITH_H3D
#  include "HAPI/AnyHapticsDevice.h"
#  include <HAPI/GodObjectRenderer.h>
#endif

using namespace HAPI;

class SCA_Omni

{
	static SCA_Omni *m_instance[OMNIINDEX_MAX];
	static int m_omninum;
	static int m_refCount;

	//class PrivateData;
#ifdef WITH_H3D
	AnyHapticsDevice	m_Omni;
	//PrivateData		*m_private;
#endif
	int				m_omniindex;


	
	/**
	 * Precision or range of the axes
	 */
	int 			m_prec;


	
	/** is the Omni initialized ?*/
	bool			m_isinit;

	/**
	 * Open the Omni
	 */
	bool CreateOmniDevice(void);
	

	/**
	 * Close the Omni
	 */
	void DestroyOmniDevice(void);



	SCA_Omni(short int index);

	~SCA_Omni();
	
public:

	static SCA_Omni *GetInstance(short int omniindex);
	static void HandleEvents(void);
	void ReleaseInstance();
	void SetHapticObject(char * infile);
	



	/**
	 * precision is default '3200' which is overridden by input
	 */

	void cSetPrecision(int val);


	
		Vec3 getPosition(){//input some vector list
			return m_Omni.getPosition();
		}

	

	
	/**
	 * Test if the Omni is connected
	 */
	int Connected(void);

	/**
	 * Name of the joytsick
	 */
	const char *GetName();
};

#endif

