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
	//this will copy properties and so on...
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
	SCA_Omni *os = ((SCA_OmniManager *)m_eventmgr)->GetOmniDevice(m_omniindex);
	bool result = false;
	bool reset = m_reset && m_level;
	
	if (os==NULL) /* Omni - don't do anything */
		return false;
	
	m_reset = false;

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
	{"setHapticObject",(PyCFunction) SCA_OmniSensor::sPySetHapticObject,	METH_VARARGS,(const char *)SetHapticObject_doc},
	{NULL,NULL} //Sentinel
};

PyAttributeDef SCA_OmniSensor::Attributes[] = {
	KX_PYATTRIBUTE_SHORT_RW("index",0,OMNIINDEX_MAX-1,true,SCA_OmniSensor,m_omniindex),
	KX_PYATTRIBUTE_INT_RW("threshold",0,32768,true,SCA_OmniSensor,m_precision),
	KX_PYATTRIBUTE_INT_RW("button",0,100,false,SCA_OmniSensor,m_button),
	KX_PYATTRIBUTE_RO_FUNCTION("connected",		SCA_OmniSensor, pyattr_get_connected),
	KX_PYATTRIBUTE_RO_FUNCTION("position",		SCA_OmniSensor, pyattr_get_position),
	{ NULL }	//Sentinel
};

const char SCA_OmniSensor::SetHapticObject_doc[] = 
"setHapticObject(triVertexList)\n"
"\tCreates a haptic object in the haptic renderer using a list of vertices using a file (x,y,z)\n";
PyObject *SCA_OmniSensor::PySetHapticObject( PyObject *args )
{
	SCA_Omni *omni = ((SCA_OmniManager *)m_eventmgr)->GetOmniDevice(m_omniindex);
	char * infile;
	if (!PyArg_ParseTuple(args, "s:setHapticObject", &infile)) {
	return NULL;
	}
	// turn args into array of vector3 
	printf("File should be: %s\n",infile);
	omni->SetHapticObject(infile);
	Py_RETURN_NONE;
}



PyObject *SCA_OmniSensor::pyattr_get_connected(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *omni = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);
	return PyBool_FromLong( omni ? omni->Connected() : 0 );
}

PyObject *SCA_OmniSensor::pyattr_get_position(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_OmniSensor* self = static_cast<SCA_OmniSensor*>(self_v);
	SCA_Omni *omni = ((SCA_OmniManager *)self->m_eventmgr)->GetOmniDevice(self->m_omniindex);


	float x_coord, y_coord, z_coord;
	Vec3 test = omni->getPosition();
	
	//Haptics device get position

	x_coord = test.x;
	y_coord = -test.z;//z is up and down
	z_coord = test.y;//y is back and forth
	PyObject *ret = PyTuple_New(3);

	PyTuple_SET_ITEM(ret, 0, PyFloat_FromDouble(x_coord));
	PyTuple_SET_ITEM(ret, 1, PyFloat_FromDouble(y_coord));
	PyTuple_SET_ITEM(ret, 2, PyFloat_FromDouble(z_coord));

	return ret;
}


#endif
