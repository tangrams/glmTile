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
    virtual ~glmTile(){}
    
    bool load(int _tileX, int _tileY, int _zoom);
    
    std::map< std::string, std::vector<glmFeatureRef> > byId;
    std::map< std::string, std::vector<glmFeatureRef> > byLayers;

    std::vector<glmLabeledFeatureRef> labelFeatures;
    
    void    renderLayer(const std::string &_layerName);
    void    renderLayer(const std::vector< std::string > &_layersNames);
    
    int     tileX, tileY, zoom;
};