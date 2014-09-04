//
//  glmLabelPoint.h
//
//  Created by Patricio Gonzalez Vivo on 9/2/14.
//
//

#pragma once

#include "glmTools.h"

#include "glmRectangle.h"

class glmLabelPoint : public glm::vec3 {
public:
    
    glmLabelPoint(){};
    glmLabelPoint(float _x, float _y, float _text_width);
    
    bool compute(glmLabelPoint &_other);
    
    void update();
    
    void draw(bool _bDebug);
    
    glmRectangle label;
    double      angle;
    float       margin;
    
private:
    float       area;
};