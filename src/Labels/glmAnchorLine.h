//
//  glmAnchorLine.h
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/26/14.
//
//

#pragma once

#include "glmSmartLine.h"

class glmAnchorLine : public glmSmartLine {
public:
    
    glmAnchorLine();
    glmAnchorLine(const glmPolyline &_poly);
    glmAnchorLine(const std::vector<glm::vec3> &_points);
    
    virtual ~glmAnchorLine();
    
    void        clear();
    
    std::vector<float>  marks;
    glm::vec3   originalCentroid;
    bool        bLetterByLetter;
};