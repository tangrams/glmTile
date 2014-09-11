//
//  glmLabelManager.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#include "glmLabelManager.h"
#include <algorithm>

glmLabelManager::glmLabelManager(): m_bFontChanged(true) {
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
    m_cameraPos = _pos;
    m_bProjectionChanged = true;
}

void glmLabelManager::updateProjection(){
    
    if(m_bProjectionChanged){
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
        
        for (auto &it : lineLabels) {
            if(m_bFontChanged){
                it->setFont(m_font);
            }
            it->updateProjection();
            
            if (it->bVisible) {
                it->seedAnchorAt(0.5);
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

void glmLabelManager::draw(){
    
//    glColor4f(1.,1.,1.,1.);
//    for (auto &it : pointLabels) {
//        if (it->bVisible) {
//            it->draw();
//        }
//        
//    }
    
    float alpha = glm::dot( glm::normalize(m_cameraPos) ,glm::vec3(0.,0.,1.));
    glColor4f(1., 1., 1., alpha);
    
    for (auto &it : lineLabels) {
        if (it->bVisible) {
            it->draw();
        }
    }
    
    glColor4f(1.,1.,1.,1.);
}