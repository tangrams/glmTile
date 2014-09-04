//
//  glmLabeledFeature.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#include "glmLabeledFeature.h"

glmLabeledFeature::glmLabeledFeature():
type(LABEL_LINE),
bVisible(false)
{
}

glmLabeledFeature::~glmLabeledFeature(){
}

void glmLabeledFeature::updateProjection(){
    if (type == LABEL_LINE || type == LABEL_AREA){
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
    }
    
    if ( type == LABEL_POINT || type == LABEL_AREA){
        glm::ivec4 viewport;
        glm::mat4x4 mvmatrix, projmatrix;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
        glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
        
        projectedCentroid = glm::project(centroid, mvmatrix, projmatrix, viewport);
        bVisible = projectedCentroid.z >= 0.0 && projectedCentroid.z <= 1.0;
    }
}

void glmLabeledFeature::draw(){
    if (type == LABEL_AREA ){
        text.drawOnPosition(projectedCentroid,5.0,QUARTER_PI);
    } else if (type == LABEL_LINE){
        text.drawOnLine(projectedPolyline,0.5,0.5);
    } else if ( type == LABEL_POINT){
        text.drawOnPosition(projectedCentroid,5.0,HALF_PI);
    }
}