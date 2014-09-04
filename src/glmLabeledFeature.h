//
//  glmLabeledFeature.h
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#pragma once

#include "glmFeature.h"

#include "glmText.h"
#include "glmPolyline.h"


class glmLabeledFeature : public glmFeature{
public:
    
    glmLabeledFeature();
    virtual ~glmLabeledFeature();
    
    void updateProjection();
    void draw();
    
    glmPolyline     polyline;
    glmPolyline     projectedPolyline;
    
    glm::vec3       centroid;
    glm::vec3       projectedCentroid;
    
    glmText         text;
    LabelType       type;
    
    bool            bVisible;
    
private:
    
};

typedef std::tr1::shared_ptr<glmLabeledFeature> glmLabeledFeatureRef;