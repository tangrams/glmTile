//
//  glmLabelManager.h
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#pragma once

#include "ftgl.h"

#include <glmTile.h>
#include <glmLabeledFeature.h>

class glmLabelManager {
public:
    
    glmLabelManager();
    virtual ~glmLabelManager();
    
    void setFont(FTFont *_font);
    
    void updateProjection();
    
    void draw();
    
    std::vector<glmLabeledFeatureLineRef> lineLabels;
    std::vector<glmLabeledFeaturePointRef> pointLabels;
    
private:
    FTFont  *font;
    
    bool bFontChanged;
};