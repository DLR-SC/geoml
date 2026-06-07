.. _installation:

Installation and Building
=========================

geoml can be installed in three different ways:

* via Conda from the Conda channel DLR-SC
* build from source
* download binaries from `geoml's GitHub page <https://github.com/DLR-SC/geoml>`_

Geoml is available for Linux and Windows.

Via Conda
---------

The easiest way to install geoml and all its dependencies is using the package manager Conda.

To install geoml into a separate environement (which is recommended), enter the following into the terminal 

.. code-block:: bash

    conda create -n geoml_env geoml -c dlr-sc

Activate the environement:

.. code-block:: bash

   conda activate geoml_env

The following dependencies are needed

* CMake 3.15 or higher
* A C++17 compatible compiler (GCC, Clang, MSVC)
* OpenCASCADE 7.6 or higher

Further dependencies can be found in the `pyproject.toml <add Url after it is in the main branch>`_ 

Building from Source
--------------------

* activate the conda environment ``geoml_env``(and install needed dependencies as mentioned above) and make sure a C++17 compatible compiler (GCC, Clang, MSVC) is provided

Clone the repository and build with CMake:

.. code-block:: bash

   git clone https://github.com/DLR-SC/geoml.git
   cd geoml
   mkdir build && cd build
   cmake ..
   cmake --build .

CMake options:

.. list-table::
   :header-rows: 1

   * - Option
     - Description
     - Default Value
   * - GEOML_INSTALL_INTERNAL_API
     - Install geoml's C++ bindings
     - OFF
   * - GEOML_BUILD_PYTHON_BINDINGS 
     - Install geoml's Python bindings
     - OFF
   * - GEOML_BUILD_TESTS
     - Build geoml Testsuite
     - OFF
   * - GEOML_BUILD_EXAMPLES
     - Build geoml examples
     - OFF
   * - GEOML_BUILD_DOCS
     - Build documentation
     - OFF

Installation
~~~~~~~~~~~~

After building, install the library:

.. code-block:: bash

   cmake --install .
