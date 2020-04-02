#include "Python.h"
#include "mcs.h"
#include "me.h"

#include <stdio.h>

#if PY_MAJOR_VERSION >= 3
	#define PyInt_FromLong PyLong_FromLong
	#define PyString_AS_STRING PyBytes_AS_STRING
	#define PyString_FromString PyUnicode_FromString
	inline char* PyString_AsString(PyObject *ob) {
    		PyObject *enc;
    		char *cstr;
   		enc = PyUnicode_AsEncodedString(ob, "utf-8", "Error");
    		if( enc == NULL ) {
        		PyErr_Format(PyExc_ValueError, "Cannot encode string");
        		return NULL;
    		}
    		cstr = PyBytes_AsString(enc);
    		Py_XDECREF(enc);
    		return cstr;
	}
	#define MOD_ERROR_VAL NULL
    	#define MOD_SUCCESS_VAL(val) val
   	#define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
    	#define MOD_DEF(ob, name, methods, doc) \
       		static struct PyModuleDef moduledef = { \
          		PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
       		ob = PyModule_Create(&moduledef);
#else
	#define MOD_ERROR_VAL
    	#define MOD_SUCCESS_VAL(val)
    	#define MOD_INIT(name) PyMODINIT_FUNC init##name(void)
    	#define MOD_DEF(ob, name, methods, doc) \
       		ob = Py_InitModule3(name, methods, doc);
#endif

static PyObject *get_current_time(PyObject *self, PyObject *args, PyObject *kwds) {
	long int mjd, mpm;
	if(!PyArg_ParseTuple(args, "", NULL)) {
		PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
		goto fail;
	}

	LWA_time(&mjd, &mpm);
	
	return Py_BuildValue("(ii)", mjd, mpm);
	
fail:
    return NULL;
}

PyDoc_STRVAR(get_current_time_doc, \
"Get the current time as two-element tuple of MJD and MPM");


static PyObject *subsystem_to_sid(PyObject *self, PyObject *args, PyObject *kwds) {
	char *input;
	int result;
	if(!PyArg_ParseTuple(args, "s", &input)) {
		PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
		goto fail;
	}

	result = LWA_getsid(input);
	if( result == 0 ) {
		PyErr_Format(PyExc_ValueError, "Invalid subsystem name");
		goto fail;
	}

	return Py_BuildValue("i", result);
	
fail:
	return NULL;
}

PyDoc_STRVAR(subsystem_to_sid_doc, \
"Convert a subsystem name to a subsystem ID");

static PyObject *sid_to_subsystem(PyObject *self, PyObject *args, PyObject *kwds) {
        int input;
        char *result;
        if(!PyArg_ParseTuple(args, "i", &input)) {
                PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
                goto fail;
        }

        result = LWA_sid2str(input);
        if( !strcmp("XXX", result) ) {
                PyErr_Format(PyExc_ValueError, "Invalid subsystem ID");
                goto fail;
        }

	return Py_BuildValue("s", result);

fail:
        return NULL;
}

PyDoc_STRVAR(sid_to_subsystem_doc, \
"Convert a subsystem ID to a subsystem name");


static PyObject *command_to_cid(PyObject *self, PyObject *args, PyObject *kwds) {
	char *input;
	int result;
	if(!PyArg_ParseTuple(args, "s", &input)) {
		PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
		goto fail;
	}

	result = LWA_getcmd(input);
	if( result == 0 ) {
		PyErr_Format(PyExc_ValueError, "Invalid command name");
		goto fail;
	}

	return Py_BuildValue("i", result);
	
fail:
	return NULL;
}

PyDoc_STRVAR(command_to_cid_doc, \
"Convert a command name to a command ID");

static PyObject *cid_to_command(PyObject *self, PyObject *args, PyObject *kwds) {
        int input;
        char *result;
        if(!PyArg_ParseTuple(args, "i", &input)) {
                PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
                goto fail;
        }

        result = LWA_cmd2str(input);
        if( !strcmp("XXX", result) ) {
                PyErr_Format(PyExc_ValueError, "Invalid subsystem ID");
                goto fail;
        }

	return Py_BuildValue("s", result);

fail:
        return NULL;
}

PyDoc_STRVAR(cid_to_command_doc, \
"Convert a command ID to a command name");


static PyObject *execcmd_to_eid(PyObject *self, PyObject *args, PyObject *kwds) {
	char *input;
	int result;
	if(!PyArg_ParseTuple(args, "s", &input)) {
		PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
		goto fail;
	}

    if( !strcmp("ERR", input) ) {
        result = ME_CMD_ERR;
    } else if( !strcmp("NUL", input) ) {
        result = ME_CMD_NUL;
    } else if( !strcmp("SHT", input) ) {
        result = ME_CMD_SHT;
    } else if( !strcmp("STP", input) ) {
        result = ME_CMD_STP;
    } else { 
        PyErr_Format(PyExc_ValueError, "Invalid exec command name");
		goto fail;
	}

	return Py_BuildValue("i", result);
	
fail:
	return NULL;
}

PyDoc_STRVAR(execcmd_to_eid_doc, \
"Convert an exec command name to an exec command ID");

static PyObject *eid_to_execcmd(PyObject *self, PyObject *args, PyObject *kwds) {
        int input;
        char *result;
        if(!PyArg_ParseTuple(args, "i", &input)) {
                PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
                goto fail;
        }
        
        if( input == ME_CMD_ERR ) {
            strcpy(result, "ERR");
        } else if( input == ME_CMD_NUL ) {
            strcpy(result, "NUL");
        } else if( input == ME_CMD_SHT ) {
            strcpy(result, "SHT");
        } else if( input == ME_CMD_STP ) {
            strcpy(result, "STP");
        } else {
            PyErr_Format(PyExc_ValueError, "Invalid subsystem ID");
            goto fail;
        }
        
	    return Py_BuildValue("s", result);

fail:
        return NULL;
}

PyDoc_STRVAR(eid_to_execcmd_doc, \
"Convert an exec command ID to an exec command name");


/*
  Module Setup - Function Definitions and Documentation
*/

static PyMethodDef ConfigMethods[] = {
    {"get_current_time", (PyCFunction) get_current_time, METH_VARARGS, get_current_time_doc},
	{"subsystem_to_sid", (PyCFunction) subsystem_to_sid, METH_VARARGS, subsystem_to_sid_doc},
	{"sid_to_subsystem", (PyCFunction) sid_to_subsystem, METH_VARARGS, sid_to_subsystem_doc},
	{"command_to_cid",   (PyCFunction) command_to_cid,   METH_VARARGS, command_to_cid_doc  },
	{"cid_to_command",   (PyCFunction) cid_to_command,   METH_VARARGS, cid_to_command_doc  },
	{"execcmd_to_eid",   (PyCFunction) execcmd_to_eid,   METH_VARARGS, execcmd_to_eid_doc  },
	{"eid_to_execcmd",   (PyCFunction) eid_to_execcmd,   METH_VARARGS, eid_to_execcmd_doc  },
	{NULL,       NULL,                  0,            NULL       }
};

PyDoc_STRVAR(config_doc, \
"Configuration information about the LWA-MCS system");

/*
  Module Setup - Initialization
*/

MOD_INIT(config) {
	PyObject *m, *all;
	
	Py_Initialize();

	// Module definitions and functions
	MOD_DEF(m, "config", ConfigMethods, config_doc);
	if( m == NULL ) {
		return MOD_ERROR_VAL;
	}
	
	// Version information
	PyModule_AddObject(m, "__version__", PyString_FromString("0.1"));
	
	// Constants
#ifdef USE_ADP
	PyModule_AddObject(m, "IS_ADP", Py_True);
#else
	PyModule_AddObject(m, "IS_ADP", Py_False);
#endif
	PyModule_AddObject(m, "MAX_NDR", PyInt_FromLong(ME_MAX_NDR));
	
	PyModule_AddObject(m, "SOCKET_TIMEOUT", PyFloat_FromDouble(5.0));
	
	PyModule_AddObject(m, "SCH_PATH", PyString_FromString(LWA_SCH_SCP_DIR));
	
	PyModule_AddObject(m, "MSE_ADDRESS", PyString_FromString(LWA_IP_MSE));
	PyModule_AddObject(m, "MSE_PORT", PyInt_FromLong(LWA_PORT_MSE));
	PyModule_AddObject(m, "MSE2_ADDRESS", PyString_FromString(LWA_IP_MSE));
	PyModule_AddObject(m, "MSE2_PORT", PyInt_FromLong(LWA_PORT_MSE2));
	PyModule_AddObject(m, "MEE_ADDRESS", PyString_FromString(LWA_IP_MEE));
	PyModule_AddObject(m, "MEE_PORT", PyInt_FromLong(LWA_PORT_MEE));
	
	// Function listings
	all = PyList_New(0);
	PyList_Append(all, PyString_FromString("IS_ADP"));
    PyList_Append(all, PyString_FromString("MAX_NDR"));
	PyList_Append(all, PyString_FromString("SOCKET_TIMEOUT"));
	PyList_Append(all, PyString_FromString("get_current_time"));
	PyList_Append(all, PyString_FromString("subsystem_to_sid"));
	PyList_Append(all, PyString_FromString("sid_to_subsystem"));
	PyList_Append(all, PyString_FromString("command_to_cid"));
	PyList_Append(all, PyString_FromString("cid_to_command"));
	PyList_Append(all, PyString_FromString("execcmd_to_eid"));
	PyList_Append(all, PyString_FromString("eid_to_execcmd"));
	PyModule_AddObject(m, "__all__", all);

	return MOD_SUCCESS_VAL(m);
}
