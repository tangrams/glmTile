//
//  glmGeometryBuilder.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#include "glmGeometryBuilder.h"

#include <fstream>
#include <utility>

#include <curl/curl.h>
#include <iostream>

//write_data call back from CURLOPT_WRITEFUNCTION
//responsible to read and fill "stream" with the data.
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::string data((const char*) ptr, (size_t) size*nmemb);
    *((std::stringstream*) stream) << data;
    return size*nmemb;
}

std::string getURL(const std::string& url) {
    CURL * curl = curl_easy_init();
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /* example.com is redirected, so we tell libcurl to follow redirection */
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
    std::stringstream out;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    return out.str();
}

glmGeometryBuilder::glmGeometryBuilder():m_geometryOffset(0.0,0.0,0.0),lineWidth(5.5),labelManager(NULL){
    LayerColorPalette["earth"] = glm::vec4(0.5,0.5,0.5,1.0);
    LayerColorPalette["landuse"] = glm::vec4(0.0,0.7,0.0,1.0);
    LayerColorPalette["water"] = glm::vec4(0.0,0.0,0.9,1.0);
    LayerColorPalette["buildings"] = glm::vec4(0.9,0.9,0.5,1.0);
    LayerColorPalette["places"] = glm::vec4(1.0,0.0,0.0,1.0);
    LayerColorPalette["roads"] = glm::vec4(0.1,0.1,0.1,1.0);
    LayerColorPalette["pois"] = glm::vec4(1.0,0.1,0.1,1.0);
}

glmGeometryBuilder::~glmGeometryBuilder(){
};

void glmGeometryBuilder::setLabelManager(glmLabelManager *_lm){
    labelManager = _lm;
}

void glmGeometryBuilder::setOffset(glm::vec3 _offset){
    m_geometryOffset = _offset;
}

void glmGeometryBuilder::setOffset(int _tileX, int _tileY, int _zoom){
    float n = powf(2.0f, _zoom);
    m_geometryOffset.x = lon2x((_tileX + 0.5) / n * 360.0f - 180.0f);
    m_geometryOffset.y = lat2y(atanf(sinhf(PI*(1-2*(_tileY+0.5)/n))) * 180.0f / PI);
}

void glmGeometryBuilder::load(int _tileX, int _tileY, int _zoom, glmTile &_tile){
    //  TODO: get JSON file from the web
    //
    std::string tmp = getURL("http://vector.mapzen.com/osm/all/"+toString(_zoom)+"/"+toString(_tileX)+"/"+toString(_tileY)+".json");
    
    std::tr1::shared_ptr<Json::Value> jsonVal(new Json::Value);
    
    int length = tmp.size();
    Json::Reader jsonReader;
    jsonReader.parse(tmp.c_str(), tmp.c_str() + length, *(jsonVal.get()));
    
    _tile.tileX = _tileX;
    _tile.tileY = _tileY;
    _tile.zoom = _zoom;
    load( *(jsonVal.get()), _tile );
    
}

void glmGeometryBuilder::load(Json::Value &_jsonRoot, glmTile & _tile){
    
    //  Generate the geometries for these layers...
    //  ... also record those that have labels.
    //
    buildLayer(_jsonRoot, "earth", _tile, 0.0);
    buildLayer(_jsonRoot, "landuse", _tile, 1.0);
    
    buildLayer(_jsonRoot, "water", _tile, 2.0);
    buildLayer(_jsonRoot, "buildings", _tile, 3.0);
    
    buildLayer(_jsonRoot, "places", _tile, 4.0);
    buildLayer(_jsonRoot, "roads", _tile, 5.0);
    buildLayer(_jsonRoot, "pois", _tile, 6.0);
    
    //  TODO:
    //      - until the data from the server get fixed, merge streets
    //      - and merge buildings (both important for have smarter labels)
    //
    for (auto &pointLabel: _tile.labeledPoints) {
        for (auto &otherBuilding : _tile.byLayers["buildings"]) {
            
            if (pointLabel.get() != otherBuilding.get()
                && pointLabel->orginal.size() > 0 ) {
                
                std::cout << "Check " << otherBuilding->idString << ":" << otherBuilding->orginal.size() << " on " << pointLabel->idString << ":" << pointLabel->orginal.size() << std::endl;
                
                glm::vec3 centroid = otherBuilding->orginal.getCentroid();
                if ( pointLabel->orginal.isInside(centroid.x, centroid.y) ) {
                    
                    pointLabel->polylines.push_back(otherBuilding->orginal);
                    std::cout << otherBuilding->idString << " inside " << pointLabel->idString << std::endl;
                }
            }
        }
    }
}

glmTile glmGeometryBuilder::getFromFile(std::string _filename){
    
    std::ifstream inputStream(_filename.c_str(),std::ifstream::in);
    if(inputStream.bad()){
		return glmTile();
	}
    
    Json::Reader jsonReader;
    Json::Value m_jsonRoot;
    
    if (!jsonReader.parse(inputStream, m_jsonRoot)) {
        return glmTile();
    }
    
    glmTile newTile;
    load(m_jsonRoot, newTile);
    return newTile;
}

glmTile glmGeometryBuilder::getFromWeb(int _tileX, int _tileY, int _zoom){
    glmTile newTile;
    load(_tileX, _tileY,_zoom,newTile);
    return newTile;
}

void glmGeometryBuilder::buildLayer(Json::Value &_jsonRoot, const std::string &_layerName, glmTile &_tile, float _minHeight) {
    
    Json::Value featureListJson = _jsonRoot[_layerName.c_str()]["features"];
    
    for (int i = 0; i < featureListJson.size(); i++) {
        
        //  Extract properties
        //
        Json::Value geometryJson = featureListJson[i]["geometry"];
        std::string geometryType = geometryJson["type"].asString();
        Json::Value propsJson = featureListJson[i]["properties"];
        
        float height = _minHeight;
        if (propsJson.isMember("height"))
            height += propsJson["height"].asFloat();
        
        float minHeight = _minHeight;
        if (propsJson.isMember("min_height"))
            minHeight += propsJson["min_height"].asFloat();
        
        
        glmFeatureRef feature(new glmFeature());
    
        // Parse geometry into feature
        //
        if (geometryType.compare("Point") == 0) {
            
            //  PARSE POINT
            //

            if (propsJson.isMember("name") && labelManager != NULL) {
                
                glmFeatureLabelPointRef labelRef(new glmFeatureLabelPoint());
                
                labelRef->setPosition( glm::vec3(lon2x(geometryJson["coordinates"][0].asFloat()),
                                                 lat2y(geometryJson["coordinates"][1].asFloat()),
                                                 _minHeight) - m_geometryOffset );
                
                labelRef->setText( propsJson["name"].asString() );
                
                _tile.labeledFeatures.push_back(labelRef);
                _tile.labeledPoints.push_back(labelRef);
                
                labelRef->setFont(labelManager->getFont());
                labelManager->pointLabels.push_back(labelRef);
                
                feature = labelRef;
            }
            
            pointJson2Mesh(geometryJson["coordinates"], *feature, 3, 6, minHeight);
            
        } else if (geometryType.compare("MultiPoint") == 0) {
            
            //  PARSE MULTI-POINT
            //
            if (propsJson.isMember("name") && labelManager != NULL) {
                glmFeatureLabelPointRef labelRef(new glmFeatureLabelPoint());
                labelRef->setPosition( glm::vec3(lon2x(geometryJson["coordinates"][0].asFloat()),
                                                  lat2y(geometryJson["coordinates"][1].asFloat()),
                                                  _minHeight) - m_geometryOffset );
                
                labelRef->setText( propsJson["name"].asString() );
                
                _tile.labeledFeatures.push_back(labelRef);
                _tile.labeledPoints.push_back(labelRef);
                
                labelRef->setFont(labelManager->getFont());
                labelManager->pointLabels.push_back(labelRef);
                
                feature = labelRef;
            }
            
            for (int j = 0; j < geometryJson["coordinates"].size(); j++) {
                pointJson2Mesh(geometryJson["coordinates"], *feature, 3.0, 6, minHeight);
            }
            
        } else if (geometryType.compare("LineString") == 0) {
            
            //  PARSE LINE STRING
            //
            glmPolyline polyline;
            lineJson2Polyline(geometryJson["coordinates"],polyline, minHeight);
            
            if (propsJson.isMember("name") && labelManager != NULL) {
                glmFeatureLabelLineRef labelRef(new glmFeatureLabelLine());
                
                labelRef->polyline = polyline;
                
                labelRef->setText(propsJson["name"].asString());
                _tile.labeledFeatures.push_back(labelRef);
                _tile.labeledLines.push_back(labelRef);
                
                labelRef->setFont(labelManager->getFont());
                labelManager->lineLabels.push_back(labelRef);
                
                feature = labelRef;
            }
            
            feature->orginal = polyline;
            feature->add(polyline,lineWidth);
            
        } else if (geometryType.compare("MultiLineString") == 0) {
            
            //  PARSE MULTI-LINE STRING
            //
            glmPolyline polyline;
            
            if (propsJson.isMember("name") && labelManager != NULL) {
                
                glmFeatureLabelLineRef labelRef(new glmFeatureLabelLine());
                
                lineJson2Polyline(geometryJson["coordinates"][0],polyline,minHeight);
                
                labelRef->polyline = polyline;
                labelRef->setText(propsJson["name"].asString());
                _tile.labeledFeatures.push_back(labelRef);
                _tile.labeledLines.push_back(labelRef);
                
                labelRef->setFont(labelManager->getFont());
                labelManager->lineLabels.push_back(labelRef);
                
                feature = labelRef;
            }
            
            for (int j = 0; j < geometryJson["coordinates"].size(); j++) {
                polyline.clear();
                lineJson2Polyline(geometryJson["coordinates"][j],polyline,minHeight);
                
            }
            
            feature->orginal = polyline;
            feature->add(polyline,lineWidth);
            
        } else if (geometryType.compare("Polygon") == 0) {
            
            //  PARSE POLYGON
            //
            
            glmPolyline polyline;
            lineJson2Polyline(geometryJson["coordinates"][0],polyline, height);
            
            if (propsJson.isMember("name") && labelManager != NULL) {
                glmFeatureLabelPointRef labelRef(new glmFeatureLabelPoint());

                labelRef->setPosition(polyline.getCentroid());
                labelRef->polylines.push_back(polyline);
                
                labelRef->setText(propsJson["name"].asString());
                
                _tile.labeledFeatures.push_back(labelRef);
                _tile.labeledPoints.push_back(labelRef);
                
                labelRef->setFont(labelManager->getFont());
                labelRef->orginal = polyline;
                
                labelManager->pointLabels.push_back(labelRef);
                feature = labelRef;
            }
            
            feature->orginal = polyline;
            polygonJson2Mesh(geometryJson["coordinates"], *feature, minHeight, height);
            
        } else if (geometryType.compare("MultiPolygon") == 0) {
            
            glmPolyline polyline;
            lineJson2Polyline(geometryJson["coordinates"][0][0], polyline, height);
            
            if (propsJson.isMember("name") && labelManager != NULL) {
                glmFeatureLabelPointRef labelRef(new glmFeatureLabelPoint());
                
                labelRef->setPosition( polyline.getCentroid() );
                labelRef->polylines.push_back(polyline);
                labelRef->setText(propsJson["name"].asString());
                
                _tile.labeledFeatures.push_back(labelRef);
                _tile.labeledPoints.push_back(labelRef);
                
                labelRef->setFont(labelManager->getFont());
                labelManager->pointLabels.push_back(labelRef);
                
                feature = labelRef;
            }
            
            feature->orginal = polyline;
            
            for (int j = 0; j < geometryJson["coordinates"].size(); j++) {
                polygonJson2Mesh(geometryJson["coordinates"][j], *feature, minHeight, height);
            }
            
        } else if (geometryType.compare("GeometryCollection") == 0) {
            
            //  PARSE GEOMETRY-COLLECTION
            //
            
            // TODO
            std::cout << "TODO: GeometryCollection" << std::endl;
        }
        
        //  Assign color based on Layer name
        //
        feature->setColor(LayerColorPalette[_layerName]);
        feature->idString = featureListJson[i]["id"].asString();
        
        //  Save the share pointer into 2 maps organiced by ID and LAYER 
        //
        _tile.byId[feature->idString].push_back(feature);
        _tile.byLayers[_layerName].push_back(feature);
    }
}

//---------------------------------------------------------------------------

void glmGeometryBuilder::lineJson2Polyline(Json::Value &lineJson, glmPolyline &_poly, float _minHeight){
    for (int i = 0; i < lineJson.size(); i++) {
        _poly.add(glm::vec3(lon2x(lineJson[i][0].asFloat()),
                            lat2y(lineJson[i][1].asFloat()),
                            _minHeight) - m_geometryOffset);
    }
}

//---------------------------------------------------------------------------

void glmGeometryBuilder::pointJson2Mesh(Json::Value &_lineJson, glmMesh &_mesh, float _radius, int _sides, float _minHeight){
    
    glm::vec3 p(lon2x(_lineJson[0].asFloat()),
                lat2y(_lineJson[1].asFloat()),
                _minHeight);
    p -= m_geometryOffset;
    
    //  make a circle
    //  TODO: add this to polyline;
    //
    float angle = 0.0f;
    float jump = TWO_PI/(float)_sides;
    glmPolyline polyline;
    for(int i = 0; i < _sides; i++){
        polyline.add(glm::vec3(p.x+_radius*cos(angle),
                               p.y+_radius*sin(angle),
                               _minHeight));
        angle += jump;
    }
    _mesh.add(polyline);
}

//---------------------------------------------------------------------------
void glmGeometryBuilder::polygonJson2Mesh(Json::Value &polygonJson, glmMesh &_mesh, float _minHeight, float _height) {
    uint16_t indexOffset = (uint16_t)_mesh.getVertices().size();    // track indices
    
    //  Go through the Json polygons making walls and adding it to the tessalator
    //
    std::vector<glmPolyline> rings;
    
    for (int i = 0; i < polygonJson.size(); i++) {
        
        glmPolyline ringCoords;
        lineJson2Polyline(polygonJson[i],ringCoords,_height);
        
        // Extrude polygon based on height
        if (_height != _minHeight){
            
            glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
            glm::vec3 tan, nor;
            
            for (int i = 0; i < ringCoords.size() - 1; i++) {
                
                //For each vertex in the polygon, make two triangles to form a quad
                //
                glm::vec3 ip0 = ringCoords[i];
                glm::vec3 ip1 = ringCoords[i+1];
                
                tan = ringCoords[i+1] - ringCoords[i];
                nor = glm::cross(up, tan);
                
                _mesh.addTexCoord(glm::vec2(1.,0.));
                _mesh.addVertex(ip0);
                _mesh.addNormal(nor);
                _mesh.addTexCoord(glm::vec2(0.,0.));
                _mesh.addVertex(ip1);
                _mesh.addNormal(nor);
                
                _mesh.addTexCoord(glm::vec2(1.,1.));
                _mesh.addVertex(ip0+glm::vec3(0.,0.,_minHeight - _height - m_geometryOffset.z));
                _mesh.addNormal(nor);
                _mesh.addTexCoord(glm::vec2(0.,1.));
                _mesh.addVertex(ip1+glm::vec3(0.,0.,_minHeight - _height - m_geometryOffset.z));
                _mesh.addNormal(nor);
                
                _mesh.addIndex(indexOffset);
                _mesh.addIndex(indexOffset + 2);
                _mesh.addIndex(indexOffset + 1);
                
                _mesh.addIndex(indexOffset + 1);
                _mesh.addIndex(indexOffset + 2);
                _mesh.addIndex(indexOffset + 3);
                indexOffset += 4;
            }
        }
        
        rings.push_back(ringCoords);
    }
    
    _mesh.add(rings);
}
