//
//  glmAnchorLine.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/26/14.
//
//

#include "glmAnchorLine.h"

glmAnchorLine::glmAnchorLine():originalCentroid(0.0,0.0,0.0),bLetterByLetter(false){
};

glmAnchorLine::glmAnchorLine(const glmPolyline &_poly):originalCentroid(0.0,0.0,0.0),bLetterByLetter(false){
    for (int i = 0; i < _poly.size(); i++) {
        add(_poly[i]);
    }
}

glmAnchorLine::glmAnchorLine(const std::vector<glm::vec3> &_points):originalCentroid(0.0,0.0,0.0),bLetterByLetter(false){
    for (auto &it : _points) {
        add(it);
    }
}

glmAnchorLine::~glmAnchorLine(){
};

void glmAnchorLine::clear(){
    glmSmartLine::clear();
    marks.clear();
}