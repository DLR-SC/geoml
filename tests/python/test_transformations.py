from geoml import pygeoml
from OCC.Core.gp import gp_Pnt, gp_Vec, gp_GTrsf, gp_XYZ 
from OCC.Core.TopoDS import TopoDS_Solid
from OCC.Core.Geom import Geom_Surface

import pytest 


###########################################################################
##################### test geoml/transformations/Transform.h ##############
###########################################################################


def test_Transform_h():
    # test: constructing a Transform object

    transf = pygeoml.Transform()
    assert type(transf) is pygeoml.Transform

    g_trans = gp_GTrsf()
    translation = gp_XYZ(0.0, 0.0, 1.0)    
    g_trans.SetTranslationPart(translation)

    transf_1 = pygeoml.Transform(g_trans)
    assert type(transf_1) is pygeoml.Transform

    vec = gp_Vec(1.0, 2.0, 3.0)
    
    transf_2 = pygeoml.Transform(vec)
    assert type(transf_2) is pygeoml.Transform


    # test: void pre_multiply(const Transform& aTrans);
    transf_1.pre_multiply(transf_2)
    assert type(transf_1) is pygeoml.Transform


    # test: TopoDS_Shape operator()(const TopoDS_Shape& shape) const;
    dx = 1.0
    dy = 2.0
    dz = 4.0

    box_shape = pygeoml.create_box(dx, dy, dz)

    box = box_shape.shape()

    box_transformed = transf_2(box)
    assert type(box_transformed) is TopoDS_Solid


    # test: gp_Pnt operator()(const gp_Pnt& point) const;
    point = gp_Pnt(0.0, 1.0, 2.0)

    point_transformed = transf_1(point)
    assert type(point_transformed) is gp_Pnt

    # test:  Handle(Geom_Surface) operator()(const Handle(Geom_Surface)& surf) const;
    x = gp_Pnt(0,0,0)
    y = gp_Pnt(0.5,0,0)       
    z = gp_Pnt(1,1,0)
    w = gp_Pnt(0,3,0)
    
    surf = pygeoml.create_surface(x,y,z,w)

    surf_transformed = transf_2(surf)

    face = pygeoml.SurfaceToFace(surf_transformed, 1e-5)

    # test: gp_Vec operator()(const gp_Vec& vec) const;
    vec_transformed = transf_1(vec)
    assert type(vec_transformed) is gp_Vec

    # test: Transform operator*(const Transform& a, const Transform& b);

    transf_3 = transf_1 * transf_2 
    assert type(transf_3) is pygeoml.Transform

    surf_transf_2 = transf_3(surf_transformed)
    assert type(surf_transf_2) is Geom_Surface



