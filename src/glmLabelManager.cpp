//
//  glmLabelManager.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#include "glmLabelManager.h"
#include <algorithm>

glmLabelManager::glmLabelManager(): font(NULL), bFontChanged(true) {
}

glmLabelManager::~glmLabelManager(){
    
}

void glmLabelManager::setFont(FTFont *_font){
    font = _font;
    bFontChanged = true;
}

bool depthSort(const glmLabeledFeaturePointRef &_A, const glmLabeledFeaturePointRef &_B){
    return _A->labelAnchoir.z < _B->labelAnchoir.z;
}

void glmLabelManager::updateProjection(){
    
    for (auto &it : pointLabels) {
        it->updateProjection();
    }
    
    std::sort(pointLabels.begin(),pointLabels.end(), depthSort);
    
    for (int i = 0; i < pointLabels.size(); i++) {
        if(pointLabels[i]->bVisible){
            for (int j = i-1; j >= 0 ; j--) {
                if (pointLabels[i]->labelAnchoir.isOver(pointLabels[j]->labelAnchoir) ){
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
        
        if(bFontChanged){
            it->text.setFont(font);
            it->labelAnchoir.setLabelBoundingBox(it->text.getBoundingBox());
        }
        
        if (it->bVisible) {
            it->draw();
        }
        
    }
    
    for (auto &it : lineLabels) {
        
        if(bFontChanged){
            it->text.setFont(font);
        }
        
        if (it->bVisible) {
            it->draw();
        }
        
    }
    
    bFontChanged = false;
}