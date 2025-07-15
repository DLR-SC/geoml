#ifndef TOPOALGORITHMS_H
#define TOPOALGORITHMS_H
/*
* SPDX-License-Identifier: Apache-2.0
* SPDX-FileCopyrightText: 2020 German Aerospace Center (DLR)
*/

#include "geoml_internal.h"
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <Geom_BSplineSurface.hxx>
#include <vector>

namespace geoml
{

class TopoAlgorithms
{
public:

    /**
     * @brief Cuts all faces of shell at the specified parameters.
     * @param shape The shape to be cut
     * @param uparams  U-Parameters to cut
     * @param vparams  V-Parameters to cut
     * @return The shape at cutted params
     */
    GEOML_EXPORT static TopoDS_Shape CutShellAtUVParameters(const TopoDS_Shape &shape, std::vector<double> uparams, std::vector<double> vparams);

    /**
     * @brief Cuts the surface at the specified parameters into a shell
     * @param surface The surface to be cut
     * @param uparams  U-Parameters to cut
     * @param vparams  V-Parameters to cut
     * @return The surface cut into a shell
     */
    GEOML_EXPORT static TopoDS_Shell CutSurfaceAtUVParameters(Handle(Geom_Surface) surface, std::vector<double> uparams, std::vector<double> vparams);

    GEOML_EXPORT static void CutSurfaceAtUVParameters(Handle(Geom_Surface) surface, std::vector<double> uparams, std::vector<double> vparams, TopoDS_Shell& shell);


    /**
     * @brief Cuts the surface at c1 discontinuities into a shell
     * @param shape The shape to be cut
     * @return The cutted shape
     */
    GEOML_EXPORT static TopoDS_Shape CutShellAtKinks(const TopoDS_Shape &shape);

    /**
     * @brief Cuts the surface at c1 discontinuities into a shell
     * @param surface The surface to be cut
     * @return The surface cut into a shell
     */
    GEOML_EXPORT static TopoDS_Shell CutSurfaceAtKinks(Handle(Geom_BSplineSurface) surface);

    /**
     * @brief Cuts the surface at c1 discontinuities. The resulting faces are added to the shell
     * @param surface The surface to be cut
     * @param shell The shell to add the faces
    */
    GEOML_EXPORT static void CutSurfaceAtKinks(Handle(Geom_BSplineSurface) surface, TopoDS_Shell& shell);

    /**
     * @brief Checks, whether the wire is degenereated, i.e. infinitesimal small
     */
    GEOML_EXPORT static bool IsDegenerated(const TopoDS_Wire& wire);
};

} // namespace geoml

#endif // TOPOALGORITHMS_H
