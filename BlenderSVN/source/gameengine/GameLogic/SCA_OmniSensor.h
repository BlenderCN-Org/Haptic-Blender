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

/** \file SCA_OmniSensor.h
 *  \ingroup gamelogic
 */


#ifndef __OMNISENSOR_H__
#define __OMNISENSOR_H__

#include "SCA_ISensor.h"
#include "Omni/SCA_OmniDefines.h"

class SCA_OmniSensor :public SCA_ISensor
{
	Py_Header
	
	/**
	 * Axis 1-JOYAXIS_MAX, MUST be followed by m_axisf
	 */
	int 	m_axis;
	/**
	 * Axis flag to find direction, MUST be an int
	 */
	int 	m_axisf;
	/**
	 * The actual button
	 */
	int 	m_button;
	/**
	 * Flag for a pressed or released button
	 */
	int 	m_buttonf;
	/**
	 * The actual hat 1-JOYHAT_MAX. MUST be followed by m_hatf
	 */
	int 	m_hat;
	/**
	 * Flag to find direction 1-12, MUST be an int
	 */
	int 	m_hatf;
	/**
	 * The threshold value the axis acts upon
	 */
	int 	m_precision;
	/**
	 * Is an event triggered ?
	 */
	bool	m_istrig;
	/**
	 * Last trigger state for this sensors Omni,
	 * Otherwise it will trigger all the time
	 * this is used to see if the trigger state changes.
	 */
	bool	m_istrig_prev;
	/**
	 * The mode to determine axis,button or hat
	 */
	short int m_joymode;
	/**
	 * Select which Omni to use
	 */
	short int m_omniindex;

	/**
	 * Detect all events for the currently selected type
	 */
	bool m_bAllEvents;

	enum KX_OMNISENSORMODE {
		KX_OMNISENSORMODE_NODEF = 0,
		KX_OMNISENSORMODE_AXIS,
		KX_OMNISENSORMODE_BUTTON,
		KX_OMNISENSORMODE_HAT,
		KX_OMNISENSORMODE_AXIS_SINGLE,
		KX_OMNISENSORMODE_MAX
	};
	bool isValid(KX_OMNISENSORMODE);

public:
	SCA_OmniSensor(class SCA_OmniManager* eventmgr,
					   SCA_IObject* gameobj,
					   short int omniindex,
					   short int joymode,
					   int axis, int axisf,int prec,
					   int button,
					   int hat, int hatf, bool allevents);
	virtual ~SCA_OmniSensor();
	virtual CValue* GetReplica();
	
	virtual bool Evaluate();
	virtual bool IsPositiveTrigger();
	virtual void Init();
	
	short int GetOmniIndex(void) {
		return m_omniindex;
	}

#ifdef WITH_PYTHON

	/* --------------------------------------------------------------------- */
	/* Python interface ---------------------------------------------------- */
	/* --------------------------------------------------------------------- */

	/* Omni Index */

	KX_PYMETHOD_DOC_VARARGS(SCA_OmniSensor,SetHapticObject)


	static PyObject*	pyattr_get_connected(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	static PyObject*	pyattr_get_position(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef);
	


	
#endif  /* WITH_PYTHON */

};

#endif  /* __OMNISENSOR_H__ */
