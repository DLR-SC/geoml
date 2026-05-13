.. _installation:

Installation
============

Prerequisites
-------------

The following dependencies are required:

* CMake 3.15 or higher
* A C++17 compatible compiler (GCC, Clang, MSVC)
* OpenCASCADE 7.6 or higher

Building from Source
--------------------

Clone the repository and build with CMake:

.. code-block:: bash

   git clone https://github.com/yourorg/geoml.git
   cd geoml
   mkdir build && cd build
   cmake ..
   cmake --build .

Installation
------------

After building, install the library:

.. code-block:: bash

   cmake --install .

Usage in Your Project
---------------------

Add geoml to your CMake project:

.. code-block:: cmake

   find_package(geoml REQUIRED)
   target_link_libraries(my_target PRIVATE geoml::geoml)