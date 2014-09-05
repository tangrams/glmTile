//
//  glmTile.cpp
//
//  Created by Patricio Gonzalez Vivo on 8/27/14.
//
//

#include "glmTile.h"

#include "glmGeometryBuilder.h"

glmTile::glmTile():
tileX(-1), tileY(-1), zoom(-1)
{
}

bool glmTile::load(int _tileX, int _tileY, int _zoom){
    tileX = _tileX;
    tileY = _tileY;
    zoom = _zoom;
    
    glmGeometryBuilder builder;
    builder.setOffset(19294,24642,16);
    builder.load(tileX, tileY, zoom, *this);
}

void glmTile::renderLayer(const std::string &_layerName){
    for (auto &it : byLayers[_layerName]){
        it->draw();
    }
}

void glmTile::renderLayer(const std::vector< std::string > &_layersNames){
    for (auto &it : _layersNames ) {
        renderLayer(it);
    }
}