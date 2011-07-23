/*
 +-----------------------------------------------------------+
 * @desc	Python bindings to RLF
 * @file	rlftopy.c
 * @package RLF
 * @license GPL3
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include <Python.h>
#include <unicodeobject.h>
#include <bytesobject.h>
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
 * @desc	Set flag
 +-----------------------------------------------------------+
 */
static PyObject*
set_flag(PyObject *self, PyObject* args) {
	unsigned int m, x, y, flag;
	if(!PyArg_ParseTuple(args, "i(ii)i", &m, &x, &y, &flag)) {
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
	unsigned int n, x, y, flag;
	if(!PyArg_ParseTuple(args, "i(ii)i", &n, &x, &y, &flag)) {
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
	unsigned int n, x, y, flag;
	if(!PyArg_ParseTuple(args, "i(ii)i", &n, &x, &y, &flag)) {
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
	unsigned int n, f;
	f = CELL_MASK;
	if(!PyArg_ParseTuple(args, "i|i", &n, &f)) {
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
	unsigned int n, flag;
	if(!PyArg_ParseTuple(args, "ii", &n, &flag)) {
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
		return RLFL_handle_error(e, "LOS failed");
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
	if(!PyArg_ParseTuple(args, "i(ii)|iii", &m, &x, &y, &r, &a, &lw)) {
		return NULL;
	}
	err e = RLFL_fov(m, x, y, r, a, lw);
	if(e < 0) {
		return RLFL_handle_error(e, "Fov failed");
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
	unsigned int m, x1, y1, x2, y2, f, a = PATH_BASIC;
	int r = -1;
	float d = 10.0f;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)|iif", &m, &x1, &y1, &x2, &y2, &a, &r, &f, &d)) {
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
/*
 +-----------------------------------------------------------+
 * @desc	create and retreive Path
 +-----------------------------------------------------------+
 */
static PyObject*
path(PyObject *self, PyObject* args) {
	unsigned int m, x1, y1, x2, y2, f = PROJECT_NONE, a = PATH_BASIC;
	unsigned int i, x, y;
	int r = -1;
	float d = 10.0f;
	if(!PyArg_ParseTuple(args, "i(ii)(ii)|iiif", &m, &x1, &y1, &x2,
											     &y2, &a, &r, &f, &d)) {
		return NULL;
	}

	int path = RLFL_path_create(m, x1, y1, x2, y2, a, r, f, d);
	if(path < 0) {
		// No path
		Py_RETURN_FALSE;
	}
	int count = RLFL_path_size(path);

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
 * @desc	scatter
 +-----------------------------------------------------------+
 */
static PyObject*
scatter(PyObject *self, PyObject* args) {
	int m, ox, oy, r;
	bool los = true;
	unsigned int flag = 0;
	if(!PyArg_ParseTuple(args, "i(ii)i|ii", &m, &ox, &oy, &r, &flag, &los)) {
		return NULL;
	}
	unsigned int dx, dy;
	err e = RLFL_scatter(m, ox, oy, &dx, &dy, r, flag, los);
	if(e < 0) {
		return RLFL_handle_error(e, "Scatter failed");
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
		if(!RLFL_cell_valid(m, ox, oy))
			return RLFL_handle_error(projection, "Projection failed: origin invalid");
		if(!RLFL_cell_valid(m, tx, ty))
			return RLFL_handle_error(projection, "Projection failed: destination invalid");
		return RLFL_handle_error(projection, "Projection failed");
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
		return RLFL_handle_error(projection, "Projection failed");
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
	 {"los", los, METH_VARARGS, "Line of sight"},
	 {"fov", fov, METH_VARARGS, "Field of view"},
	 {"distance", distance, METH_VARARGS, "Distance between two points"},
	 {"create_path", create_path, METH_VARARGS, "New path"},
	 {"delete_path", delete_path, METH_VARARGS, "Delete path"},
	 {"path_size", path_size, METH_VARARGS, "Path size"},
	 {"path_get", path_get, METH_VARARGS, "Retrieve path"},
	 {"path", path, METH_VARARGS, "Create and retrive path"},
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

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
