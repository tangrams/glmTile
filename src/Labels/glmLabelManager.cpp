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

bool depthSort(const glmFeatureLabelPointRef &_A, const glmFeatureLabelPointRef &_B){
    return _A->getDepth() < _B->getDepth();
}

void glmLabelManager::updateProjection(){
    
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
        it->updateProjection();
    }

}

void glmLabelManager::draw(){
    for (auto &it : pointLabels) {
        
        if(m_bFontChanged){
            it->setFont(m_font);
        }
        
        if (it->bVisible) {
            it->draw();
        }
        
    }
    
    for (auto &it : lineLabels) {
        
        if(m_bFontChanged){
            it->setFont(m_font);
        }
        
        if (it->bVisible) {
            it->draw();
        }
        
    }
    
    m_bFontChanged = false;
}