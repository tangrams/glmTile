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
    
    void addLabelsFrom(glmTile &_tile);
    
    void setFont(FTFont *_font);
    
    void updateProjection();
    
    void draw();
    
    std::vector<glmLabeledFeatureRef> labels;
    
private:
    FTFont  *font;
    
    bool bFontChanged;
};