//
//  glmLabelManager.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#include "glmLabelManager.h"
#include <algorithm>

glmLabelManager::glmLabelManager(): m_bFontChanged(true), bLines(true), bPoints(true), bDebugLines(false), bDebugPoints(false) {
}

glmLabelManager::~glmLabelManager(){
    
}

void glmLabelManager::setFont(glmFont *_font){
    m_font = glmFontRef(_font);
    m_bFontChanged = true;
}

void glmLabelManager::setFont(glmFontRef &_font){
    m_font = _font;
    m_bFontChanged = true;
}

glmFontRef& glmLabelManager::getFont(){
    return m_font;
}

void glmLabelManager::addLineLabel( glmFeatureLabelLineRef &_lineLabel ){
    if(m_font != NULL){
        _lineLabel->setFont(m_font);
    }
    _lineLabel->setCameraPos(&m_cameraPos);
    _lineLabel->pointLabels = &pointLabels;
    
    lineLabels.push_back(_lineLabel);
    
    //  TODO: Check duplicates street names
}

void glmLabelManager::mergLineLabels( glmFeatureLabelLineRef &_father, glmFeatureLabelLineRef &_child ){
    
}

void glmLabelManager::addPointLabel( glmFeatureLabelPointRef &_pointLabel ){
    if(m_font != NULL){
        _pointLabel->setFont(m_font);
    }
    _pointLabel->setCameraPos(&m_cameraPos);
    
    bool isPrev = false;
    for (int i = 0; i < pointLabels.size(); i++) {
        
        if(pointLabels[i]->idString == _pointLabel->idString ){
           
            mergePointLabels(pointLabels[i],_pointLabel);
            
            isPrev = true;
            break;
        }
    }
    
    if(!isPrev){
        pointLabels.push_back(_pointLabel);
    }
}

void glmLabelManager::mergePointLabels( glmFeatureLabelPointRef &_father, glmFeatureLabelPointRef &_child){
    
    //  Move shapes from _child to father
    //
    for (int i = _child->shapes.size()-1; i >= 0; i--) {
        _father->shapes.push_back(_child->shapes[i]);
        _child->shapes.erase(_child->shapes.begin()+i);
    }
    
    //  Re center father
    //
    int maxHeight = 0;
    glm::vec3 center;
    for (auto &it: _father->shapes) {
        if (it[0].z > maxHeight) {
            maxHeight = it[0].z;
        }
        center += it.getCentroid();
    }
    center = center / (float)_father->shapes.size();
    center.z = maxHeight;
    _father->setPosition(center);
    
}

bool glmLabelManager::deleteLabel(const std::string &_idString){
    for (int i = lineLabels.size()-1 ; i >= 0 ; i--) {
        if (lineLabels[i]->idString == _idString) {
            lineLabels.erase(lineLabels.begin()+i);
            break;
        }
    }
    
    for (int i = pointLabels.size()-1 ; i >= 0 ; i--) {
        if (pointLabels[i]->idString == _idString) {
            pointLabels.erase(pointLabels.begin()+i);
            break;
        }
    }
}

void glmLabelManager::updateFont(){
    if(m_bFontChanged){
        for (auto &it : pointLabels) {
            it->setFont(m_font);
        }
        
        for (auto &it : lineLabels) {
            it->setFont(m_font);
        }
        
        m_bFontChanged = false;
    }
}

bool depthSort(const glmFeatureLabelPointRef &_A, const glmFeatureLabelPointRef &_B){
    return _A->getAnchorPoint().z < _B->getAnchorPoint().z;
}

void glmLabelManager::updateCameraPosition( const glm::vec3 &_pos ){
    if(m_cameraPos != _pos){
        m_cameraPos = _pos;
        m_bProjectionChanged = true;
    }
}

void glmLabelManager::forceProjectionUpdate(){
    m_bProjectionChanged = true;
}

void glmLabelManager::updateProjection(){
    
    if(m_bProjectionChanged){
        
        if(bPoints){
            for (auto &it : pointLabels) {
                it->updateProjection();
            }
            
            std::sort(pointLabels.begin(),pointLabels.end(), depthSort);
            
            for (int i = 0; i < pointLabels.size(); i++) {
                if(pointLabels[i]->bVisible){
                    for (int j = i-1; j >= 0 ; j--) {
                        if (pointLabels[i]->isOver( pointLabels[j].get() ) ){
                            pointLabels[i]->bVisible = false;
                            break;
                        }
                    }
                }
            }
        } else {
            for (auto &it : pointLabels) {
                it->bVisible = false;
            }
        }
        
        if(bLines){
            for (auto &it : lineLabels) {
                if(m_bFontChanged){
                    it->setFont(m_font);
                }
                it->updateProjection();
            }
        } else {
            for (auto &it : lineLabels) {
                it->bVisible = false;
            }
        }
        
        
        m_bProjectionChanged = false;
    }
}

void glmLabelManager::updateOcclusions(float *_depthBuffer, int _width, int _height){
    for (auto &it : pointLabels) {
        if (it->bVisible) {
            glm::vec3 pos = it->getAnchorPoint();
            
            if(pos.x>0
               &&pos.x<_width
               &&pos.y>0
               &&pos.y<_height){
                
                int index = ((int)pos.y) * _width + (int)pos.x;
                float depth = _depthBuffer[ index*3 ];
                
                if(pos.z == depth){
                    it->bVisible = true;
                } else {
                    it->bVisible = false;
                }
            }
        }
    }
}

void glmLabelManager::draw2D(){
    
    glColor4f(1.,1.,1.,1.);
    
    for (auto &it : pointLabels) {
        it->draw2D();
        
        if(bDebugPoints){
            it->drawDebug();
        }
    }
    
    for (auto &it : lineLabels) {
        it->draw2D();
        
        if(bDebugLines){
            it->drawDebug();
        }
    }
    
    glColor4f(1.,1.,1.,1.);
}

void glmLabelManager::draw3D(){
    if(bPoints){
        glColor4f(1.,1.,1.,1.);
        for (auto &it : pointLabels) {
            it->draw3D();
        }
    }
}