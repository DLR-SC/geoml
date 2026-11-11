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

This section describes how to build geoml from source with Pixi or Conda on
Windows and Linux, and how to install it, build the Python bindings, run the
tests and build the documentation.

Build Dependencies
^^^^^^^^^^^^^^^^^^

Before giving a detailed step by step building/installation instructions, we mention the minimal requirement for a C++ build:

* Git
* CMake 3.15 or newer
* Ninja (used as CMake generator in all commands below)
* a C++17 compiler: on Windows MSVC (Visual Studio or the Build Tools for
  Visual Studio), on Linux GCC or Clang
* OpenCASCADE 7.6.2

The C++ compiler is not part of the Pixi or Conda environment and must be
installed on the system. On Debian/Ubuntu, for example, install GCC with:

.. code-block:: bash

   sudo apt install build-essential

For Python bindings you additionally (to the minimal requirement) need:

* Python 3.11 (the Pixi environment pins ``>=3.11,<3.12``)
* pythonocc-core 7.6.2 (must match the OpenCASCADE version)
* SWIG
* setuptools (used by CMake to find the Python ``site-packages`` directory)

For documentation you additionally (to the minimal requirement) need:

* Doxygen
* Sphinx
* Breathe
* sphinx-rtd-theme
* m2r
* docutils < 0.21

Install Pixi or Conda
^^^^^^^^^^^^^^^^^^^^^

We recommend using either Pixi or Conda.

Pixi can be installed from the official
`Pixi installation page <https://pixi.prefix.dev/latest/installation/>`_.

On Windows, download the Windows installer from that page, run it, restart the
terminal and check the installation with:

.. code-block:: batch

   pixi --version

On Linux, use the official installation script:

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

On Linux, download and run the matching Miniforge installer:

.. code-block:: bash

   curl -L -O "https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-$(uname)-$(uname -m).sh"
   bash Miniforge3-$(uname)-$(uname -m).sh

Follow the installer prompts and allow it to initialize Conda for your shell.
Then restart the terminal and check:

.. code-block:: bash

   conda --version

Clone the Repository
^^^^^^^^^^^^^^^^^^^^

.. code-block:: bash

   git clone https://github.com/DLR-SC/geoml.git
   cd geoml

On Windows, use a terminal with the x64 compiler available, for example
**x64 Native Tools Command Prompt for VS**. On Linux, a normal terminal is
enough, as long as ``g++`` (or another C++17 compiler) is installed.

Build with Pixi
^^^^^^^^^^^^^^^

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
^^^^^^^^^^^^^^^^

From the geoml repository root, create and activate the Conda environment:

.. code-block:: bash

   conda env create -f environment.yml
   conda activate geoml-bld

All build commands in this documentation use Ninja as CMake generator. Ninja is
not part of ``environment.yml``, so install it into the environment:

.. code-block:: bash

   conda install -c conda-forge ninja

If you want to build Python bindings, also install:

.. code-block:: bash

   conda install -c conda-forge swig

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
^^^^^^^^^^^^^^^^^^^^^^

Building compiles geoml in the build directory. Installing copies the built
files to a clean install prefix. Enter the install command in the geoml
repository root, for example ``C:\data\code\geoml`` or ``~/geoml``. Do not
enter it from inside ``build-release`` or another build directory.

Windows with Pixi:

.. code-block:: batch

   pixi run cmake --install build-release --prefix C:/data/code/geoml/install

Linux with Pixi:

.. code-block:: bash

   pixi run cmake --install build-release --prefix "$PWD/install"

With Conda, run the same commands without ``pixi run`` in the activated
environment.

The install prefix contains the headers in ``include/geoml``, the library in
``lib`` and the CMake package files in ``lib/cmake/geoml``.

Build Python Bindings
^^^^^^^^^^^^^^^^^^^^^

Enable Python bindings with ``GEOML_BUILD_PYTHON_BINDINGS=ON``. Run the
configure, build and install commands from the geoml repository root.

Windows with Pixi:

.. code-block:: batch

   pixi run cmake -S . -B build-python -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_PYTHON_BINDINGS=ON ^
     -DGEOML_BUILD_TESTS=OFF
   pixi run cmake --build build-python --parallel
   pixi run cmake --install build-python --prefix .pixi/envs/default
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
^^^^^^^^^^^^^^^^^^^

Enable tests with ``GEOML_BUILD_TESTS=ON``. Run the following commands from the
geoml repository root.

Windows with Pixi:

.. code-block:: batch

   pixi run cmake -S . -B build-tests -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_TESTS=ON
   pixi run cmake --build build-tests --parallel
   pixi run ctest --test-dir build-tests --output-on-failure

Linux with Pixi:

.. code-block:: bash

   pixi run cmake -S . -B build-tests -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_TESTS=ON
   pixi run cmake --build build-tests --parallel
   pixi run ctest --test-dir build-tests --output-on-failure

``ctest --test-dir`` must point to the build directory in which the tests were
configured with ``GEOML_BUILD_TESTS=ON``. If you enabled the tests in another
build directory, for example ``build-release``, use that name instead of
``build-tests``, here and in the commands below. Otherwise ctest fails with
``Failed to change working directory ... No such file or directory``.

With Conda, activate the environment (``conda activate geoml-bld``) and use the
same commands without ``pixi run``.

Windows with Conda:

.. code-block:: batch

   cmake -S . -B build-tests -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_TESTS=ON
   cmake --build build-tests --parallel
   ctest --test-dir build-tests --output-on-failure

Linux with Conda:

.. code-block:: bash

   cmake -S . -B build-tests -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_TESTS=ON
   cmake --build build-tests --parallel
   ctest --test-dir build-tests --output-on-failure

The test build contains two test executables:

* ``geoml-unittests`` for unit tests, in ``build-tests/tests/unittests``
* ``geoml-apitest`` for API tests, in ``build-tests/tests/apitests``

If you are already inside the build directory, you can omit ``--test-dir``:

.. code-block:: batch

   cd build-tests
   pixi run ctest --output-on-failure

Run the Test Executables Directly
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can also run a test executable directly, for example to see the full
GoogleTest output or to run only some of the tests with ``--gtest_filter``.
Keep two things in mind:

* **Run the executables inside the Pixi or Conda environment.** They need the
  OpenCASCADE shared libraries at runtime, and these libraries are in the
  environment. If you start ``geoml-apitest.exe`` from a plain terminal on
  Windows, a system error dialog says that ``TKBool.dll``, ``TKFillet.dll``,
  ``TKIGES.dll``, ``TKSTEP.dll`` or another ``TK*.dll`` was not found.
* **Start each executable from its own build directory.** The tests read input
  files from ``TestData/`` relative to the current directory. Started from
  anywhere else, they fail with errors such as
  ``BRepTools::Read(guides, "TestData/...", b)``. ctest changes to the right
  directory automatically.

To run the API tests, change into their build directory and start the executable
through ``pixi run``:

.. code-block:: batch

   cd build-tests\tests\apitests
   pixi run ./geoml-apitest.exe

To run the unit tests afterwards, change into their build directory:

.. code-block:: batch

   cd ..\unittests
   pixi run ./geoml-unittests.exe

To run only some of the tests, add a GoogleTest filter, for example:

.. code-block:: batch

   pixi run ./geoml-unittests.exe --gtest_filter=*Curve*

On Linux, use the same commands with forward slashes and without ``.exe``:

.. code-block:: bash

   cd build-tests/tests/apitests
   pixi run ./geoml-apitest
   cd ../unittests
   pixi run ./geoml-unittests

``pixi run`` finds the geoml environment from these subdirectories as well.

Use ``./`` and forward slashes in paths after ``pixi run``, also on Windows. Pixi
runs the command in its own cross-platform shell, which can treat a backslash as
an escape character. A command such as
``pixi run build-tests\tests\apitests\geoml-apitest.exe`` can then fail with
``build-teststestsapitestsgeoml-apitest.exe: command not found``.

Instead of prefixing every command with ``pixi run``, you can activate the
environment once. Then you can start the executables without ``./``:

.. code-block:: batch

   pixi shell
   cd build-tests\tests\apitests
   geoml-apitest.exe
   cd ..\unittests
   geoml-unittests.exe

With Conda, run ``conda activate geoml-bld`` instead of ``pixi shell``.

Build Documentation
^^^^^^^^^^^^^^^^^^^

Enable the documentation with ``GEOML_BUILD_DOCS=ON``. The documentation build
configures the whole project, so it needs OpenCASCADE as well as Doxygen and
Sphinx from the environment. Run the following commands from the geoml
repository root.

Windows with Pixi:

.. code-block:: batch

   pixi run cmake -S . -B build-docs -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     -DGEOML_BUILD_DOCS=ON
   pixi run cmake --build build-docs --target Sphinx --parallel

Linux with Pixi:

.. code-block:: bash

   pixi run cmake -S . -B build-docs -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DGEOML_BUILD_DOCS=ON
   pixi run cmake --build build-docs --target Sphinx --parallel

With Conda, install the documentation tools first (see `Build with Conda`_) and
run the same commands without ``pixi run`` in the activated environment.

The generated HTML pages are written to:

.. code-block:: text

   build-docs/docs/html/index.html

Build Configurations
^^^^^^^^^^^^^^^^^^^^

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

   pixi run cmake -S . -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug

On Windows, a program that links against geoml must use the same configuration
as geoml itself. A ``Debug`` program cannot link against a ``Release`` build of
geoml and fails with linker errors such as ``LNK2038: mismatch detected for
'RuntimeLibrary'``.

CMake Options
^^^^^^^^^^^^^

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
   * - ``GEOML_BUILD_DOCS``
     - Build Doxygen/Sphinx documentation.
     - ``OFF``
   * - ``GEOML_INSTALL_INTERNAL_API``
     - Additionally install the internal C++ headers and the
       ``geoml_internal`` CMake package. The public headers and the ``geoml``
       CMake package are always installed.
     - ``OFF``

``GEOML_BUILD_EXAMPLES`` also exists, but the repository currently contains no
``examples`` directory, so setting it to ``ON`` makes the CMake configuration
fail. Leave it ``OFF``.

Use geoml from C++
------------------

To use geoml in your own C++ project, geoml must first be installed (see
`Install After Building`_ or `Build Python Bindings`_, which installs into the
Pixi or Conda environment). Configure, build and run your project in the same
environment that contains OpenCASCADE, because geoml links against it.

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

Configure, build and run the example from the ``geoml-cpp-example`` directory.
Use the same ``CMAKE_BUILD_TYPE`` as for geoml, ``Release`` in the commands
below. On Windows, a mismatch fails with ``LNK2038`` linker errors (see
`Build Configurations`_).

**With Pixi.** The example directory is outside the geoml repository, so
``pixi run`` cannot find the geoml Pixi environment on its own. Pass the geoml
``pyproject.toml`` with ``--manifest-path``. The examples below assume geoml is
cloned to ``C:\data\code\geoml`` or ``~/geoml``. Use forward slashes in the paths
after ``pixi run``, also on Windows.

Windows, geoml installed into the Pixi environment:

.. code-block:: batch

   pixi run --manifest-path C:/data/code/geoml/pyproject.toml cmake -S . -B build -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     "-DCMAKE_PREFIX_PATH=C:/data/code/geoml/.pixi/envs/default;C:/data/code/geoml/.pixi/envs/default/Library"
   pixi run --manifest-path C:/data/code/geoml/pyproject.toml cmake --build build
   pixi run --manifest-path C:/data/code/geoml/pyproject.toml ./build/example.exe

Linux, geoml installed into the Pixi environment:

.. code-block:: bash

   pixi run --manifest-path ~/geoml/pyproject.toml cmake -S . -B build -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DCMAKE_PREFIX_PATH="$HOME/geoml/.pixi/envs/default"
   pixi run --manifest-path ~/geoml/pyproject.toml cmake --build build
   pixi run --manifest-path ~/geoml/pyproject.toml ./build/example

If geoml was installed into a separate prefix such as ``C:/data/code/geoml/install``
or ``~/geoml/install``, add that prefix to ``CMAKE_PREFIX_PATH``, for example
``"-DCMAKE_PREFIX_PATH=C:/data/code/geoml/install;C:/data/code/geoml/.pixi/envs/default/Library"``
on Windows or ``-DCMAKE_PREFIX_PATH="$HOME/geoml/install;$HOME/geoml/.pixi/envs/default"``
on Linux. OpenCASCADE is still taken from the Pixi environment.

**With Conda.** Activate the environment first. Then ``CONDA_PREFIX`` points to
it.

Windows, geoml installed into the Conda environment:

.. code-block:: batch

   conda activate geoml-bld
   cmake -S . -B build -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     "-DCMAKE_PREFIX_PATH=%CONDA_PREFIX%;%CONDA_PREFIX%\Library"
   cmake --build build
   build\example.exe

Windows, geoml installed into ``C:\data\code\geoml\install``:

.. code-block:: batch

   conda activate geoml-bld
   cmake -S . -B build -G Ninja ^
     -DCMAKE_BUILD_TYPE=Release ^
     "-DCMAKE_PREFIX_PATH=C:\data\code\geoml\install;%CONDA_PREFIX%\Library"
   cmake --build build
   build\example.exe

Linux, geoml installed into the Conda environment or into ``~/geoml/install``:

.. code-block:: bash

   conda activate geoml-bld
   cmake -S . -B build -G Ninja \
     -DCMAKE_BUILD_TYPE=Release \
     -DCMAKE_PREFIX_PATH="$HOME/geoml/install;$CONDA_PREFIX"
   cmake --build build
   ./build/example

The example program returns exit code ``0`` if the curve was created.

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

``environment.yml`` pins OpenCASCADE to 7.6.2, and the solver then picks a
``pythonocc-core`` and a Python version that are compatible with it. The Pixi
environment pins ``pythonocc-core`` 7.6.2 and Python 3.11 explicitly. Do not
install or update ``pythonocc-core``, ``opencascade`` or ``python`` separately in
an already-created environment, because this can pull in incompatible versions.

If CMake cannot find OpenCASCADE, make sure that you run CMake inside the
environment: prefix the command with ``pixi run`` or activate the Conda
environment first.

If CMake cannot find a C++ compiler on Windows, start the build from an
**x64 Native Tools Command Prompt for VS**. You can check that the compiler is
available with:

.. code-block:: batch

   where cl

On Linux, check that ``g++ --version`` works, otherwise install a compiler (see
`Build Dependencies`_).

If ``pixi run`` fails with ``could not find pixi.toml or pyproject.toml with
tool.pixi``, the current directory is not inside the geoml repository. Either
change into the repository or pass ``--manifest-path`` with the path to the geoml
``pyproject.toml`` (see `Use geoml from C++`_).

If dependency paths look wrong, delete the build directory and configure again.

If running a geoml executable on Windows (for example ``geoml-apitest.exe``)
fails with a system error saying that ``TKBool.dll``, ``TKFillet.dll``,
``TKIGES.dll``, ``TKSTEP.dll`` or another ``TK*.dll`` was not found, the
OpenCASCADE libraries are not on ``PATH``. This happens when the executable is
started from a terminal where the Pixi or Conda environment is not active. Run it
through ``pixi run``, from a ``pixi shell``, or after ``conda activate
geoml-bld``. For a single terminal session, you can also add the library
directory to ``PATH`` manually:

.. code-block:: batch

   set PATH=C:\data\code\geoml\.pixi\envs\default\Library\bin;%PATH%

On Linux, the equivalent error is ``error while loading shared libraries:
libTKBool.so...``. Fix it in the same way, by running the executable inside the
activated environment.

If compilation fails with an error such as ``C1083`` and
``Geom_BSplineCurve.h`` cannot be opened, check the include name in
``main.cpp``. The OpenCASCADE header is named ``Geom_BSplineCurve.hxx``.
