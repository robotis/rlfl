/*
	Python bindings to RLFL

    Copyright (C) 2011

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>

    <jtm@robot.is>
*/
#include <Python.h>
#include "headers/rlfl.h"

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyObject *RLFLError;
static void* RLFL_handle_error(err code, const char* generic);
/*
 +-----------------------------------------------------------+
 * @desc	Create new map
 * @return 	Map number
 +-----------------------------------------------------------+
 */
static PyObject*
create_map(PyObject *self, PyObject* args)
{
	int w, h;
	if(!PyArg_ParseTuple(args, "ii", &w, &h)) {
		return NULL;
	}

	/* Create map */
	int m = RLFL_new_map(w, h);

	if(m < 0) {
		if(m == RLFL_ERR_SIZE)
		{
			return RLFL_handle_error(m, "Invalid map size");
		}
		else if (m == RLFL_ERR_NO_MAP)
		{
			return RLFL_handle_error(m, "Too many maps");
		}
		else
		{
			return RLFL_handle_error(m, NULL);
		}
	}

	/* Success */
	return Py_BuildValue("i", m);
}
/*
 +-----------------------------------------------------------+
 * @desc	Get map width
 +-----------------------------------------------------------+
 */
static PyObject*
map_size(PyObject *self, PyObject* args)
{
	unsigned int m;
	if(!PyArg_ParseTuple(args, "i", &m)) {
		return NULL;
	}

	unsigned int w, h;
	int e = RLFL_map_size(m, &w, &h);
	if(w < 0) {
		return RLFL_handle_error(e, "");
	}

	return Py_BuildValue("(ii)", w, h);
}
/*
 +-----------------------------------------------------------+
 * @desc	Destroy all maps
 +-----------------------------------------------------------+
 */
static PyObject*
delete_all_maps(PyObject *self, PyObject* args) {
	RLFL_wipe_all();
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Destroy map
 * @param	int
 * @return  Bool
 +-----------------------------------------------------------+
 */
static PyObject*
delete_map(PyObject *self, PyObject* args) {
	unsigned int n;
	if(!PyArg_ParseTuple(args, "i", &n)) {
		return NULL;
	}

	if(RLFL_wipe_map(n) > -1) {
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Compute path map
 +-----------------------------------------------------------+
 */
static PyObject*
path_fill_map(PyObject *self, PyObject* args) {
	unsigned int m, x, y, s=0;
	float f = 0.0;
	if(!PyArg_ParseTuple(args, "i(ii)|fi", &m, &x, &y, &f, &s)) {
		return NULL;
	}
	int e = RLFL_path_fill_map(m, x, y, f, s);
	if(e < 0) {
		if(e == RLFL_ERR_NO_PATH)
			return RLFL_handle_error(e, "Unable to create pathmap: Too many maps");
		return RLFL_handle_error(e, NULL);
	}

	return Py_BuildValue("i", e);
}
/*
 +-----------------------------------------------------------+
 * @desc	Compute safety map
 +-----------------------------------------------------------+
 */
static PyObject*
path_fill_safety_map(PyObject *self, PyObject* args) {
	unsigned int m, x, y;
	float f = 0.0;
	if(!PyArg_ParseTuple(args, "i(ii)|f", &m, &x, &y, &f)) {
		return NULL;
	}
	int e = RLFL_path_fill_map(m, x, y, f, true);
	if(e < 0) {
		if(e == RLFL_ERR_NO_PATH)
			return RLFL_handle_error(e, "Unable to create pathmap: Too many maps");
		return RLFL_handle_error(e, NULL);
	}

	return Py_BuildValue("i", e);
}
/*
 +-----------------------------------------------------------+
 * @desc	Compute autoexplore map
 +-----------------------------------------------------------+
 */
static PyObject*
path_fill_autoexplore_map(PyObject *self, PyObject* args) {
	unsigned int m;
	unsigned long flg = 0;
	float f = 0.0;
	if(!PyArg_ParseTuple(args, "i|lf", &m, &flg, &f)) {
		return NULL;
	}
	int e = RLFL_path_fill_autoexplore_map(m, flg, f);
	if(e < 0) {
		if(e == RLFL_ERR_NO_PATH)
			return RLFL_handle_error(e, "Unable to create pathmap: Too many maps");
		return RLFL_handle_error(e, NULL);
	}

	return Py_BuildValue("i", e);
}
/*
 +-----------------------------------------------------------+
 * @desc	Compute custom map
 +-----------------------------------------------------------+
 */
static PyObject*
path_fill_custom_map(PyObject *self, PyObject* args) {
	unsigned int m;
	unsigned long flg = 0;
	float f = 0.0;
	if(!PyArg_ParseTuple(args, "i|lf", &m, &f, &flg)) {
		return NULL;
	}
	int e = RLFL_path_fill_custom_map(m, flg, f);
	if(e < 0) {
		if(e == RLFL_ERR_NO_PATH)
			return RLFL_handle_error(e, "Unable to create pathmap: Too many maps");
		return RLFL_handle_error(e, NULL);
	}

	return Py_BuildValue("i", e);
}
/*
 +-----------------------------------------------------------+
 * @desc	Compute path map
 +-----------------------------------------------------------+
 */
static PyObject*
path_step_map(PyObject *self, PyObject* args) {
	unsigned int m, p, x, y;
	if(!PyArg_ParseTuple(args, "ii(ii)", &m, &p, &x, &y)) {
		return NULL;
	}
	unsigned int nx, ny;
	int e = RLFL_path_step_map(m, p, x, y, &nx, &ny);
	if(e < 0) {
		if(e == RLFL_ERR_NO_PATH)
			return RLFL_handle_error(e, "Uninitialized pathmap used");
		if(e == RLFL_ERR_GENERIC)
			return RLFL_handle_error(e, "Found no path");
		return RLFL_handle_error(e, NULL);
	}
	return Py_BuildValue("(ii)", nx, ny);
}
/*
 +-----------------------------------------------------------+
 * @desc	Wipe path map
 +-----------------------------------------------------------+
 */
static PyObject*
path_clear_map(PyObject *self, PyObject* args) {
	unsigned int m, p;
	if(!PyArg_ParseTuple(args, "ii", &m, &p)) {
		return NULL;
	}
	int e = RLFL_path_wipe_map(m, p);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Wipe all path maps
 +-----------------------------------------------------------+
 */
static PyObject*
path_clear_all_maps(PyObject *self, PyObject* args) {
	unsigned int m;
	if(!PyArg_ParseTuple(args, "i", &m)) {
		return NULL;
	}
	int e = RLFL_path_wipe_all_maps(m);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Set flag
 +-----------------------------------------------------------+
 */
static PyObject*
set_flag(PyObject *self, PyObject* args) {
	unsigned int m, x, y;
	unsigned long flag;
	if(!PyArg_ParseTuple(args, "i(ii)l", &m, &x, &y, &flag)) {
		return NULL;
	}
	int e = RLFL_set_flag(m, x, y, flag);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Has flag
 +-----------------------------------------------------------+
 */
static PyObject*
has_flag(PyObject *self, PyObject* args) {
	unsigned int n, x, y;
	unsigned long flag;
	if(!PyArg_ParseTuple(args, "i(ii)l", &n, &x, &y, &flag)) {
		return NULL;
	}
	err e = RLFL_has_flag(n, x, y, flag);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	if(e) {
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Clear flag
 +-----------------------------------------------------------+
 */
static PyObject*
clear_flag(PyObject *self, PyObject* args) {
	unsigned int n, x, y;
	unsigned long flag;
	if(!PyArg_ParseTuple(args, "i(ii)l", &n, &x, &y, &flag)) {
		return NULL;
	}
	err e = RLFL_clear_flag(n, x, y, flag);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Get flags
 +-----------------------------------------------------------+
 */
static PyObject*
get_flags(PyObject *self, PyObject* args) {
	unsigned int m, x, y;
	if(!PyArg_ParseTuple(args, "i(ii)", &m, &x, &y)) {
		return NULL;
	}
	int flag = RLFL_get_flags(m, x, y);
	if(flag < 0) {
		return RLFL_handle_error(flag, NULL);
	}
	return Py_BuildValue("i", flag);
}
/*
 +-----------------------------------------------------------+
 * @desc	Clear map
 +-----------------------------------------------------------+
 */
static PyObject*
clear_map(PyObject *self, PyObject* args) {
	unsigned int n;
	unsigned long f = CELL_MASK;
	if(!PyArg_ParseTuple(args, "i|l", &n, &f)) {
		return NULL;
	}
	err e = RLFL_clear_map(n, f);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Fill map
 +-----------------------------------------------------------+
 */
static PyObject*
fill_map(PyObject *self, PyObject* args) {
	unsigned int n;
	unsigned long flag;
	if(!PyArg_ParseTuple(args, "il", &n, &flag)) {
		return NULL;
	}
	err e = RLFL_fill_map(n, flag);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Line of sight
 +-----------------------------------------------------------+
 */
static PyObject*
los(PyObject *self, PyObject* args) {
	unsigned int m, x1, y1, x2, y2;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)", &m, &x1, &y1, &x2, &y2)) {
		return NULL;
	}
	err e = RLFL_los(m, x1, y1, x2, y2);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	if(e) {
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Field of view
 +-----------------------------------------------------------+
 */
static PyObject*
fov(PyObject *self, PyObject* args) {
	unsigned int m, x, y, r, a;
	bool lw = true;
	if(!PyArg_ParseTuple(args, "i(ii)i|ii", &m, &x, &y, &r, &a, &lw)) {
		return NULL;
	}
	err e = RLFL_fov(m, x, y, r, a, lw);
	if(e < 0) {
		if(e == RLFL_ERR_GENERIC)
			return RLFL_handle_error(e, "Illegal radius");

		return RLFL_handle_error(e, NULL);
	}
	Py_RETURN_NONE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Line of sight
 +-----------------------------------------------------------+
 */
static PyObject*
distance(PyObject *self, PyObject* args) {
	unsigned int x1, y1, x2, y2;
	if(!PyArg_ParseTuple(args, "(ii)(ii)", &x1, &y1, &x2, &y2)) {
		return NULL;
	}
	int d = RLFL_distance(x1, y1, x2, y2);
	return Py_BuildValue("i", d);
}
/*
 +-----------------------------------------------------------+
 * @desc	Create path
 +-----------------------------------------------------------+
 */
static PyObject*
create_path(PyObject *self, PyObject* args) {
	unsigned int m, x1, y1, x2, y2, a = PATH_BASIC;
	unsigned long f = 0;
	int r = -1;
	float d = 10.0f;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)|iilf", &m, &x1, &y1, &x2, &y2, &a, &r, &f, &d)) {
		return NULL;
	}

	int p = RLFL_path_create(m, x1, y1, x2, y2, a, r, f, d);
	if(p < 0) {
		Py_RETURN_FALSE;
	}
	return Py_BuildValue("i", p);
}
/*
 +-----------------------------------------------------------+
 * @desc	Path size
 +-----------------------------------------------------------+
 */
static PyObject*
path_size(PyObject *self, PyObject* args) {
	unsigned int p;
	if(!PyArg_ParseTuple(args, "i", &p)) {
		return NULL;
	}
	err e = RLFL_path_size(p);
	if(e < 0) {
		Py_RETURN_FALSE;
	}
	return Py_BuildValue("i", e);
}
/*
 +-----------------------------------------------------------+
 * @desc	Path size
 +-----------------------------------------------------------+
 */
static PyObject*
delete_path(PyObject *self, PyObject* args) {
	unsigned int p;
	if(!PyArg_ParseTuple(args, "i", &p)) {
		return NULL;
	}
	err e = RLFL_path_delete(p);
	if(e < 0) {
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Retreive Path
 +-----------------------------------------------------------+
 */
static PyObject*
path_get(PyObject *self, PyObject* args) {
	unsigned int path;
	if(!PyArg_ParseTuple(args, "i", &path)) {
		return NULL;
	}
	int count = RLFL_path_size(path);
	if(count < 0) {
		Py_RETURN_FALSE;
	}
	PyObject *result = PyTuple_New(count);
	unsigned int i, x, y;
	for (i = 0; i < count; i++) {
		RLFL_path_step(path, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	return result;
}
static PyObject*
_path(unsigned int m, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
	  unsigned int a, int r, unsigned long f, float d)
{
	unsigned int i, x, y;
	int path = RLFL_path_create(m, x1, y1, x2, y2, a, r, f, d);

	if(path < 0)
	{
		if(path == RLFL_ERR_GENERIC)
		{
			// No path
			Py_RETURN_FALSE;
		}
		else
		{
			return RLFL_handle_error(path, NULL);
		}
	}
	int count = RLFL_path_size(path);
	if(count < 0) {
		return RLFL_handle_error(count, NULL);
	}
	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLFL_path_step(path, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLFL_path_delete(path);

	if(e < 0) {
		return RLFL_handle_error(e, "Path delete failed");
	}
	return result;
}
/*
 +-----------------------------------------------------------+
 * @desc	create and retreive Away Path
 +-----------------------------------------------------------+
 */
static PyObject*
path_away(PyObject *self, PyObject* args)
{
	unsigned int m, x1, y1, x2, y2, f = PROJECT_NONE, a = PATH_BASIC;
	int r = -1;
	float d = 10.0f;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)|iiif", &m, &x1, &y1, &x2,
												 &y2, &a, &r, &f, &d))
	{
		return NULL;
	}
	f |= PROJECT_REVR;
	return _path(m, x1, y1, x2, y2, a, r, f, d);
}
/*
 +-----------------------------------------------------------+
 * @desc	create and retreive Path
 +-----------------------------------------------------------+
 */
static PyObject*
path(PyObject *self, PyObject* args) {
	unsigned int m, x1, y1, x2, y2, f = PROJECT_NONE, a = PATH_BASIC;
	int r = -1;
	float d = 10.0f;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)|iiif", &m, &x1, &y1, &x2,
											     &y2, &a, &r, &f, &d))
	{
		return NULL;
	}
	return _path(m, x1, y1, x2, y2, a, r, f, d);
}
/*
 +-----------------------------------------------------------+
 * @desc	scatter
 +-----------------------------------------------------------+
 */
static PyObject*
scatter(PyObject *self, PyObject* args) {
	unsigned int m, ox, oy;
	int r = -1;
	bool los = true;
	unsigned long flag = 0;
	if(!PyArg_ParseTuple(args, "i(ii)|ili", &m, &ox, &oy, &r, &flag, &los)) {
		return NULL;
	}
	unsigned int dx, dy;
	err e = RLFL_scatter(m, ox, oy, &dx, &dy, r, flag, los);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	return Py_BuildValue("(ii)", dx, dy);
}
/*
 +-----------------------------------------------------------+
 * @desc	Beam
 +-----------------------------------------------------------+
 */
static PyObject*
project_beam(PyObject *self, PyObject* args) {
	unsigned int i, m, x, y, ox, oy, tx, ty, f = PROJECT_NONE;
	int range = -1;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)|ii", &m, &ox, &oy, &tx, &ty, &range, &f)) {
		return NULL;
	}
	err projection = RLFL_project_beam(m, ox, oy, tx, ty, range, f);
	if(projection < 0) {
		if(projection == RLFL_ERR_OUT_OF_BOUNDS)
		{
			if(!RLFL_cell_valid(m, ox, oy))
				return RLFL_handle_error(projection, "Projection failed: origin invalid");
			if(!RLFL_cell_valid(m, tx, ty))
				return RLFL_handle_error(projection, "Projection failed: destination invalid");
		}
		return RLFL_handle_error(projection, NULL);
	}

	int count = RLFL_project_size(projection);
	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLFL_project_step(projection, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLFL_project_delete(projection);
	if(e < 0) {
		return RLFL_handle_error(e, "Delete failed");
	}
	return result;
}
/*
 +-----------------------------------------------------------+
 * @desc	Ball
 +-----------------------------------------------------------+
 */
static PyObject*
project_ball(PyObject *self, PyObject* args) {
	unsigned int i, m, x, y, ox, oy, tx, ty, r, f;
	int range = -1;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)i|ii", &m, &ox, &oy, &tx, &ty, &r, &range, &f)) {
		return NULL;
	}
	err projection = RLFL_project_ball(m, ox, oy, tx, ty, r, range, f);
	if(projection < 0) {
		if(projection == RLFL_ERR_OUT_OF_BOUNDS)
		{
			if(!RLFL_cell_valid(m, ox, oy))
				return RLFL_handle_error(projection, "Projection failed: origin invalid");
			if(!RLFL_cell_valid(m, tx, ty))
				return RLFL_handle_error(projection, "Projection failed: destination invalid");
		}
		return RLFL_handle_error(projection, NULL);
	}

	int count = RLFL_project_size(projection);
	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLFL_project_step(projection, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLFL_project_delete(projection);
	if(e < 0) {
		return RLFL_handle_error(e, "Delete failed");
	}
	return result;
}
/*
 +-----------------------------------------------------------+
 * @desc	Breath
 +-----------------------------------------------------------+
 */
static PyObject*
project_cone(PyObject *self, PyObject* args) {
	unsigned int i, m, x, y, ox, oy, tx, ty, r, f;
	int range = -1;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)i|ii", &m, &ox, &oy, &tx, &ty, &r, &range, &f)) {
		return NULL;
	}
	err projection = RLFL_project_cone(m, ox, oy, tx, ty, r, range, f);
	if(projection < 0) {
		if(projection == RLFL_ERR_OUT_OF_BOUNDS)
		{
			if(!RLFL_cell_valid(m, ox, oy))
				return RLFL_handle_error(projection, "Projection failed: origin invalid");
			if(!RLFL_cell_valid(m, tx, ty))
				return RLFL_handle_error(projection, "Projection failed: destination invalid");
		}
		return RLFL_handle_error(projection, NULL);
	}

	int count = RLFL_project_size(projection);
	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLFL_project_step(projection, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLFL_project_delete(projection);
	if(e < 0) {
		return RLFL_handle_error(e, NULL);
	}
	return result;
}
/*
 +-----------------------------------------------------------+
 * @desc	Random int
 +-----------------------------------------------------------+
 */
static PyObject*
randint(PyObject *self, PyObject* args) {
	int max;
	if(!PyArg_ParseTuple(args, "i", &max)) {
		return NULL;
	}
	return Py_BuildValue("i", RLFL_randint(max));
}
/*
 +-----------------------------------------------------------+
 * @desc	Error handling
 +-----------------------------------------------------------+
 */
static void*
RLFL_handle_error(err code, const char* custom) {
	RLFLError = PyErr_NewException("rlfl.Error", NULL, NULL);
	if(custom)
	{
		PyErr_SetString(RLFLError, custom);
	}
	else
	{
		switch(code)
		{
			case RLFL_ERR_NO_MAP :
				PyErr_SetString(RLFLError, "Map not initialized");
				break;
			case RLFL_ERR_FLAG :
				PyErr_SetString(RLFLError, "Invalid flag used");
				break;
			case RLFL_ERR_OUT_OF_BOUNDS :
				PyErr_SetString(RLFLError, "Location out of bounds");
				break;
			case RLFL_ERR_SIZE :
				PyErr_SetString(RLFLError, "Map too large");
				break;
			case RLFL_ERR_NO_PATH :
				PyErr_SetString(RLFLError, "No path found");
				break;
			default :
				PyErr_SetString(RLFLError, "Generic Error -1");
				break;
		}
	}
	return NULL;
}
// ----------------------------------------------------------->
static PyMethodDef RLFLMethods[] =
{
	 {"create_map", create_map, METH_VARARGS, "Create new RLF map"},
	 {"delete_all_maps", delete_all_maps, METH_VARARGS, "Delete all maps"},
	 {"delete_map", delete_map, METH_VARARGS, "Delete RLE map"},
	 {"set_flag", set_flag, METH_VARARGS, "Set flag on cell"},
	 {"has_flag", has_flag, METH_VARARGS, "Query cell"},
	 {"get_flags", get_flags, METH_VARARGS, "Get flags"},
	 {"clear_flag", clear_flag, METH_VARARGS, "Clear flag on cell"},
	 {"clear_map", clear_map, METH_VARARGS, "Clear map"},
	 {"fill_map", fill_map, METH_VARARGS, "Fill map"},
	 {"path_fill_map", path_fill_map, METH_VARARGS, "Compute path map"},
	 {"path_fill_safety_map", path_fill_safety_map, METH_VARARGS, "Compute safety map"},
	 {"path_fill_autoexplore_map", path_fill_autoexplore_map, METH_VARARGS, "Compute autoexplore map"},
	 {"path_fill_custom_map", path_fill_custom_map, METH_VARARGS, "Compute custom map"},
	 {"path_step_map", path_step_map, METH_VARARGS, "Step on the path map"},
	 {"path_clear_map", path_clear_map, METH_VARARGS, "Clear the path map"},
	 {"path_clear_all_maps", path_clear_all_maps, METH_VARARGS, "Clear all path maps"},
	 {"los", los, METH_VARARGS, "Line of sight"},
	 {"fov", fov, METH_VARARGS, "Field of view"},
	 {"distance", distance, METH_VARARGS, "Distance between two points"},
	 {"create_path", create_path, METH_VARARGS, "New path"},
	 {"delete_path", delete_path, METH_VARARGS, "Delete path"},
	 {"path_size", path_size, METH_VARARGS, "Path size"},
	 {"path_get", path_get, METH_VARARGS, "Retrieve path"},
	 {"path", path, METH_VARARGS, "Create and retrive path"},
	 {"path_away", path_away, METH_VARARGS, "Create and retrive away path"},
	 {"scatter", scatter, METH_VARARGS, "Random spot in range and view"},
	 {"randint", randint, METH_VARARGS, "Random integer"},
	 {"project_beam", project_beam, METH_VARARGS, "Beam projection"},
	 {"project_ball", project_ball, METH_VARARGS, "Ball projection"},
	 {"project_cone", project_cone, METH_VARARGS, "Cone projection"},
	 {"map_size", map_size, METH_VARARGS, "(Width, Height) of map"},
     {NULL, NULL, 0, NULL}
};
#if PY_MAJOR_VERSION >= 3
static int rlfl_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int rlfl_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "rlfl",
        "RogueLike Function Library",
        sizeof(struct module_state),
        RLFLMethods,
        NULL,
        rlfl_traverse,
        rlfl_clear,
        NULL
};

#define INITERROR return NULL

PyObject *
PyInit_rlfl(void)

#else
#define INITERROR return

void
initrlfl(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
    PyObject *module = PyModule_Create(&moduledef);
#else
    PyObject *module = Py_InitModule("rlfl", RLFLMethods);
#endif

    if (module == NULL)
        INITERROR;
    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException("rlfl.Error", NULL, NULL);
    if (st->error == NULL) {
        Py_DECREF(module);
        INITERROR;
    }

    /*
     * RLF constants
     * */
    PyModule_AddIntConstant(module, "MAX_MAPS", 	RLFL_MAX_MAPS);

    /* Add flag constants */
    PyModule_AddIntConstant(module, "CELL_NONE", 	CELL_NONE);
    PyModule_AddIntConstant(module, "CELL_DARK", 	CELL_DARK);
    PyModule_AddIntConstant(module, "CELL_OPEN", 	CELL_OPEN);
    PyModule_AddIntConstant(module, "CELL_VIEW", 	CELL_VIEW);
    PyModule_AddIntConstant(module, "CELL_LIT", 	CELL_LIT);
    PyModule_AddIntConstant(module, "CELL_WALK", 	CELL_WALK);
    PyModule_AddIntConstant(module, "CELL_MEMO", 	CELL_MEMO);
    PyModule_AddIntConstant(module, "CELL_SEEN", 	CELL_SEEN);
    PyModule_AddIntConstant(module, "CELL_ROOM", 	CELL_ROOM);
    PyModule_AddIntConstant(module, "CELL_GLOW", 	CELL_GLOW);
    PyModule_AddIntConstant(module, "CELL_MASK", 	CELL_MASK);
    PyModule_AddIntConstant(module, "CELL_PATH", 	CELL_PATH);
    PyModule_AddIntConstant(module, "CELL_OCUP", 	CELL_OCUP);
    PyModule_AddIntConstant(module, "CELL_REFL", 	CELL_REFL);
    PyModule_AddIntConstant(module, "CELL_PERM", 	CELL_PERM);
    PyModule_AddIntConstant(module, "CELL_PASS", 	CELL_PASS);
    PyModule_AddIntConstant(module, "CELL_MARK", 	CELL_MARK);

    /* FOV algorithims */
    PyModule_AddIntConstant(module, "FOV_CIRCULAR", FOV_CIRCULAR);
    PyModule_AddIntConstant(module, "FOV_DIAMOND", 	FOV_DIAMOND);
    PyModule_AddIntConstant(module, "FOV_SHADOW", 	FOV_SHADOW);
    PyModule_AddIntConstant(module, "FOV_PERMISSIVE", FOV_PERMISSIVE);
    PyModule_AddIntConstant(module, "FOV_DIGITAL", 	FOV_DIGITAL);
    PyModule_AddIntConstant(module, "FOV_RESTRICTIVE", 	FOV_RESTRICTIVE);

    /* Path algorithims */
    PyModule_AddIntConstant(module, "PATH_ASTAR", 	PATH_ASTAR);
    PyModule_AddIntConstant(module, "PATH_BASIC", 	PATH_BASIC);

    /* Projections */
    PyModule_AddIntConstant(module, "PROJECT_THRU", PROJECT_THRU);
    PyModule_AddIntConstant(module, "PROJECT_STOP", PROJECT_STOP);
    PyModule_AddIntConstant(module, "PROJECT_PASS", PROJECT_PASS);
    PyModule_AddIntConstant(module, "PROJECT_SHEL", PROJECT_SHEL);
    PyModule_AddIntConstant(module, "PROJECT_DIAMOND", PROJECT_DIAMOND);
    PyModule_AddIntConstant(module, "PROJECT_SQUARE", PROJECT_SQUARE);

    /* Various */
    PyModule_AddIntConstant(module, "MAX_MAPS", 	RLFL_MAX_MAPS);
    PyModule_AddIntConstant(module, "MAX_PATHS", 	RLFL_MAX_PATHS);
    PyModule_AddIntConstant(module, "MAX_PROJECTS", 	RLFL_MAX_PROJECTS);
    PyModule_AddIntConstant(module, "MAX_RANGE", 	RLFL_MAX_RANGE);
    PyModule_AddIntConstant(module, "MAX_RADIUS", 	RLFL_MAX_RADIUS);
    PyModule_AddIntConstant(module, "MAX_WIDTH", 	RLFL_MAX_WIDTH);
    PyModule_AddIntConstant(module, "MAX_HEIGHT", 	RLFL_MAX_HEIGHT);

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
