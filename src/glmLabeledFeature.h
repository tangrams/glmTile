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
#include "glmLabelPoint.h"

class glmLabeledFeature : public glmFeature{
public:
    
    glmLabeledFeature():bVisible(false){};
    virtual ~glmLabeledFeature(){};
    
    virtual void updateProjection() = 0;
    virtual void draw() = 0;

    glmText         text;
    bool            bVisible;
};

class glmLabeledFeaturePoint : public glmLabeledFeature{
public:
    
    glmLabeledFeaturePoint():centroid(0.0,0.0,0.0){};
    virtual ~glmLabeledFeaturePoint(){};
    
    void updateProjection();
    void draw();
    
    glmLabelPoint   labelAnchoir;
    glm::vec3       centroid;
};

class glmLabeledFeatureLine : public glmLabeledFeature{
public:
    
    glmLabeledFeatureLine(){};
    virtual ~glmLabeledFeatureLine(){};
    
    void updateProjection();
    void draw();
    
    glmPolyline     polyline;
    glmPolyline     projectedPolyline;
};

typedef std::tr1::shared_ptr<glmLabeledFeature> glmLabeledFeatureRef;
typedef std::tr1::shared_ptr<glmLabeledFeaturePoint> glmLabeledFeaturePointRef;
typedef std::tr1::shared_ptr<glmLabeledFeatureLine> glmLabeledFeatureLineRef;