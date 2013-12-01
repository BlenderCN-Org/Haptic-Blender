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

/** \file gameengine/GameLogic/SCA_OmniSensor.cpp
 *  \ingroup gamelogic
 */

#include "SCA_OmniManager.h"
#include "SCA_OmniSensor.h"

#include "SCA_EventManager.h"
#include "SCA_LogicManager.h"

#include "PyObjectPlus.h"

#include <stdio.h>
#include <stddef.h>
#include <iostream>



SCA_OmniSensor::SCA_OmniSensor(class SCA_OmniManager* eventmgr,
									   SCA_IObject* gameobj,
									   short int omniindex,
									   short int joymode,
									   int axis, int axisf,int prec,
									   int button,
									   int hat, int hatf, bool allevents)
									   :SCA_ISensor(gameobj,eventmgr),
									   m_axis(axis),
									   m_axisf(axisf),
									   m_button(button),
									   m_hat(hat),
									   m_hatf(hatf),
									   m_precision(prec),
									   m_joymode(joymode),
									   m_omniindex(omniindex),
									   m_bAllEvents(allevents)
{
/*
std::cout << " axis "		<< m_axis		<< std::endl;
std::cout << " axis flag "	<< m_axisf		<< std::endl;
std::cout << " precision "	<< m_precision	<< std::endl;
std::cout << " button " 	<< m_button 	<< std::endl;
std::cout << " hat "		<< m_hat		<< std::endl;
std::cout << " hat flag "	<< m_hatf		<< std::endl;
*/
	Init();
}

void SCA_OmniSensor::Init()
{
	m_istrig=(m_invert)?1:0;
	m_istrig_prev=0;
	m_reset = true;
}

SCA_OmniSensor::~SCA_OmniSensor()
{
}


CValue* SCA_OmniSensor::GetReplica()
{
	SCA_OmniSensor* replica = new SCA_OmniSensor(*this);
	// this will copy properties and so on...
	replica->ProcessReplica();
	replica->Init();
	return replica;
}


bool SCA_OmniSensor::IsPositiveTrigger()
{ 
	bool result =	m_istrig;
	if (m_invert)
		result = !result;
	return result;
}


bool SCA_OmniSensor::Evaluate()
{
	SCA_Omni *js = ((SCA_OmniManager *)m_eventmgr)->GetOmniDevice(m_omniindex);
	bool result = false;
	bool reset = m_reset && m_level;
	
	if (js==NULL) /* no Omni - don't do anything */
		return false;
	
	m_reset = false;

	switch (m_joymode) {
		case KX_OMNISENSORMODE_AXIS:
		{
			/* what is what!
			 *  m_axisf == JOYAXIS_RIGHT, JOYAXIS_UP, JOYAXIS_DOWN, JOYAXIS_LEFT
			 *  m_axisf == 1 == up
			 *  m_axisf == 2 == left
			 *  m_axisf == 3 == down
			 *
			 *  numberof== m_axis (1-4), range is half of JOYAXIS_MAX since
			 *      it assumes the axis Omnis are axis parirs (0,1), (2,3), etc
			 *      also note that this starts at 1 where functions its used
			 *      with expect a zero index.
			 */
			
			if (!js->IsTrigAxis() && !reset) /* No events from SDL? - don't bother */
				return false;
			
			js->cSetPrecision(m_precision);
			if (m_bAllEvents) {
				if (js->aAxisPairIsPositive(m_axis-1)) { /* use zero based axis index internally */
					m_istrig = 1;
					result = true;
				}
				else {
					if (m_istrig) {
						m_istrig = 0;
						result = true;
					}
				}
			}
			else {
				if (js->aAxisPairDirectionIsPositive(m_axis-1, m_axisf)) { /* use zero based axis index internally */
					m_istrig = 1;
					result = true;
				}
				else {
					if (m_istrig) {
						m_istrig = 0;
						result = true;
					}
				}
			}
			break;
		}
		case KX_OMNISENSORMODE_AXIS_SINGLE:
		{
			/* Like KX_OMNISENSORMODE_AXIS but don't pair up axis */
			if (!js->IsTrigAxis() && !reset) /* No events from SDL? - don't bother */
				return false;
			
			/* No need for 'm_bAllEvents' check here since were only checking 1 axis */
			js->cSetPrecision(m_precision);
			if (js->aAxisIsPositive(m_axis-1)) { /* use zero based axis index internally */
				m_istrig = 1;
				result = true;
			}
			else {
				if (m_istrig) {
					m_istrig = 0;
					result = true;
				}
			}
			break;
		}

		case KX_OMNISENSORMODE_BUTTON:
		{
			/* what is what!
			 *  m_button = the actual button in question
			 */
			if (!js->IsTrigButton() && !reset) /* No events from SDL? - don't bother */
				return false;
			
			if (( m_bAllEvents && js->aAnyButtonPressIsPositive()) || (!m_bAllEvents && js->aButtonPressIsPositive(m_button))) {
				m_istrig = 1;
				result = true;
			}
			else {
				if (m_istrig) {
					m_istrig = 0;
					result = true;
				}
			}
			break;
		}
		case KX_OMNISENSORMODE_HAT:
		{
			/* what is what!
			 *  numberof = m_hat  -- max 4
			 *  direction= m_hatf -- max 12
			 */
			
			if (!js->IsTrigHat() && !reset) /* No events from SDL? - don't bother */
				return false;
			
			if ((m_bAllEvents && js->GetHat(m_hat-1)) || js->aHatIsPositive(m_hat-1, m_hatf)) {
				m_istrig = 1;
				result = true;
			}
			else {
				if (m_istrig) {
					m_istrig = 0;
					result = true;
				}
			}
			break;
		}
			/* test for ball anyone ?*/
		default:
			printf("Error invalid switch statement\n");
			break;
	}
	
	/* if not all events are enabled, only send a positive pulse when
	 * the button state changes */
	if (!m_bAllEvents) {
		if (m_istrig_prev == m_istrig) {
			result = false;
		}
		else {
			m_istrig_prev = m_istrig;
		}
	}
	
	if (reset)
		result = true;
	
	return result;
}


bool SCA_OmniSensor::isValid(SCA_OmniSensor::KX_OMNISENSORMODE m)
{
	bool res = false;
	res = ((m > KX_OMNISENSORMODE_NODEF) && (m < KX_OMNISENSORMODE_MAX));
	return res;
}

#ifdef WITH_PYTHON

/* ------------------------------------------------------------------------- */
/* Python functions 														 */
/* ------------------------------------------------------------------------- */

/* Integration hooks ------------------------------------------------------- */
PyTypeObject SCA_OmniSensor::Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"SCA_OmniSensor",
	sizeof(PyObjectPlus_Proxy),
	0,
	py_base_dealloc,
	0,
	0,
	0,
	0,
	py_base_repr,
	0,0,0,0,0,0,0,0,0,
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
	0,0,0,0,0,0,0,
	Methods,
	0,
	0,
	&SCA_ISensor::Type,
	0,0,0,0,0,0,
	py_base_new
};

PyMethodDef SCA_OmniSensor::Methods[] = {
	{"getButtonActiveList",(PyCFunction) SCA_OmniSensor::sPyGetButtonActiveList,	METH_NOARGS,(const char *)GetButtonActiveList_doc},
	{"getButtonStatus",(PyCFunction) SCA_OmniSensor::sPyGetButtonStatus,	METH_VARARGS,(const char *)GetButtonStatus_doc},
	{NULL,NULL} //Sentinel
};

PyAttributeDef SCA_OmniSensor::Attributes[] = {
	KX_PYATTRIBUTE_SHORT_RW("index",0,JOYINDEX_MAX-1,true,SCA_OmniSensor,m_omniindex),
	KX_PYATTRIBUTE_INT_RW("threshold",0,32768,true,SCA_OmniSensor,m_precision),
	KX_PYATTRIBUTE_INT_RW("button",0,100,false,SCA_OmniSensor,m_button),
	KX_PYATTRIBUTE_INT_LIST_RW_CHECK("axis",0,3,true,SCA_OmniSensor,m_axis,2,CheckAxis),
	KX_PYATTRIBUTE_INT_LIST_RW_CHECK("hat",0,12,true,SCA_OmniSensor,m_hat,2,CheckHat),
	KX_PYATTRIBUTE_RO_FUNCTION("axisValues",	SCA_OmniSensor, pyattr_get_axis_values),
	KX_PYATTRIBUTE_RO_FUNCTION("axisSingle", SCA_OmniSensor, pyattr_get_axis_single),
	KX_PYATTRIBUTE_RO_FUNCTION("hatValues",	SCA_OmniSensor, pyattr_get_hat_values),
	KX_PYATTRIBUTE_RO_FUNCTION("hatSingle", SCA_OmniSensor, pyattr_get_hat_single),
	KX_PYATTRIBUTE_RO_FUNCTION("numAxis",		SCA_OmniSensor, pyattr_get_num_axis),
	KX_PYATTRIBUTE_RO_FUNCTION("numButtons",	SCA_OmniSensor, pyattr_get_num_buttons),
	KX_PYATTRIBUTE_RO_FUNCTION("numHats",		SCA_OmniSensor, pyattr_get_num_hats),
	KX_PYATTRIBUTE_RO_FUNCTION("connected",		SCA_OmniSensor, pyattr_get_connected),
	{ NULL }	//Sentinel
};

/* get button active list  -------------------------------------------------- */
const char SCA_OmniSensor::GetButtonActiveList_doc[] = 
"getButtonActiveList\n"
"\tReturns a list containing the indices of the button currently pressed.\n";
PyObject *SCA_OmniSensor::PyGetButtonActiveList( )
{
	SCA_Omni *joy = ((SCA_OmniManager *)m_eventmgr)->GetOmniDevice(m_omniindex);
	PyObject *ls = PyList_New(0);
	PyObject *value;
	int i;
	
	if (joy) {
		for (i=0; i < joy->GetNumberOfButtons(); i++) {
			if (joy->aButtonPressIsPositive(i)) {
				value = PyLong_FromLong(i);
				PyList_Append(ls, value);
				Py_DECREF(value);
			}
		}
	}
	return ls;
}

/* get button status  -------------------------------------------------- */
const char SCA_OmniSensor::GetButtonStatus_doc[] = 
"getButtonStatus(buttonIndex)\n"
"\tReturns a bool of the current pressed state of the specified button.\n";
PyObject *SCA_OmniSensor::PyGetButtonStatus( PyObject *args )
{
	SCA_Omni *joy = ((SCA_OmniManager *)m_eventmgr)->GetOmniDevice(m_omniindex);
	int index;
	
	if (!PyArg_ParseTuple(args, "i:getButtonStatus", &index)) {
		return NULL;
	}
	if (joy && index >= 0 && index < joy->GetNumberOfButtons()) {
		return PyBool_FromLong(joy->aButtonPressIsPositive(index) ? 1 : 0);
	}
	return PyBool_FromLong(0);
}

PyObject *SCA_OmniSensor::pyattr_get_axis_values(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	
	int axis_index= joy->GetNumberOfAxes();
	PyObject *list= PyList_New(axis_index);
	
	while (axis_index--) {
		PyList_SET_ITEM(list, axis_index, PyLong_FromLong(joy->GetAxisPosition(axis_index)));
	}
	
	return list;
}

PyObject *SCA_OmniSensor::pyattr_get_axis_single(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	
	if (self->m_joymode != KX_OMNISENSORMODE_AXIS_SINGLE) {
		PyErr_SetString(PyExc_AttributeError, "val = sensor.axisSingle: Omni Sensor, not 'Single Axis' type");
		return NULL;
	}
	
	return PyLong_FromLong(joy->GetAxisPosition(self->m_axis-1));
}

PyObject *SCA_OmniSensor::pyattr_get_hat_values(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	
	int hat_index= joy->GetNumberOfHats();
	PyObject *list= PyList_New(hat_index);
	
	while (hat_index--) {
		PyList_SET_ITEM(list, hat_index, PyLong_FromLong(joy->GetHat(hat_index)));
	}
	
	return list;
}

PyObject *SCA_OmniSensor::pyattr_get_hat_single(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	
	return PyLong_FromLong(joy->GetHat(self->m_hat-1));
}

PyObject *SCA_OmniSensor::pyattr_get_num_axis(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	return PyLong_FromLong( joy ? joy->GetNumberOfAxes() : 0 );
}

PyObject *SCA_OmniSensor::pyattr_get_num_buttons(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	return PyLong_FromLong( joy ? joy->GetNumberOfButtons() : 0 );
}

PyObject *SCA_OmniSensor::pyattr_get_num_hats(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	return PyLong_FromLong( joy ? joy->GetNumberOfHats() : 0 );
}

PyObject *SCA_OmniSensor::pyattr_get_connected(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *joy = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	return PyBool_FromLong( joy ? joy->Connected() : 0 );
}

#endif
