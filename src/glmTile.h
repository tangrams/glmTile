//
//  glmTile.h
//
//  Created by Patricio Gonzalez Vivo on 8/27/14.
//
//

#pragma once

#include "glmFeature.h"
#include "glmLabeledFeature.h"

#include <map>

class glmTile {
public:
    
    glmTile();
    
    bool load(int _tileX, int _tileY, int _zoom);
    void draw();
    
    std::map< std::string, std::vector<glmFeatureRef> > layers;
    std::vector<glmLabeledFeatureRef> labelFeatures;
    
    int     tileX, tileY, zoom;
};