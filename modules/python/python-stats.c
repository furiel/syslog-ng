/*
 * Copyright (c) 2018 Viktor Tusa <tusavik@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */
#include "python-stats.h"
#include "stats/stats-registry.h"
#include "stats/stats-cluster-single.h"
#include <Python.h>

typedef struct _PyStats
{
  PyObject_HEAD
  StatsCounterItem *counter;
  const char *stats_name;
} PyStats;

static PyTypeObject py_stats_type;

PyObject *
py_stats_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  PyStats *self = (PyStats *) type->tp_alloc(type, 0);

  if (self != NULL)
    {
      if (!PyArg_ParseTuple(args, "s", &(self->stats_name)))
        {
          PyErr_SetString(PyExc_TypeError, "parameter is not a string object");
          return NULL;
        }

      stats_lock();

      StatsClusterKey sc_key;
      stats_cluster_single_key_set(&sc_key, SCS_PYTHON, self->stats_name, NULL);
      stats_register_counter(0, &sc_key, SC_TYPE_SINGLE_VALUE, &(self->counter));

      stats_unlock();
    }
  return (PyObject *) self;
};

static void
py_stats_free(PyStats *self)
{
  stats_lock();
  StatsClusterKey sc_key;
  stats_cluster_single_key_set(&sc_key, SCS_PYTHON, self->stats_name, NULL);
  stats_unregister_counter(&sc_key, SC_TYPE_SINGLE_VALUE, &(self->counter));
  stats_unlock();

  free( (void *) self->stats_name);

  PyObject_Del(self);
}

static PyObject *
py_stats_increase(PyStats *self, PyObject *unused)
{
  stats_counter_inc(self->counter);
  Py_RETURN_NONE;
}

static PyObject *
py_stats_decrease(PyStats *self, PyObject *unused)
{
  stats_counter_dec(self->counter);
  Py_RETURN_NONE;
}

static PyObject *
py_stats_add(PyStats *self, PyObject *args)
{
  gssize amount;
  if (!PyArg_ParseTuple(args,"n", &amount))
    {
      PyErr_SetString(PyExc_TypeError, "parameter is not a number");
      return NULL;
    }
  stats_counter_add(self->counter, amount);
  Py_RETURN_NONE;
}

static PyObject *
py_stats_sub(PyStats *self, PyObject *args)
{
  gssize amount;
  if (!PyArg_ParseTuple(args,"n", &amount))
    {
      PyErr_SetString(PyExc_TypeError, "parameter is not a number");
      return NULL;
    }

  stats_counter_sub(self->counter, amount);
  Py_RETURN_NONE;
}

static PyMethodDef py_stats_methods[] =
{
  {
    "increase", (PyCFunction)py_stats_increase,
    METH_NOARGS, "Increase counter"
  },
  {
    "decrease", (PyCFunction)py_stats_decrease,
    METH_NOARGS, "Decrease counter"
  },
  {
    "add", (PyCFunction)py_stats_add,
    METH_VARARGS, "Increase counter with amount"
  },
  {
    "sub", (PyCFunction)py_stats_sub,
    METH_VARARGS, "Decrease counter with amount"
  },
  {NULL}
};

static PyTypeObject py_stats_type =
{
  PyVarObject_HEAD_INIT(&PyType_Type, 0)
  .tp_name = "StatsCounter",
  .tp_basicsize = sizeof(PyStats),
  .tp_dealloc = (destructor) py_stats_free,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  .tp_doc = "StatsCounter representing a counter in syslog-ng statistics",
  .tp_new = py_stats_new,
  .tp_methods = py_stats_methods,
  .tp_str = NULL,
  .tp_as_mapping = NULL,
  0,
};

static PyMethodDef stats_module_methods[] =
{
  {NULL}  /* Sentinel */
};


PyMODINIT_FUNC
PyInit_stats(void)
{
  PyObject *m;
  if (PyType_Ready(&py_stats_type) < 0)
    return;

  m = Py_InitModule3("sngstats", stats_module_methods,
                     "Module for encapsulating StatsCounter type");

  Py_INCREF((PyObject *) &py_stats_type);
  PyModule_AddObject(m, "StatsCounter", (PyObject *) &py_stats_type);
}

void
py_stats_init(void)
{
  PyInit_stats();
}

