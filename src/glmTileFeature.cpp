//
//  glmTileFeature.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/3/14.
//
//

#include "glmTileFeature.h"

void glmLabeledFeature::updateProjection(){
    if (type == LABEL_LINE || type == LABEL_AREA){
        projectedPolyline = polyline.getProjected();
    }
    
    if ( type == LABEL_POINT || type == LABEL_AREA){
        glm::ivec4 viewport;
        glm::mat4x4 mvmatrix, projmatrix;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
        glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
        
        projectedCentroid = glm::project(centroid, mvmatrix, projmatrix, viewport);
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