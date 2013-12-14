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
 * Contributor(s): Mitchell Stokes.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file gameengine/GameLogic/SCA_PythonOmni.cpp
 *  \ingroup gamelogic
 */


#include "SCA_PythonOmni.h"
#include "./Omni/SCA_Omni.h"
#include "SCA_IInputDevice.h"

//#include "GHOST_C-api.h"

/* ------------------------------------------------------------------------- */
/* Native functions                                                          */
/* ------------------------------------------------------------------------- */

SCA_PythonOmni::SCA_PythonOmni(SCA_Omni* Omni)
: PyObjectPlus(),
m_Omni(Omni)
{
#ifdef WITH_PYTHON
	m_event_dict = PyDict_New();
#endif
}

SCA_PythonOmni::~SCA_PythonOmni()
{
	// The Omni reference we got in the constructor was a new instance,
	// so we release it here
	m_Omni->ReleaseInstance();

#ifdef WITH_PYTHON
	PyDict_Clear(m_event_dict);
	Py_DECREF(m_event_dict);
#endif
}

#ifdef WITH_PYTHON

/* ------------------------------------------------------------------------- */
/* Python functions                                                          */
/* ------------------------------------------------------------------------- */
PyObject* SCA_PythonOmni::py_repr(void)
{
	return PyUnicode_FromString(m_Omni->GetName());
}


/* Integration hooks ------------------------------------------------------- */
PyTypeObject SCA_PythonOmni::Type = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"SCA_PythonOmni",
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
	&PyObjectPlus::Type,
	0,0,0,0,0,0,
	py_base_new
};

PyMethodDef SCA_PythonOmni::Methods[] = {
	{NULL,NULL} //Sentinel
};

/*
5 Rotations torques? (may not need, just get position of "end" position) 2 buttons one inkwell
*/
PyAttributeDef SCA_PythonOmni::Attributes[] = {

	KX_PYATTRIBUTE_RO_FUNCTION("name", SCA_PythonOmni, pyattr_get_name),
	{ NULL }	//Sentinel
};


PyObject* SCA_PythonOmni::pyattr_get_name(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_PythonOmni* self = static_cast<SCA_PythonOmni*>(self_v);

	return PyUnicode_FromString(self->m_Omni->GetName());
}

#endif
