#include "toyws.h"

typedef struct 
{
    PyObject_HEAD
    Response* response;
} StartResponse;

static PyObject*
start_response(PyObject* self, PyObject *args, PyObject *kwargs)
{
    Response* response = ((StartResponse*) self)->response;
    
    PyObject* status = NULL;
    PyObject* headers = NULL;
    
    if(!PyArg_UnpackTuple(args, "start_response", 2, 2, &status, &headers))
        return NULL;
    
    if(!PyString_Check(status)) {
        printf("start_response argument 1 should be a 'status reason' string\n");
        return NULL;
    }
    response->status = status;
    printf("%s\n", PyString_AsString(response->status));
    if(!PyList_Check(headers)) {
        printf("start response argument 2 should be a list of 2-tuples\n");
        return NULL;
    }
    Py_INCREF(response->status);
    //Py_INCREF(self_headers);
    Py_RETURN_NONE;
}

PyTypeObject StartResponse_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "start_response",           /* tp_name (__name__)                         */
  sizeof(StartResponse),      /* tp_basicsize                               */
  0,                          /* tp_itemsize                                */
  (destructor)PyObject_FREE,  /* tp_dealloc                                 */
  0, 0, 0, 0, 0, 0, 0, 0, 0,  /* tp_{print,getattr,setattr,compare,...}     */
  start_response              /* tp_call (__call__)                         */
};


void call_app(Response* response, char *body, int *len)
{
    PyObject* moduleString = PyString_FromString(py_app);
    PyObject* app_module = PyImport_Import(moduleString);
    

    /* get function reference */
    PyObject* app_func = PyObject_GetAttrString(app_module, py_app);
    printf("get app\n");
    StartResponse* start_response = PyObject_NEW(StartResponse, &StartResponse_Type);
    start_response->response = response;
    
    PyObject* env = PyDict_New();
    /*
      PyDict_SetItem(env,
                   PyString_FromString("REQUEST_METHOD"),
                   PyString_FromString("GET"));
      PyDict_SetItem(env,
                   PyString_FromString("PATH_INFO"),
                   PyString_FromString("/"));
    */
    PyObject* args = PyTuple_Pack(2, env, start_response);
    printf("calling app\n");
    PyObject* output = PyObject_CallObject(app_func, args);
    
    Py_DECREF(start_response);
    
    if(!PyString_Check(output)) {
        printf("error : app should return string");
        return ;
    }
    response->body = output;
    printf("%s\n", PyString_AsString(output));
     strcpy(body, PyString_AsString(output));
    *len = strlen(body);
}
