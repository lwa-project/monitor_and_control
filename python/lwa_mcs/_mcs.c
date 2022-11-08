#include "Python.h"
#include "mcs.h"
#include "me.h"

#include <stdio.h>
#include <gdbm.h>
#include <sys/socket.h>
#include <netinet/in.h> /* for network sockets */
#include <arpa/inet.h>  /* for network sockets */

#if PY_MAJOR_VERSION >= 3
    #define PyMIB_RETURN "y"

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
    #define PyMIB_RETURN "s"

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


static PyObject *send_sch_command(PyObject *self, PyObject *args, PyObject *kwds) {
    char *subsystem, *command, *data;
    PyObject *output;
    if(!PyArg_ParseTuple(args, "sss", &subsystem, &command, &data)) {
        PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
        goto fail;
    }
    
    struct LWA_cmd_struct cmd; 

    struct timeval tv;  /* from sys/time.h; included via LWA_MCS.h */
    struct timezone tz;
    
    int sockfd, result;   
    struct sockaddr_in address; /* for network sockets */
    struct timeval timeout;      
    timeout.tv_sec = LWA_PTQ_TIMEOUT;
    timeout.tv_usec = 0;
    
    sockfd = socket(             /* create socket */
                    AF_INET,     /* domain; network sockets */
                    SOCK_STREAM, /* type (TCP-like) */
                    0);          /* protocol (normally 0) */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(LWA_IP_MSE);
    address.sin_port = htons(LWA_PORT_MSE);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    
    /* connect socket to server's socket */
    result = connect( sockfd, (struct sockaddr *) &address, sizeof(address));
    if (result==-1) {
        close(sockfd);
        PyErr_Format(PyExc_RuntimeError, "MCS/sch - ms_exec does not appear to be running");
        goto fail;
    }

    memset(&cmd, 0, sizeof(struct LWA_cmd_struct));
    if ( !( cmd.sid = LWA_getsid(subsystem) ) ) {
        close(sockfd);
        PyErr_Format(PyExc_ValueError, "Invalid subsystem name");
        goto fail;
    }
    if ( !( cmd.cid = LWA_getcmd(command) ) ) {
        close(sockfd);
        PyErr_Format(PyExc_ValueError, "Invalid command name");
        goto fail;
    }
    gettimeofday(&cmd.tv, &tz);
    strcpy(cmd.data, data); /* changed in reply */
    cmd.datalen = -1;
    write(sockfd, &cmd, sizeof(struct LWA_cmd_struct));
    read(sockfd, &cmd, sizeof(struct LWA_cmd_struct));
    
    output = Py_BuildValue("(ii)", cmd.ref, cmd.bAccept);
    close(sockfd);
    return output;

fail:
    return NULL;
}

PyDoc_STRVAR(send_sch_command_doc, \
"Given a subsystem name, command name, and a command data string, send the\n\
command to ms_exec for processing and return a two-element tuple of the command\n\
reference ID and whether or not it was accepted.");


static PyObject *read_mib_ip(
    PyObject *self, PyObject *args, PyObject *kwds) {
    char *subsystem, *label;
    PyObject *output;
    if(!PyArg_ParseTuple(args, "ss", &subsystem, &label)) {
        PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
        goto fail;
    }
    
    struct LWA_mib_entry record;
    struct timeval tv;
    
    double ts;
    
    int sockfd, result;   
    struct sockaddr_in address; /* for network sockets */
    struct timeval timeout;      
    timeout.tv_sec = LWA_PTQ_TIMEOUT;
    timeout.tv_usec = 0;
    
    sockfd = socket(             /* create socket */
                    AF_INET,     /* domain; network sockets */
                    SOCK_STREAM, /* type (TCP-like) */
                    0);          /* protocol (normally 0) */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(LWA_IP_MSE);
    address.sin_port = htons(LWA_PORT_MSE2);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    
    /* connect socket to server's socket */
    result = connect( sockfd, (struct sockaddr *) &address, sizeof(address));
    if (result==-1) {
        close(sockfd);
        PyErr_Format(PyExc_RuntimeError, "MCS/sch - ms_mdre_ip does not appear to be running");
        goto fail;
    }

    memset(&record, 0, sizeof(struct LWA_mib_entry));
    strcpy(record.ss, subsystem);
    strcpy(record.label, label);
    write(sockfd, &record, sizeof(struct LWA_mib_entry));
    read(sockfd, &record, sizeof(struct LWA_mib_entry));
    
    tv = record.last_change;
    ts = tv.tv_sec + tv.tv_usec/1e6;
    output = Py_BuildValue("(" PyMIB_RETURN "#f)", record.val, MIB_VAL_FIELD_LENGTH, ts);
    close(sockfd);
    return output;

fail:
    return NULL;
} 

PyDoc_STRVAR(read_mib_ip_doc, \
"Given a subsystem name and a MIB label, read the MIB from the ms_mdre_ip\n\
interface and return a two-element tuple of the undecoded value itself,\n\
and the timestamp of when the value was last updated.\n\
\n\
.. note:\n\
    The value itself may be null padded");
      

static PyObject *read_mib(PyObject *self, PyObject *args, PyObject *kwds) {
    char *subsystem, *mib_label;
    PyObject *output;
    if(!PyArg_ParseTuple(args, "ss", &subsystem, &mib_label)) {
        PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
        goto fail;
    }
    
    /* dbm-related variables */
    char dbm_filename[256];
    GDBM_FILE dbm_ptr;
    struct dbm_record record;
    datum datum_key;
    datum datum_data;
    
    char label[MIB_LABEL_FIELD_LENGTH];     /* this is the key for dbm */
    char key[MIB_LABEL_FIELD_LENGTH];
    struct timeval tv;  /* from sys/time.h; included via LWA_MCS.h */
    
    double ts;
    
    /* Go! */
    sprintf(dbm_filename, "%s/%s.gdb", LWA_SCH_SCP_DIR, subsystem);
    dbm_ptr = gdbm_open(dbm_filename, 0, GDBM_READER, 0, NULL);
    if (!dbm_ptr) {
        PyErr_Format(PyExc_ValueError, "Invalid subsystem name");
        goto fail;
    }
    
    sprintf(key, "%s", mib_label);
    datum_key.dptr = key;
    datum_key.dsize = strlen(key);
    datum_data = gdbm_fetch(dbm_ptr,datum_key);
    if (datum_data.dptr) {
        memcpy( &record, datum_data.dptr, datum_data.dsize );
        free(datum_data.dptr);
    } else {
        gdbm_close(dbm_ptr);
        PyErr_Format(PyExc_ValueError, "Invalid MIB label");
        goto fail;
    }
    tv = record.last_change;
    ts = tv.tv_sec + tv.tv_usec/1e6;
    
    output = Py_BuildValue("(" PyMIB_RETURN PyMIB_RETURN "#f)", record.type_dbm, record.val, MIB_VAL_FIELD_LENGTH, ts);
    gdbm_close(dbm_ptr);
    return output;
    
fail:
    return NULL;
}

PyDoc_STRVAR(read_mib_doc, \
"Given a subsystem name and a MIB label, read the MIB from disk and\n\
and return a three-element tuple of the value encoding, the undecoded\n\
value itself, and the timestamp of when the value was last updated.\n\
\n\
.. note:\n\
    The value itself may be null padded");


static PyObject *send_exec_command(PyObject *self, PyObject *args, PyObject *kwds) {
    char *command, *data;
    int status;
    PyObject *output;
    if(!PyArg_ParseTuple(args, "ss", &command, &data)) {
        PyErr_Format(PyExc_RuntimeError, "Invalid parameters");
        goto fail;
    }
    
    struct me_cmd_struct cmd;
    
    int sockfd, result;   
    struct sockaddr_in address; /* for network sockets */
    struct timeval timeout;      
    timeout.tv_sec = LWA_PTQ_TIMEOUT;
    timeout.tv_usec = 0;
    
    sockfd = socket(             /* create socket */
                    AF_INET,     /* domain; network sockets */
                    SOCK_STREAM, /* type (TCP-like) */
                    0);          /* protocol (normally 0) */
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(LWA_IP_MEE);
    address.sin_port = htons(LWA_PORT_MEE);
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
    
    /* connect socket to server's socket */
    result = connect( sockfd, (struct sockaddr *) &address, sizeof(address));
    if (result==-1) {
        close(sockfd);
        PyErr_Format(PyExc_RuntimeError, "MCS/exec - me_exec does not appear to be running");
        goto fail;
    }
    
    memset(&cmd, 0, sizeof(struct me_cmd_struct));
    if( !strcmp("NUL", command) ) {
        cmd.cmd = ME_CMD_NUL;
    } else if( !strcmp("SHT", command) ) {
        cmd.cmd = ME_CMD_SHT;
    } else if( !strcmp("STP", command) ) {
        cmd.cmd = ME_CMD_STP;
    } else {
        close(sockfd);
        PyErr_Format(PyExc_ValueError, "Invalid command name");
        goto fail;
    }
    strcpy(cmd.args, data);
    write(sockfd, &cmd, sizeof(struct me_cmd_struct));
    read(sockfd, &cmd, sizeof(struct me_cmd_struct));
    
    status = 1;
    if( cmd.cmd < ME_CMD_NUL ) {
        status = 0;
    }
    output = Py_BuildValue("i", status);
    close(sockfd);
    return output;

fail:
    return NULL;
}

PyDoc_STRVAR(send_exec_command_doc, \
"Given an MCS/exec command name and command argument data, send the command\n\
to me_exec for processing and return whether or not the command was accepted.");
    

/*
  Module Setup - Function Definitions and Documentation
*/

static PyMethodDef McsMethods[] = {
    {"get_current_time",  (PyCFunction) get_current_time,  METH_VARARGS, get_current_time_doc },
    {"send_sch_command",  (PyCFunction) send_sch_command,  METH_VARARGS, send_sch_command_doc },
    {"read_mib_ip",       (PyCFunction) read_mib_ip,       METH_VARARGS, read_mib_ip_doc      },
    {"read_mib",          (PyCFunction) read_mib,          METH_VARARGS, read_mib_doc         },
    {"send_exec_command", (PyCFunction) send_exec_command, METH_VARARGS, send_exec_command_doc},
    {NULL,                NULL,                            0,            NULL                 }
};

PyDoc_STRVAR(mcs_doc, \
"Configuration information and basic functions for the LWA-MCS system");

/*
  Module Setup - Initialization
*/

MOD_INIT(_mcs) {
    PyObject *m, *all;
    
    Py_Initialize();

    // Module definitions and functions
    MOD_DEF(m, "_mcs", McsMethods, mcs_doc);
    if( m == NULL ) {
        return MOD_ERROR_VAL;
    }
    
    // Version information
    PyModule_AddObject(m, "__version__", PyString_FromString("0.4"));
    
    // Constants
#ifdef USE_ADP
    PyModule_AddObject(m, "IS_ADP", Py_True);
#else
    PyModule_AddObject(m, "IS_ADP", Py_False);
#endif
    PyModule_AddObject(m, "MAX_NDR", PyInt_FromLong(ME_MAX_NDR));
    
    PyModule_AddObject(m, "SCH_PATH", PyString_FromString(LWA_SCH_SCP_DIR));
    
    PyModule_AddObject(m, "MCS_TIMEOUT", PyInt_FromLong(LWA_PTQ_TIMEOUT));
    
    // Function listings
    all = PyList_New(0);
    PyList_Append(all, PyString_FromString("IS_ADP"));
    PyList_Append(all, PyString_FromString("MAX_NDR"));
    PyList_Append(all, PyString_FromString("SCH_PATH"));
    PyList_Append(all, PyString_FromString("MCS_TIMEOUT"));
    PyList_Append(all, PyString_FromString("get_current_time"));
    PyList_Append(all, PyString_FromString("send_sch_command"));
    PyList_Append(all, PyString_FromString("read_mib_ip"));
    PyList_Append(all, PyString_FromString("read_mib"));
    PyList_Append(all, PyString_FromString("send_exec_command"));
    PyModule_AddObject(m, "__all__", all);

    return MOD_SUCCESS_VAL(m);
}
