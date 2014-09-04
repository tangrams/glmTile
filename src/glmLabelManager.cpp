//
//  glmLabelManager.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/4/14.
//
//

#include "glmLabelManager.h"

glmLabelManager::glmLabelManager():
font(NULL),
bFontChanged(true)
{
}

glmLabelManager::~glmLabelManager(){
}

void glmLabelManager::addLabelsFrom(glmTile &_tile) {
    for (auto &it: _tile.labelFeatures) {
        labels.push_back(it);
    }
}

void glmLabelManager::setFont(FTFont *_font){
    font = _font;
    bFontChanged = true;
}

void glmLabelManager::updateProjection(){
    for (auto &it : labels) {
        it->updateProjection();
    }
}

void glmLabelManager::draw(){
    for (auto &it : labels) {
        
        if(bFontChanged){
            it->text.setFont(font);
        }
        
        it->draw();        
//        if (bDebug){
//            ofSetColor(255, 200);
//            it->projectedPolyline.draw();
//        }
//        
//        if(it->type == LABEL_AREA){
//            ofSetColor(255, 0,0,200);
//            drawCross(it->projectedCentroid);
//        }
    }
    
    bFontChanged = false;
}