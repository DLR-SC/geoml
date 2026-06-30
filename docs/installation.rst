.. _installation:

Installation and Building
=========================

There are three ways to obtain geoml:

1. **Conda package from the DLR-SC Conda channel.**

   This package is planned, but still work in progress.

2. **Build from source.**

   This is the recommended path for developers and for users who need the
   current repository state, Python bindings, tests, or documentation.

3. **GitHub release package.**

   This is planned as an automatically generated package from GitHub CI. The
   documentation is deployed through GitHub Pages; binary/source packages should
   be attached to GitHub Releases.

Conda Package
-------------
TODO

GitHub Release Package
----------------------
TODO (also change order and put this after the building from source section)

Building from Source
--------------------

Build Dependencies
------------------

Before giving a detailed step by step building/installation instructions, we mention that the minimal requirement for a C++ build you need:

* Git
* CMake 3.15 or newer
* a C++17 compiler
* OpenCASCADE

For Python bindings you additionally (to the minimal requirement) need:

* Python 3.10
* PythonOCC
* SWIG
* setuptools

For documentation you additionally (to the minimal requirement) need:

* Doxygen
* Sphinx
* Breathe
* sphinx-rtd-theme
* m2r
* docutils < 0.21

Install Pixi or Conda
---------------------

We recommend using either Pixi or Conda.

Pixi can be installed from the official
`Pixi installation page <https://pixi.prefix.dev/latest/#installation>`_.

On Windows, download the Windows installer from that page, run it, restart the
terminal and check the installation with:

.. code-block:: batch

   pixi --version

On Linux, follow the official installation command:

.. code-block:: bash

   curl -fsSL https://pixi.sh/install.sh | sh

After installation, restart the terminal and check:

.. code-block:: bash

   pixi --version

For Conda, this documentation recommends Miniforge, because it provides Conda
with the conda-forge ecosystem. Download it from the official
`Miniforge releases page <https://github.com/conda-forge/miniforge/releases/latest>`_.
The general Conda installation documentation is available in the
`Conda install guide <https://docs.conda.io/projects/conda/en/latest/user-guide/install/index.html>`_.

On Windows, download ``Miniforge3-Windows-x86_64.exe``, run the installer,
open the Miniforge Prompt and check:

.. code-block:: batch

   conda --version

On Linux, download the matching Miniforge shell installer from the release page,
run it, restart the terminal and check:

.. code-block:: bash

   conda --version

Clone the Repository
--------------------

.. code-block:: bash

   git clone https://github.com/DLR-SC/geoml.git
   cd geoml

On Windows, use a terminal with the x64 compiler available, for example
**x64 Native Tools Command Prompt for VS**.

Build with Pixi
---------------

Pixi uses the dependencies from ``pyproject.toml``.
Run the following commands from the geoml repository root, i.e. the directory
that contains ``CMakeLists.txt`` and ``pyproject.toml``.

Windows:

.. code-block:: batch

   pixi install
   pixi run cmake -S . -B build-release -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF ^
     -DGEOML_BUILD_TESTS=OFF
   pixi run cmake --build build-release --parallel

Linux:

.. code-block:: bash

   pixi install
   pixi run cmake -S . -B build-release -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF \
     -DGEOML_BUILD_TESTS=OFF
   pixi run cmake --build build-release --parallel

Build with Conda
----------------

From the geoml repository root, create and activate the Conda environment:

.. code-block:: bash

   conda env create -f environment.yml
   conda activate geoml-bld

If you want to use ninja as CMake generator, install it with:

.. code-block:: bash

   conda install ninja

If you want to build Python bindings, also install:

.. code-block:: bash

   conda install swig

If you want to build the documentation, also install:

.. code-block:: bash

   conda install -c conda-forge doxygen sphinx breathe sphinx-rtd-theme m2r "docutils<0.21"

Run the following build commands from the geoml repository root, i.e. the
directory that contains ``CMakeLists.txt``.

Windows:

.. code-block:: batch

   cmake -S . -B build-release -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF ^
     -DGEOML_BUILD_TESTS=OFF
   cmake --build build-release --parallel

Linux:

.. code-block:: bash

   cmake -S . -B build-release -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=OFF \
     -DGEOML_BUILD_TESTS=OFF
   cmake --build build-release --parallel

Install After Building
----------------------

Building compiles geoml in the build directory. Installing copies the built
files to a clean install prefix. Enter the install command in the geoml
repository root, for example ``C:\data\code\geoml`` or ``~/geoml``. Do not
enter it from inside ``build-release`` or another build directory.

Windows:

.. code-block:: batch

   cmake --install build-release --prefix C:\data\code\geoml\install

Linux:

.. code-block:: bash

   cmake --install build-release --prefix "$PWD/install"

Build Python Bindings
---------------------

Enable Python bindings with ``GEOML_BUILD_PYTHON_BINDINGS=ON``. Run the
configure, build and install commands from the geoml repository root.

Windows with Pixi:

.. code-block:: batch

   pixi run cmake -S . -B build-python -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_PYTHON_BINDINGS=ON ^
     -DGEOML_BUILD_TESTS=OFF
   pixi run cmake --build build-python --parallel
   pixi run cmake --install build-python --prefix .pixi\envs\default
   pixi run python -c "from geoml import pygeoml; print('OK')"

Linux with Pixi:

.. code-block:: bash

   pixi run cmake -S . -B build-python -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=ON \
     -DGEOML_BUILD_TESTS=OFF
   pixi run cmake --build build-python --parallel
   pixi run cmake --install build-python --prefix .pixi/envs/default
   pixi run python -c "from geoml import pygeoml; print('OK')"

With Conda, use the same CMake options without ``pixi run`` and install into
the active Conda environment.

Windows:

.. code-block:: batch

   cmake -S . -B build-python -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_PYTHON_BINDINGS=ON ^
     -DGEOML_BUILD_TESTS=OFF
   cmake --build build-python --parallel
   cmake --install build-python --prefix %CONDA_PREFIX%
   python -c "from geoml import pygeoml; print('OK')"

Linux:

.. code-block:: bash

   cmake -S . -B build-python -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_PYTHON_BINDINGS=ON \
     -DGEOML_BUILD_TESTS=OFF
   cmake --build build-python --parallel
   cmake --install build-python --prefix "$CONDA_PREFIX"
   python -c "from geoml import pygeoml; print('OK')"

Build and Run Tests
-------------------

Enable tests with ``GEOML_BUILD_TESTS=ON``. Run the following commands from the
geoml repository root.

Windows:

.. code-block:: batch

   cmake -S . -B build-tests -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_TESTS=ON
   cmake --build build-tests --parallel
   ctest --test-dir build-tests --output-on-failure

Linux:

.. code-block:: bash

   cmake -S . -B build-tests -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_TESTS=ON
   cmake --build build-tests --parallel
   ctest --test-dir build-tests --output-on-failure

The test build contains:

* ``geoml-unittests`` for unit tests
* ``geoml-apitest`` for API tests

Build Documentation
-------------------

Run the following commands from the geoml repository root.

.. code-block:: bash

   cmake -S . -B build-docs -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_DOCS=ON
   cmake --build build-docs --target Sphinx --parallel

The generated HTML pages are written to:

.. code-block:: text

   build-docs/docs/html/index.html

Build Configurations
--------------------

Common CMake configurations are:

.. list-table::
   :header-rows: 1

   * - Configuration
     - Use
   * - ``Release``
     - Optimized build for normal use.
   * - ``Debug``
     - Debug build with less optimization.

The commands above use ``Release`` as an example configuration. Use another
configuration, such as ``Debug``, if that better matches your use case.

With Ninja, choose the configuration during configuration:

.. code-block:: bash

   cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release

CMake Options
-------------

.. list-table::
   :header-rows: 1

   * - Option
     - Description
     - Default
   * - ``GEOML_BUILD_PYTHON_BINDINGS``
     - Build Python bindings.
     - ``OFF``
   * - ``GEOML_BUILD_TESTS``
     - Build unit and API tests.
     - ``OFF``
   * - ``GEOML_BUILD_EXAMPLES``
     - Build examples, if available.
     - ``OFF``
   * - ``GEOML_BUILD_DOCS``
     - Build Doxygen/Sphinx documentation.
     - ``OFF``
   * - ``GEOML_INSTALL_INTERNAL_API``
     - Install internal C++ headers and CMake package files.
     - ``OFF``

Use geoml from C++
------------------

Use the same environment that contains the installed geoml package and its
dependencies. For Conda, activate the environment first:

.. code-block:: batch

   conda activate geoml-bld

For Pixi, run the CMake commands through ``pixi run`` from the geoml repository
or point ``CMAKE_PREFIX_PATH`` to the Pixi environment where geoml was
installed, for example ``C:\data\code\geoml\.pixi\envs\default``.

Create a small project directory outside the geoml source tree, for example
``geoml-cpp-example``:

.. code-block:: text

   geoml-cpp-example/
     CMakeLists.txt
     main.cpp

Minimal ``CMakeLists.txt``:

.. code-block:: cmake

   cmake_minimum_required(VERSION 3.15)
   project(geoml_cpp_example LANGUAGES CXX)

   set(CMAKE_CXX_STANDARD 17)
   set(CMAKE_CXX_STANDARD_REQUIRED ON)

   find_package(OpenCASCADE CONFIG REQUIRED)
   find_package(geoml CONFIG REQUIRED)

   add_executable(example main.cpp)
   target_include_directories(example PRIVATE ${OpenCASCADE_INCLUDE_DIR})
   target_link_libraries(example PRIVATE geoml)

Minimal ``main.cpp``:

.. code-block:: cpp

   #include <vector>

   #include <Geom_BSplineCurve.hxx>
   #include <gp_Pnt.hxx>

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

OpenCASCADE headers usually use the ``.hxx`` extension. For example, include
``Geom_BSplineCurve.hxx`` and not ``Geom_BSplineCurve.h``.

Configure and build the example from the ``geoml-cpp-example`` directory.

If geoml was installed into a Conda environment:

.. code-block:: batch

   conda activate geoml-bld
   cmake -S . -B build -G Ninja "-DCMAKE_PREFIX_PATH=%CONDA_PREFIX%;%CONDA_PREFIX%\Library"
   cmake --build build
   build\example.exe

If geoml was installed into the default Pixi environment:

.. code-block:: batch

   pixi run cmake -S . -B build -G Ninja "-DCMAKE_PREFIX_PATH=C:\data\code\geoml\.pixi\envs\default;C:\data\code\geoml\.pixi\envs\default\Library"
   pixi run cmake --build build
   build\example.exe

If geoml was installed into ``C:\data\code\geoml\install``:

.. code-block:: batch

   cmake -S . -B build -G Ninja "-DCMAKE_PREFIX_PATH=C:\data\code\geoml\install;%CONDA_PREFIX%\Library"
   cmake --build build
   build\example.exe

On Linux, use the same pattern with the install prefix:

.. code-block:: bash

   cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/geoml/install
   cmake --build build
   ./build/example

Use geoml from Python
---------------------

After installing the Python bindings into the active environment:

.. code-block:: python

   from geoml import pygeoml

Troubleshooting
---------------

If ``conda env create -f environment.yml`` fails after a partial or outdated
environment was already created, remove the environment and recreate it from the
current ``environment.yml``:

.. code-block:: batch

   conda deactivate
   conda env remove -n geoml-bld
   conda env create -f environment.yml
   conda activate geoml-bld

The geoml environment pins Python to a version that is compatible with the
available PythonOCC packages. Do not install ``pythonocc-core`` separately into
an already-created environment with an unconstrained Python version.

If CMake cannot find OpenCASCADE, make sure the Conda or Pixi environment is
active. On Windows, also make sure that the x64 compiler is available before
running CMake:

.. code-block:: batch

   cmake -S . -B build-release -G Ninja

If dependency paths look wrong, delete the build directory and configure again.

If compilation fails with an error such as ``C1083`` and
``Geom_BSplineCurve.h`` cannot be opened, check the include name in
``main.cpp``. The OpenCASCADE header is named ``Geom_BSplineCurve.hxx``.
