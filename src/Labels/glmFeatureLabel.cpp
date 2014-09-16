//
//  glmFeatureLabel.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#include "glmFeatureLabel.h"

glmFeatureLabel::glmFeatureLabel():bVisible(false), m_text("NONE"), m_bChanged(true){
}

glmFeatureLabel::glmFeatureLabel(const std::string &_text):bVisible(false), m_text(_text), m_bChanged(true){
    
}

glmFeatureLabel::~glmFeatureLabel(){
    
};

void glmFeatureLabel::setFont(glmFontRef &_fontRef){
    m_font = _fontRef;
    m_bChanged = true;
}

void glmFeatureLabel::setText(const std::string &_text){
    m_text = _text;
    m_bChanged = true;
}

