/*
 +-----------------------------------------------------------+
 * @desc	Python bindings to RLF
 * @file	rlftopy.c
 * @package RLF
 * @license FIXME
 * <jtm@robot.is>
 +-----------------------------------------------------------+
 */
#include <Python.h>
#include <unicodeobject.h>
#include <bytesobject.h>
#include "headers/rlf.h"

struct module_state {
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

static PyObject *RLFError;
static void* RLF_handle_error(err code, const char* generic);
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
	PyArg_ParseTuple(args, "ii", &w, &h);

	if(w <= 0 || h <= 0)
		Py_RETURN_FALSE;

	/* Create map */
	int m = RLF_new_map(w, h);

	if(m < 0)
		Py_RETURN_FALSE;
	/* Success */
	return Py_BuildValue("i", m);
}
/*
 +-----------------------------------------------------------+
 * @desc	Destroy all maps
 +-----------------------------------------------------------+
 */
static PyObject*
delete_all_maps(PyObject *self, PyObject* args) {
	RLF_wipe_all();
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
	PyArg_ParseTuple(args, "i", &n);

	if(RLF_wipe_map(n) > -1) {
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
	PyArg_ParseTuple(args, "iiii", &m, &x, &y, &flag);
	int e = RLF_set_flag(m, x, y, flag);
	if(e < 0) {
		return RLF_handle_error(e, "Flag not valid");
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
	PyArg_ParseTuple(args, "iiii", &n, &x, &y, &flag);
	err e = RLF_has_flag(n, x, y, flag);
	if(e < 0) {
		return RLF_handle_error(e, "Flag not valid");
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
	PyArg_ParseTuple(args, "iiii", &n, &x, &y, &flag);
	err e = RLF_clear_flag(n, x, y, flag);
	if(e < 0) {
		return RLF_handle_error(e, "Flag not valid");
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
	unsigned int n, x, y;
	PyArg_ParseTuple(args, "iii", &n, &x, &y);
	int flag = RLF_get_flags(n, x, y);
	if(flag < 0) {
		return RLF_handle_error(flag, "Flag not valid");
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
	PyArg_ParseTuple(args, "i|i", &n, &f);
	err e = RLF_clear_map(n, f);
	if(e < 0) {
		return RLF_handle_error(e, "Flag not valid");
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
	PyArg_ParseTuple(args, "ii", &n, &flag);
	err e = RLF_fill_map(n, flag);
	if(e < 0) {
		return RLF_handle_error(e, "Flag not valid");
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
	PyArg_ParseTuple(args, "i(ii)(ii)", &m, &x1, &y1, &x2, &y2);
	err e = RLF_los(m, x1, y1, x2, y2);
	if(e < 0) {
		return RLF_handle_error(e, "LOS failed");
	}
	if(e) {
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}
/*
 +-----------------------------------------------------------+
 * @desc	Line of sight
 +-----------------------------------------------------------+
 */
static PyObject*
los_xy(PyObject *self, PyObject* args) {
	unsigned int m, x1, y1, x2, y2;
	PyArg_ParseTuple(args, "iiiii", &m, &x1, &y1, &x2, &y2);
	err e = RLF_los(m, x1, y1, x2, y2);
	if(e < 0) {
		return RLF_handle_error(e, "LOS failed");
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
	PyArg_ParseTuple(args, "iii|iii", &m, &x, &y, &r, &a, &lw);
	err e = RLF_fov(m, x, y, r, a, lw);
	if(e < 0) {
		return RLF_handle_error(e, "Fov failed");
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
	PyArg_ParseTuple(args, "iiii", &x1, &y1, &x2, &y2);
	int d = RLF_distance(x1, y1, x2, y2);
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
	PyArg_ParseTuple(args, "i(ii)(ii)|iif", &m, &x1, &y1, &x2, &y2, &a, &r, &f, &d);

	int p = RLF_path_create(m, x1, y1, x2, y2, a, r, f, d);
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
	PyArg_ParseTuple(args, "i", &p);
	err e = RLF_path_size(p);
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
	PyArg_ParseTuple(args, "i", &p);
	err e = RLF_path_delete(p);
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
	PyArg_ParseTuple(args, "i", &path);
	int count = RLF_path_size(path);
	if(count < 0) {
		Py_RETURN_FALSE;
	}
	PyObject *result = PyTuple_New(count);
	unsigned int i, x, y;
	for (i = 0; i < count; i++) {
		RLF_path_step(path, i, &x, &y);
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
	PyArg_ParseTuple(args, "i(ii)(ii)|iiif", &m, &x1, &y1, &x2,
											 &y2, &a, &r, &f, &d);

	int path = RLF_path_create(m, x1, y1, x2, y2, a, r, f, d);
	if(path < 0) {
		// No path
		Py_RETURN_FALSE;
	}
	int count = RLF_path_size(path);

	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLF_path_step(path, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLF_path_delete(path);

	if(e < 0) {
		return RLF_handle_error(e, "Path delete failed");
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
	PyArg_ParseTuple(args, "iiii|ii", &m, &ox, &oy, &r, &flag, &los);
	unsigned int dx, dy;
	err e = RLF_scatter(m, ox, oy, &dx, &dy, r, flag, los);
	if(e < 0) {
		return RLF_handle_error(e, "Scatter failed");
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
	unsigned int i, m, x, y, ox, oy, tx, ty;
	PyArg_ParseTuple(args, "i(ii)(ii)", &m, &ox, &oy, &tx, &ty);
	err projection = RLF_project_beam(m, ox, oy, tx, ty);
	if(projection < 0) {
		return RLF_handle_error(projection, "Projection failed");
	}

	int count = RLF_project_size(projection);
	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLF_project_step(projection, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLF_project_delete(projection);
	if(e < 0) {
		return RLF_handle_error(e, "Delete failed");
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
	unsigned int i, m, x, y, ox, oy, tx, ty, r;
	PyArg_ParseTuple(args, "i(ii)(ii)i", &m, &ox, &oy, &tx, &ty, &r);
	err projection = RLF_project_ball(m, ox, oy, tx, ty, r);
	if(projection < 0) {
		return RLF_handle_error(projection, "Projection failed");
	}

	int count = RLF_project_size(projection);
	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLF_project_step(projection, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLF_project_delete(projection);
	if(e < 0) {
		return RLF_handle_error(e, "Delete failed");
	}
	return result;
}
/*
 +-----------------------------------------------------------+
 * @desc	Breath
 +-----------------------------------------------------------+
 */
static PyObject*
project_breath(PyObject *self, PyObject* args) {
	unsigned int i, m, x, y, ox, oy, tx, ty, r;
	PyArg_ParseTuple(args, "i(ii)(ii)i", &m, &ox, &oy, &tx, &ty, &r);
	err projection = RLF_project_breath(m, ox, oy, tx, ty, r);
	if(projection < 0) {
		return RLF_handle_error(projection, "Projection failed");
	}

	int count = RLF_project_size(projection);
	PyObject *result = PyTuple_New(count);
	for (i = 0; i < count; i++) {
		RLF_project_step(projection, i, &x, &y);
		PyObject *value = Py_BuildValue("(ii)", x, y);
		PyTuple_SetItem(result, i, value);
	}
	err e = RLF_project_delete(projection);
	if(e < 0) {
		return RLF_handle_error(e, "Delete failed");
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
	PyArg_ParseTuple(args, "i", &max);
	return Py_BuildValue("i", RLF_randint(max));
}
/*
 +-----------------------------------------------------------+
 * @desc	Error handling
 +-----------------------------------------------------------+
 */
static void*
RLF_handle_error(err code, const char* generic) {
	RLFError = PyErr_NewException("rlf.error", NULL, NULL);
	if(code == -2) {
		PyErr_SetString(RLFError, "Map not initialized");
	} else {
		PyErr_SetString(RLFError, generic);
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
	 {"project_breath", project_breath, METH_VARARGS, "Breath projection"},
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
        "RLF docstring",
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
    PyModule_AddIntConstant(module, "MAX_MAPS", 	MAX_MAPS);

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
    PyModule_AddIntConstant(module, "FOV_BASIC", 	FOV_BASIC);
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

#if PY_MAJOR_VERSION >= 3
    return module;
#endif
}
