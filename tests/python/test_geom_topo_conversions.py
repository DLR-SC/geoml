from geoml import pygeoml
from geoml.occ_helpers import containers
from OCC.Core.gp import gp_Pnt
from OCC.Core.TopoDS import TopoDS_Edge, TopoDS_Face
from OCC.Core.Geom import Geom_Curve
from OCC.Core.TopAbs import TopAbs_EDGE, TopAbs_FACE

import pytest 


###########################################################################
##################### test geoml/geom_topo_conversions/geom_topo_conversions.h 
###########################################################################


def test_geom_topo_conversions_h():
    # test: TopoDS_Face SurfaceToFace(const Handle(Geom_Surface) &surface, Standard_Real tolDegEd);

    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)
    
    srf = pygeoml.create_surface(x,y,z,w)

    face = pygeoml.SurfaceToFace(srf, 1e-4)
    assert type(face) is TopoDS_Face

    # test: TopoDS_Edge CurveToEdge(const Handle(Geom_Curve) &curve);

    p_1 = gp_Pnt(0.0, 0.0, 0.0)
    p_2 = gp_Pnt(1.0, 1.0, 1.0)
    p_3 = gp_Pnt(3.0, 0.0, 6.0)

    point_vec = containers.point_vector([p_1, p_2, p_3])

    curve = pygeoml.interpolate_points_to_b_spline_curve(point_vec)

    edge = pygeoml.CurveToEdge(curve)
    assert type(edge) is TopoDS_Edge

    # test: Handle(Geom_Curve) EdgeToCurve(TopoDS_Edge const& edge);

    edge = pygeoml.CurveToEdge(curve)

    crv = pygeoml.EdgeToCurve(edge)
    assert type(crv) is Geom_Curve
