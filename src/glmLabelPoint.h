//
//  glmLabelPoint.h
//
//  Created by Patricio Gonzalez Vivo on 9/2/14.
//
//

#pragma once

#include "glmTools.h"

#include "glmRectangle.h"
#include "glmText.h"

class glmLabelPoint : public glm::vec3 {
public:
    
    glmLabelPoint();
    glmLabelPoint(const glm::vec3 &_pos, const glmRectangle &_textBoundingBox );
    virtual ~glmLabelPoint();
    
    void    setPosition(const glm::vec3 &_pos);
    void    setLabelAngle(const double &_radiant);
    void    setLabelMargin(const float &_margin);
    void    setLabelBoundingBox(const glmRectangle &_textBoundingBox );
    
    bool    isOver(const glmLabelPoint &_other);
    bool    isInside(const glm::vec3 &_point);
    
    void    drawText(glmText &_text);
    void    drawPoint(const float &_width = 3.5);
    void    drawLabel(const float &_coorners = 0.0); // 0.0 = draw all border
    
    glmRectangle getLabel() const;
    
private:
    void    update();
    
    glmRectangle    label;
    float           margin;
    double          angle;
    
    bool    bChanged;
};