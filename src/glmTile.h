//
//  glmTile.h
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 8/27/14.
//
//

#pragma once

#include "json/json.h"

#include "glmTools.h"
#include "glmTileFeature.h"
#include "glmPolyline.h"

#include "ftgl.h"

class glmTile {
public:
    
    glmTile();
    glmTile(std::string fileName);
    
    void setFont(FTFont *_font);
    void setGeometryOffset(glm::vec3 _offset);  // This should be DEPECRATED, is just to center the tile
    
    bool load(std::string _fileName);
    bool load(int _tileX, int _tileY, int _zoom);
    bool load(Json::Value &_jsonRoot);
    
    void draw();
    
    std::map< std::string, std::vector<glmTileFeatureRef> > layers;
    std::vector<glmLabeledFeatureRef> labelFeatures;
    
private:

    //  This will disapear and become a different geometry constructor class
    //
    void buildLayer(Json::Value &_jsonRoot, std::string layerName, float _layerHeight = 0.);
    void buildGeometry(Json::Value &_jsonRoot ,std::string _layerName, std::vector<glmTileFeatureRef> &_mesh, float _minHeight = 0.);
    void lineJson2Mesh(Json::Value &_lineJson, glmMesh &_mesh, float _minHeight);
    void lineJson2Polyline(Json::Value &_lineJson, glmPolyline &_poly, float _minHeight);
    void polygonJson2Mesh(Json::Value &_polygonJson, glmMesh &_mesh, float _minHeight, float _height);
    
    glm::vec3   m_geometryOffset;
    
    float   lineWidth;
    int     tileX, tileY, zoom;
    
    FTFont  *font;
};