//
//  glmGeometryBuilder.h
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#pragma once

#include "json/json.h"
#include "glmTools.h"

#include "glmTile.h"

class glmGeometryBuilder {
public:
    
    glmGeometryBuilder();
    virtual ~glmGeometryBuilder(){};

    void setCenter(glm::vec3 _center);
    void setCenter(int _tileX, int _tileY, int _zoom);
    
    glmTile getFromFile(std::string _fileName);
    glmTile getFromWeb(int _tileX, int _tileY, int _zoom);
    
    void load(int _tileX, int _tileY, int _zoom, glmTile &_tile);
    void load(Json::Value &_jsonRoot, glmTile &_tile);
    
    float lineWidth;
    
private:
    
    void buildLayer(Json::Value &_jsonRoot, const std::string &_layerName, glmTile &_tile, float _minHeight = 0.);
    
    void lineJson2Polyline(Json::Value &_lineJson, glmPolyline &_poly, float _minHeight);
    
    void lineJson2Mesh(Json::Value &_lineJson, glmMesh &_mesh, float _minHeight);
    void polygonJson2Mesh(Json::Value &_polygonJson, glmMesh &_mesh, float _minHeight, float _height);
    
    glm::vec3 m_geometryOffset;
    
};