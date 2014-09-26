//
//  glmAnchorLine.h
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/26/14.
//
//

#pragma once

#include "glmFastPolyline.h"

class glmAnchorLine : public glmFastPolyline {
public:
    
    glmAnchorLine();
    
    void    clear();
    
    std::vector<float>  marks;
    glm::vec3   originalCentroid;
    bool        bLetterByLetter;
};