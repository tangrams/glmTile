//
//  glmLabelManager.h
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#pragma once

#include "glmFont.h"

#include "glmTile.h"
#include "glmFeatureLabelLine.h"
#include "glmFeatureLabelPoint.h"

class glmLabelManager {
public:
    
    glmLabelManager();
    virtual ~glmLabelManager();
    
    void setFont(glmFont *_font);
    void setFont(glmFontRef &_font);
    
    void updateFont();
    void updateProjection();
    void updateOcclusions(float *_depthBuffer, int _width, int _height);
    
    void draw();
    
    std::vector<glmFeatureLabelLineRef> lineLabels;
    std::vector<glmFeatureLabelPointRef> pointLabels;
    
private:
    
    glmFontRef  m_font;
    
    bool m_bFontChanged;
};