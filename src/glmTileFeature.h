//
//  glmTileFeature.h
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 8/27/14.
//
//

#pragma once

//  TODO: use default one
//
//#include <memory>
#include <tr1/memory>

#include "glmPolyline.h"
#include "glmMesh.h"
#include "glmText.h"

class glmTileFeature {
public:

    glmTileFeature(){};
    virtual ~glmTileFeature(){};
    
    glmMesh         geometry;
    std::string     idString;
    
private:
    
};

typedef std::tr1::shared_ptr<glmTileFeature> glmTileFeatureRef;

class glmLabeledFeature : public glmTileFeature{
public:
    
    glmLabeledFeature():
    type(LABEL_LINE){
    };
    virtual ~glmLabeledFeature(){};
    
    void updateProjection();
    void draw();
    
    glmPolyline     polyline;
    glmPolyline     projectedPolyline;
    
    glm::vec3       centroid;
    glm::vec3       projectedCentroid;
    
    glmText         text;
    LabelType       type;

private:
    
};

typedef std::tr1::shared_ptr<glmLabeledFeature> glmLabeledFeatureRef;