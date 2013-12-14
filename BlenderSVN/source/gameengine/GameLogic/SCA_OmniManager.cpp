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
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/GameLogic/SCA_OmniManager.cpp
 *  \ingroup gamelogic
 */

#include "SCA_OmniSensor.h"
#include "SCA_OmniManager.h"
#include "SCA_LogicManager.h"
//#include <vector>
#include "SCA_ISensor.h"

//using namespace std;


SCA_OmniManager::SCA_OmniManager(class SCA_LogicManager* logicmgr)
	: SCA_EventManager(logicmgr, OMNI_EVENTMGR)
{
	int i;
	for (i=0; i<1; i++) {//OMNIINDEX_MAX
		m_Omni[i] = SCA_Omni::GetInstance( i );
	}
}


SCA_OmniManager::~SCA_OmniManager()
{
	int i;
	for (i=0; i<1; i++) {
		if (m_Omni[i])
			m_Omni[i]->ReleaseInstance();
	}
}


void SCA_OmniManager::NextFrame(double curtime,double deltatime)
{
	// We should always handle events in case we want to grab them with Python
#ifdef WITH_SDL
	//SCA_Omni::HandleEvents(); /* Handle all Omni events */
#endif

	if (m_sensors.Empty()) {
		return;
	}
	else {
		SG_DList::iterator<SCA_OmniSensor> it(m_sensors);
		for (it.begin();!it.end();++it)
		{
			SCA_OmniSensor* omnisensor = *it;
			if (!omnisensor->IsSuspended())
			{
				omnisensor->Activate(m_logicmgr);
			}
		}
	}
}


SCA_Omni *SCA_OmniManager::GetOmniDevice( short int omniindex)
{
	/*
	 *Return the instance of SCA_Omni for use
	 */
	return m_Omni[0];
}
