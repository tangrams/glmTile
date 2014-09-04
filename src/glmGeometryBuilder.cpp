//
//  glmGeometryBuilder.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#include "glmGeometryBuilder.h"

#include <fstream>
#include <utility>

#include "tesselator.h"

#include <curl/curl.h>
#include <iostream>
#include <sstream>

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

glmGeometryBuilder::glmGeometryBuilder():
m_geometryOffset(0.0,0.0,0.0),
lineWidth(5.5)
{
    
}

glmTile glmGeometryBuilder::download(int _tileX, int _tileY, int _zoom){
    
    //  TODO: get JSON file from the web
    //
    std::ostringstream strStream;
    strStream<<"http://vector.mapzen.com/osm/all/"<<_zoom<<"/"<<_tileX<<"/"<<_tileY<<".json";
    
    std::string tmp = getURL(strStream.str());
    
    std::tr1::shared_ptr<Json::Value> jsonVal(new Json::Value);
    
    int length = tmp.size();
    Json::Reader jsonReader;
    jsonReader.parse(tmp.c_str(), tmp.c_str() + length, *(jsonVal.get()));
    
    glmTile newTile;
    newTile.tileX = _tileX;
    newTile.tileY = _tileY;
    newTile.zoom = _zoom;
    load( *(jsonVal.get()), newTile );
    return newTile;
}

glmTile glmGeometryBuilder::open(std::string _filename){

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

void glmGeometryBuilder::load(Json::Value &_jsonRoot, glmTile & _tile){
    buildLayer(_jsonRoot, "earth", _tile, 0.0);
    buildLayer(_jsonRoot, "landuse", _tile, 1.0);
    
    buildLayer(_jsonRoot, "water", _tile, 2.0);
    buildLayer(_jsonRoot, "buildings", _tile, 3.0);
    
    buildLayer(_jsonRoot, "places", _tile, 4.0);
    buildLayer(_jsonRoot, "roads", _tile, 5.0);
    buildLayer(_jsonRoot, "pois", _tile, 6.0);
}

void glmGeometryBuilder::setCenter(glm::vec3 _offset){
    m_geometryOffset = _offset;
}

void glmGeometryBuilder::setCenter(int _tileX, int _tileY, int _zoom){
    float n = powf(2.0f, _zoom);
    m_geometryOffset.x = lon2x((_tileX + 0.5) / n * 360.0f - 180.0f);
    m_geometryOffset.y = lat2y(atanf(sinhf(PI*(1-2*(_tileY+0.5)/n))) * 180.0f / PI);
}

void glmGeometryBuilder::buildLayer(Json::Value &_jsonRoot, const std::string &_layerName, glmTile &_tile, float _minHeight) {
    
    Json::Value featureListJson = _jsonRoot[_layerName.c_str()]["features"];
    
    std::vector<glm::vec3> verts;
    std::vector<uint16_t> inds;
    
    for (int i = 0; i < featureListJson.size(); i++) {
        
        Json::Value geometryJson = featureListJson[i]["geometry"];
        Json::Value propsJson = featureListJson[i]["properties"];
        
        std::string geometryType = geometryJson["type"].asString();
        
        float height = _minHeight;
        if (propsJson.isMember("height")) {
            height += propsJson["height"].asFloat();
        }
        
        float minHeight = _minHeight;
        if (propsJson.isMember("min_height")) {
            minHeight += propsJson["min_height"].asFloat();
        }
        
        if (geometryType.compare("Point") == 0) {
            
            // TODO
            std::cout << "TODO: Point" << std::endl;
            
        } else if (geometryType.compare("MultiPoint") == 0) {
            
            // TODO
            std::cout << "TODO: MultiPoint" << std::endl;
            
        } else if (geometryType.compare("LineString") == 0) {
            
            if (propsJson.isMember("name")) {
                
                glmLabeledFeatureRef labelRef(new glmLabeledFeature);
                
                lineJson2Polyline(geometryJson["coordinates"],labelRef->polyline,minHeight);
                
                labelRef->centroid = labelRef->polyline.getBoundingBox().getCenter();
                labelRef->text.setText(propsJson["name"].asString());
                labelRef->type = LABEL_LINE;
                
                _tile.labelFeatures.push_back(labelRef);
                
                labelRef->polyline.addAsLineToMesh(labelRef->geometry,lineWidth);
                _tile.layers[_layerName].push_back(labelRef);
                
            } else {
                glmFeatureRef feature(new glmLabeledFeature);
                lineJson2Mesh(geometryJson["coordinates"], feature->geometry, minHeight);
                _tile.layers[_layerName].push_back(feature);
            }
            
        } else if (geometryType.compare("MultiLineString") == 0) {
            
            for (int j = 0; j < geometryJson["coordinates"].size(); j++) {
                glmFeatureRef feature(new glmFeature);
                lineJson2Mesh(geometryJson["coordinates"][j], feature->geometry, minHeight);
                _tile.layers[_layerName].push_back(feature);
            }
            
        } else if (geometryType.compare("Polygon") == 0) {
            
            if (propsJson.isMember("name")) {
                glmLabeledFeatureRef labelRef(new glmLabeledFeature);
                
                lineJson2Polyline(geometryJson["coordinates"][0],labelRef->polyline, height);
                
                labelRef->centroid = labelRef->polyline.getBoundingBox().getCenter() + glm::vec3(0,0,height);
                labelRef->text.setText(propsJson["name"].asString());
                labelRef->type = LABEL_AREA;
                
                _tile.labelFeatures.push_back(labelRef);
                
                polygonJson2Mesh(geometryJson["coordinates"], labelRef->geometry, minHeight, height);
                _tile.layers[_layerName].push_back(labelRef);
            } else {
                glmFeatureRef feature(new glmFeature);
                polygonJson2Mesh(geometryJson["coordinates"], feature->geometry, minHeight, height);
                _tile.layers[_layerName].push_back(feature);
            }
            
        } else if (geometryType.compare("MultiPolygon") == 0) {
            
            for (int j = 0; j < geometryJson["coordinates"].size(); j++) {
                glmFeatureRef feature(new glmFeature);
                polygonJson2Mesh(geometryJson["coordinates"][j], feature->geometry, minHeight, height);
                _tile.layers[_layerName].push_back(feature);
            }
            
        } else if (geometryType.compare("GeometryCollection") == 0) {
            
            // TODO
            std::cout << "TODO: GeometryCollection" << std::endl;
        }
        
    }
}

void glmGeometryBuilder::polygonJson2Mesh(Json::Value &polygonJson, glmMesh &_mesh, float _minHeight, float _height) {
    TESStesselator  *m_tess = tessNewTess(NULL);                    // Tesselator instance
    uint16_t indexOffset = (uint16_t)_mesh.getVertices().size();    // track indices
    _mesh.setDrawMode(GL_TRIANGLES);
    
    glmRectangle bBox;  // will calculate the total bounding box to compute a UV for top the top view
    
    //  Go through the Json polygons making walls and adding it to the tessalator
    //
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
        
        //  Grow  bounding box
        ringCoords.addToBoundingBox(bBox);
        
        //  Add to tesselator
        tessAddContour(m_tess, 3, &ringCoords[0].x, sizeof(glm::vec3), ringCoords.size());
    }
    
    // Tessellate polygon into triangles
    tessTesselate(m_tess, TESS_WINDING_NONZERO, TESS_POLYGONS, 3, 3, NULL);
    
    // Extract triangle elements from tessellator
    const int numIndices = tessGetElementCount(m_tess);
    const TESSindex* indices = tessGetElements(m_tess);
    for (int i = 0; i < numIndices; i++) {
        const TESSindex* poly = &indices[i*3];
        for (int j = 0; j < 3; j++) {
            _mesh.addIndex(poly[j] + indexOffset);
        }
    }
    
    //  Add vertexes from tessellator
    //
    const int numVertices = tessGetVertexCount(m_tess);
    const float* vertices = tessGetVertices(m_tess);
    for (int i = 0; i < numVertices; i++) {
        _mesh.addTexCoord(glm::vec2(mapValue(vertices[3*i],bBox.getMinX(),bBox.getMaxX(),0.,1.),
                                    mapValue(vertices[3*i+1],bBox.getMinY(),bBox.getMaxY(),0.,1.)));
        _mesh.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));
        _mesh.addVertex(glm::vec3(vertices[3*i], vertices[3*i + 1], vertices[3*i + 2]));
    }
    
    tessDeleteTess(m_tess);
}

void glmGeometryBuilder::lineJson2Mesh(Json::Value &lineJson, glmMesh &_mesh, float _minHeight){
    glmPolyline polyline;
    lineJson2Polyline(lineJson,polyline,_minHeight);
    polyline.addAsLineToMesh(_mesh, lineWidth);
}

void glmGeometryBuilder::lineJson2Polyline(Json::Value &lineJson, glmPolyline &_poly, float _minHeight){
    for (int i = 0; i < lineJson.size(); i++) {
        _poly.add(glm::vec3(lon2x(lineJson[i][0].asFloat()),
                            lat2y(lineJson[i][1].asFloat()),
                            _minHeight) - m_geometryOffset);
    }
}
