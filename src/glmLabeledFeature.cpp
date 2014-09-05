//
//  glmLabeledFeature.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/5/14.
//
//

#include "glmLabeledFeature.h"

void glmLabeledFeaturePoint::updateProjection(){
    glm::ivec4 viewport;
    glm::mat4x4 mvmatrix, projmatrix;
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
    glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
    
    labelAnchoir.setPosition(glm::project(centroid, mvmatrix, projmatrix, viewport));
    
    bVisible = labelAnchoir.z >= 0.0 && labelAnchoir.z <= 1.0;
};

void glmLabeledFeaturePoint::draw(){
    labelAnchoir.drawPoint();
    labelAnchoir.drawText(text);
};

void glmLabeledFeatureLine::updateProjection(){
    glm::ivec4 viewport;
    glm::mat4x4 mvmatrix, projmatrix;
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
    glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
    
    projectedPolyline.clear();
    for (int i = 0; i < polyline.size(); i++) {
        glm::vec3 v = glm::project(polyline[i], mvmatrix, projmatrix, viewport);
        if( v.z >= 0.0 && v.z <= 1.0){
            projectedPolyline.add(v);
        }
    }
    
    bVisible = projectedPolyline.size()>0;
};

void glmLabeledFeatureLine::draw(){
    text.drawOnLine(projectedPolyline,0.5,0.5);
}