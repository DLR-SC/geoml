.. _installation:

Installation and Building
=========================

This page explains how to obtain geoml and how to build it from source.

The most general word is **obtain**: depending on the selected workflow, you
either install a package, build the library yourself, or download a prepared
release archive.

Ways to Obtain geoml
--------------------

There are three planned ways to obtain geoml:

1. **Install the Conda package from the DLR-SC Conda channel.**

   This is intended to become the easiest path for most users. The package is
   currently work in progress. Once available, it should install geoml and its
   runtime dependencies into an isolated Conda environment.

2. **Build geoml from source.**

   This is the recommended path for developers, for users who need the newest
   source code, and for users who want to build the C++ library or Python
   bindings themselves. The rest of this page focuses on this workflow.

3. **Download a prepared package from GitHub Releases.**

   This is also work in progress. The intended workflow is that GitHub CI builds
   release packages automatically and attaches them to a GitHub Release. GitHub
   Pages is used for the documentation website; downloadable software packages
   usually belong on GitHub Releases rather than on the documentation page.

Supported Platforms
-------------------

geoml is intended to build on Windows and Linux. The repository also contains a
Pixi platform entry for macOS, but the instructions below focus on Windows and
Linux.

Build from Source: Overview
---------------------------

Building from source means that you compile geoml locally after cloning the
GitHub repository. Installing is a separate optional step after the build. It
copies the built headers, libraries, CMake package files, documentation, and
possibly Python modules into a clean install location.

The usual workflow is:

1. Install or activate a build environment.
2. Clone the geoml repository.
3. Configure the build with CMake.
4. Build geoml.
5. Optionally install geoml.
6. Use the installed library from C++ or Python.

As a beginner, use an isolated environment manager. The two recommended choices
are:

* **Pixi**, because this repository already describes the project dependencies
  in ``pyproject.toml``.
* **Conda/Mamba**, because it is widely used and works well for Python and C++
  dependencies from Conda channels.

Required Build Dependencies
---------------------------

For a normal C++ build you need:

* Git
* CMake 3.15 or newer
* A C++17 capable compiler
* OpenCASCADE

For Python bindings you additionally need:

* Python 3.9 or newer
* Python development headers and libraries
* PythonOCC
* SWIG
* setuptools

For documentation you additionally need:

* Doxygen
* Sphinx
* Breathe
* sphinx-rtd-theme
* m2r
* docutils < 0.21

The Pixi dependencies are defined in
`pyproject.toml <https://github.com/DLR-SC/geoml/blob/main/pyproject.toml>`_.

Clone the Repository
--------------------

Start by cloning geoml locally:

.. code-block:: bash

   git clone https://github.com/DLR-SC/geoml.git
   cd geoml

On Windows, run the commands from a terminal that can see the Visual Studio C++
compiler, for example **x64 Native Tools Command Prompt for VS** or
**Developer PowerShell for Visual Studio**.

Windows: Build with Pixi
------------------------

Install Pixi first, then run the following commands from the geoml repository
root:

.. code-block:: powershell

   pixi install

Configure a Release build without Python bindings:

.. code-block:: powershell

   pixi run cmake -S . -B build-release -G "Visual Studio 17 2022" -A x64 `
     -DCMAKE_BUILD_TYPE=Release `
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF `
     -DGEOML_BUILD_TESTS=OFF

Build geoml:

.. code-block:: powershell

   pixi run cmake --build build-release --config Release --parallel

Install geoml into a local folder:

.. code-block:: powershell

   pixi run cmake --install build-release --config Release --prefix C:\data\code\geoml\install

To build Python bindings, configure with
``-DGEOML_BUILD_PYTHON_BINDINGS=ON``:

.. code-block:: powershell

   pixi run cmake -S . -B build-python -G "Visual Studio 17 2022" -A x64 `
     -DCMAKE_BUILD_TYPE=Release `
     -DGEOML_BUILD_PYTHON_BINDINGS=ON `
     -DGEOML_BUILD_TESTS=OFF

   pixi run cmake --build build-python --config Release --parallel
   pixi run cmake --install build-python --config Release --prefix .pixi\envs\default
   pixi run python -c "from geoml import pygeoml; print('geoml Python bindings imported successfully')"

Windows: Build with Conda or Mamba
----------------------------------

Create and activate the Conda environment:

.. code-block:: powershell

   conda env create -f environment.yml
   conda activate geoml-bld

For Python bindings and documentation, install the additional packages:

.. code-block:: powershell

   conda install -c dlr-sc -c conda-forge pythonocc-core swig doxygen sphinx breathe sphinx-rtd-theme m2r "docutils<0.21"

Configure a Release build:

.. code-block:: powershell

   cmake -S . -B build-release -G "Visual Studio 17 2022" -A x64 `
     -DCMAKE_BUILD_TYPE=Release `
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF `
     -DGEOML_BUILD_TESTS=OFF

Build:

.. code-block:: powershell

   cmake --build build-release --config Release --parallel

Install into a local folder:

.. code-block:: powershell

   cmake --install build-release --config Release --prefix C:\data\code\geoml\install

To build and install Python bindings into the active Conda environment:

.. code-block:: powershell

   cmake -S . -B build-python -G "Visual Studio 17 2022" -A x64 `
     -DCMAKE_BUILD_TYPE=Release `
     -DGEOML_BUILD_PYTHON_BINDINGS=ON `
     -DGEOML_BUILD_TESTS=OFF

   cmake --build build-python --config Release --parallel
   cmake --install build-python --config Release --prefix $env:CONDA_PREFIX
   python -c "from geoml import pygeoml; print('geoml Python bindings imported successfully')"

Linux: Build with Pixi
----------------------

Install Pixi first, then run the following commands from the geoml repository
root:

.. code-block:: bash

   pixi install

Configure a Release build without Python bindings:

.. code-block:: bash

   pixi run cmake -S . -B build-release -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF \
     -DGEOML_BUILD_TESTS=OFF

Build geoml:

.. code-block:: bash

   pixi run cmake --build build-release --parallel

Install into a local folder:

.. code-block:: bash

   pixi run cmake --install build-release --prefix "$PWD/install"

To build Python bindings:

.. code-block:: bash

   pixi run cmake -S . -B build-python -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=ON \
     -DGEOML_BUILD_TESTS=OFF

   pixi run cmake --build build-python --parallel
   pixi run cmake --install build-python --prefix .pixi/envs/default
   pixi run python -c "from geoml import pygeoml; print('geoml Python bindings imported successfully')"

Linux: Build with Conda or Mamba
--------------------------------

Create and activate the Conda environment:

.. code-block:: bash

   conda env create -f environment.yml
   conda activate geoml-bld

For Python bindings and documentation, install the additional packages:

.. code-block:: bash

   conda install -c dlr-sc -c conda-forge pythonocc-core swig doxygen sphinx breathe sphinx-rtd-theme m2r "docutils<0.21"

Configure a Release build:

.. code-block:: bash

   cmake -S . -B build-release -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF \
     -DGEOML_BUILD_TESTS=OFF

Build:

.. code-block:: bash

   cmake --build build-release --parallel

Install into a local folder:

.. code-block:: bash

   cmake --install build-release --prefix "$PWD/install"

To build and install Python bindings into the active Conda environment:

.. code-block:: bash

   cmake -S . -B build-python -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=ON \
     -DGEOML_BUILD_TESTS=OFF

   cmake --build build-python --parallel
   cmake --install build-python --prefix "$CONDA_PREFIX"
   python -c "from geoml import pygeoml; print('geoml Python bindings imported successfully')"

Build Configurations
--------------------

CMake supports different build configurations. The most important ones are:

.. list-table::
   :header-rows: 1

   * - Configuration
     - Use case
   * - ``Release``
     - Optimized build for normal use. This is the recommended default.
   * - ``Debug``
     - Debug build with debug symbols and less optimization. Useful when
       debugging geoml itself.
   * - ``RelWithDebInfo``
     - Optimized build with debug information. Often a good choice when you want
       useful debug symbols without the full cost of a Debug build.
   * - ``MinSizeRel``
     - Optimized for smaller binaries.

On Windows with Visual Studio, the generator is usually **multi-config**. You
select the configuration during build and install:

.. code-block:: powershell

   cmake --build build-release --config Release --parallel
   cmake --install build-release --config Release --prefix C:\data\code\geoml\install

On Linux with Ninja, the generator is usually **single-config**. You select the
configuration when configuring:

.. code-block:: bash

   cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
   cmake --build build-release --parallel

For Python bindings, ``Release`` or ``RelWithDebInfo`` is usually easier than a
full ``Debug`` build, because Python and Conda packages are normally built as
Release libraries.

CMake Options
-------------

Important project options:

.. list-table::
   :header-rows: 1

   * - Option
     - Description
     - Default
   * - ``GEOML_BUILD_PYTHON_BINDINGS``
     - Build the Python bindings.
     - ``OFF``
   * - ``GEOML_BUILD_TESTS``
     - Build the C++ test suite.
     - ``OFF``
   * - ``GEOML_BUILD_EXAMPLES``
     - Build C++ examples if the examples directory is available.
     - ``OFF``
   * - ``GEOML_BUILD_DOCS``
     - Build the Doxygen/Sphinx documentation.
     - ``OFF``
   * - ``GEOML_INSTALL_INTERNAL_API``
     - Install geoml's internal C++ headers and internal CMake package.
     - ``OFF``

CMake Targets
-------------

Common targets are:

.. list-table::
   :header-rows: 1

   * - Target or command
     - Meaning
   * - ``cmake --build <build-dir>``
     - Build the default target, including the geoml library and all enabled
       subtargets.
   * - ``geoml``
     - Build only the main geoml C++ library target.
   * - ``Sphinx``
     - Build the HTML documentation. Available when ``GEOML_BUILD_DOCS=ON``.
   * - ``Doxygen``
     - Build the Doxygen XML documentation used by Sphinx. Available when
       ``GEOML_BUILD_DOCS=ON``.
   * - ``geoml-apitest``
     - Build the C++ API test executable. Available when
       ``GEOML_BUILD_TESTS=ON``.
   * - ``install``
     - Install the built files into the selected install prefix.
   * - ``package``
     - Create a CPack package from the build, using the configured CPack
       generator.

Examples:

.. code-block:: bash

   cmake --build build-release --target geoml --parallel
   cmake --build build-docs --target Sphinx --parallel
   cmake --build build-release --target package

Build Documentation
-------------------

Configure with documentation enabled:

.. code-block:: bash

   cmake -S . -B build-docs -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_DOCS=ON

Build the HTML documentation:

.. code-block:: bash

   cmake --build build-docs --target Sphinx --parallel

The HTML output is generated in:

.. code-block:: text

   build-docs/docs/html/index.html

Use geoml from C++ After Installing
-----------------------------------

For C++ users, installing geoml is the cleanest workflow. After installation,
another CMake project can find geoml through ``find_package``.

Minimal ``CMakeLists.txt`` for a downstream C++ project:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.15)
   project(geoml_user LANGUAGES CXX)

   set(CMAKE_CXX_STANDARD 17)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)

   find_package(geoml CONFIG REQUIRED)

   add_executable(geoml_user main.cpp)
   target_link_libraries(geoml_user PRIVATE geoml)

Minimal ``main.cpp``:

.. code-block:: cpp

   #include <vector>
   #include <gp_Pnt.hxx>
   #include <Geom_BSplineCurve.hxx>
   #include <geoml/curves/curves.h>

   int main()
   {
       std::vector<gp_Pnt> points {
           gp_Pnt(0.0, 0.0, 0.0),
           gp_Pnt(1.0, 0.0, 0.0),
           gp_Pnt(2.0, 0.0, 1.0)
       };

       Handle(Geom_BSplineCurve) curve =
           geoml::interpolate_points_to_b_spline_curve(points);

       return curve.IsNull() ? 1 : 0;
   }

Configure the downstream project by pointing CMake to the geoml installation:

.. code-block:: bash

   cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/geoml/install
   cmake --build build --parallel

Alternatively, point directly to the package configuration directory:

.. code-block:: bash

   cmake -S . -B build -Dgeoml_DIR=/path/to/geoml/install/lib/cmake/geoml

On Windows, if the program cannot find DLLs at runtime, add the geoml
``bin`` directory and the active Conda/Pixi environment's binary directory to
``PATH``.

Use geoml from Python After Installing
--------------------------------------

If you installed the Python bindings into the active Conda or Pixi environment,
test the import with:

.. code-block:: bash

   python -c "from geoml import pygeoml; print('OK')"

Minimal Python example:

.. code-block:: python

   from geoml import pygeoml
   from geoml.occ_helpers import containers
   from OCC.Core.gp import gp_Pnt

   points = [
       gp_Pnt(0.0, 0.0, 0.0),
       gp_Pnt(1.0, 0.0, 0.0),
       gp_Pnt(2.0, 0.0, 1.0),
   ]

   curve = pygeoml.interpolate_points_to_b_spline_curve(
       containers.point_vector(points),
       2,
   )

   print(curve)

If you installed into a custom prefix instead of the active environment, Python
may not find the package automatically. In that case, add the installed
``site-packages`` directory to ``PYTHONPATH``.

Typical Windows example:

.. code-block:: powershell

   $env:PYTHONPATH = "C:\data\code\geoml\install\Lib\site-packages;$env:PYTHONPATH"

Typical Linux example, adjust the Python version in the path:

.. code-block:: bash

   export PYTHONPATH="$PWD/install/lib/python3.11/site-packages:$PYTHONPATH"

Build and Install: What Is the Difference?
------------------------------------------

Building means compiling the source code into libraries and executables inside a
build directory:

.. code-block:: bash

   cmake --build build-release --parallel

Installing means copying the built results into a clean install prefix:

.. code-block:: bash

   cmake --install build-release --prefix "$PWD/install"

As a rule of thumb:

* Build when you want to compile or test geoml.
* Install when another C++ project or Python environment should use geoml.

Troubleshooting
---------------

If CMake cannot find OpenCASCADE, make sure that the Conda or Pixi environment
is active and that OpenCASCADE is installed in that environment.

On Windows, make sure that you use an x64 Visual Studio generator:

.. code-block:: powershell

   cmake -S . -B build-release -G "Visual Studio 17 2022" -A x64

If CMake still cannot find a dependency, delete the build directory and
configure again. CMake caches dependency paths inside the build directory.
