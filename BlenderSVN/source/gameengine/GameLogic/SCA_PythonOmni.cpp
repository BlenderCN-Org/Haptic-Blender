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

PyAttributeDef SCA_PythonOmni::Attributes[] = {
	KX_PYATTRIBUTE_RO_FUNCTION("numButtons", SCA_PythonOmni, pyattr_get_num_x),
	KX_PYATTRIBUTE_RO_FUNCTION("numHats", SCA_PythonOmni, pyattr_get_num_x),
	KX_PYATTRIBUTE_RO_FUNCTION("numAxis", SCA_PythonOmni, pyattr_get_num_x),
	KX_PYATTRIBUTE_RO_FUNCTION("activeButtons", SCA_PythonOmni, pyattr_get_active_buttons),
	KX_PYATTRIBUTE_RO_FUNCTION("hatValues", SCA_PythonOmni, pyattr_get_hat_values),
	KX_PYATTRIBUTE_RO_FUNCTION("axisValues", SCA_PythonOmni, pyattr_get_axis_values),
	KX_PYATTRIBUTE_RO_FUNCTION("name", SCA_PythonOmni, pyattr_get_name),
	{ NULL }	//Sentinel
};

// Use one function for numAxis, numButtons, and numHats
PyObject* SCA_PythonOmni::pyattr_get_num_x(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_PythonOmni* self = static_cast<SCA_PythonOmni*>(self_v);

	if (strcmp(attrdef->m_name, "numButtons") == 0)
		return PyLong_FromLong(self->m_Omni->GetNumberOfButtons());
	else if (strcmp(attrdef->m_name, "numAxis") == 0)
		return PyLong_FromLong(self->m_Omni->GetNumberOfAxes());
	else if (strcmp(attrdef->m_name, "numHats") == 0)
		return PyLong_FromLong(self->m_Omni->GetNumberOfHats());

	// If we got here, we have a problem...
	PyErr_SetString(PyExc_AttributeError, "invalid attribute");
	return NULL;
}

PyObject* SCA_PythonOmni::pyattr_get_active_buttons(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_PythonOmni* self = static_cast<SCA_PythonOmni*>(self_v);
	
	const int button_number = self->m_Omni->GetNumberOfButtons();

	PyObject *list = PyList_New(0);
	PyObject *value;

	for (int i=0; i < button_number; i++) {
		if (self->m_Omni->aButtonPressIsPositive(i)) {
			value = PyLong_FromLong(i);
			PyList_Append(list, value);
			Py_DECREF(value);
		}
	}

	return list;
}

PyObject* SCA_PythonOmni::pyattr_get_hat_values(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_PythonOmni* self = static_cast<SCA_PythonOmni*>(self_v);
	
	int hat_index = self->m_Omni->GetNumberOfHats();
	PyObject *list = PyList_New(hat_index);
	
	while (hat_index--) {
		PyList_SET_ITEM(list, hat_index, PyLong_FromLong(self->m_Omni->GetHat(hat_index)));
	}
	
	return list;
}

PyObject* SCA_PythonOmni::pyattr_get_axis_values(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_PythonOmni* self = static_cast<SCA_PythonOmni*>(self_v);
	
	int axis_index = self->m_Omni->GetNumberOfAxes();
	PyObject *list = PyList_New(axis_index);
	int position;
	
	while (axis_index--) {
		position = self->m_Omni->GetAxisPosition(axis_index);

		// We get back a range from -32768 to 32767, so we use an if here to
		// get a perfect -1.0 to 1.0 mapping. Some oddball system might have an
		// actual min of -32767 for shorts, so we use SHRT_MIN/MAX to be safe.
		if (position < 0)
			PyList_SET_ITEM(list, axis_index, PyFloat_FromDouble(position/((double)-SHRT_MIN)));
		else
			PyList_SET_ITEM(list, axis_index, PyFloat_FromDouble(position/(double)SHRT_MAX));
	}
	
	return list;
}

PyObject* SCA_PythonOmni::pyattr_get_name(void *self_v, const KX_PYATTRIBUTE_DEF *attrdef)
{
	SCA_PythonOmni* self = static_cast<SCA_PythonOmni*>(self_v);

	return PyUnicode_FromString(self->m_Omni->GetName());
}
#endif
