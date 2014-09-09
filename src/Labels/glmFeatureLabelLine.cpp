//
//  glmFeatureLabelLine.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#include "glmFeatureLabelLine.h"

glmFeatureLabelLine::glmFeatureLabelLine(){
    
}

glmFeatureLabelLine::~glmFeatureLabelLine(){
    
}

void glmFeatureLabelLine::updateCached(){
    if(m_font!=NULL&&m_text!="NONE"){
        
        m_lettersWidth.clear();
        m_wordsWidth.clear();
        
        std::string word = "";
        float wordWidth = 0.0f;
        
        for(int i = 0; i < m_text.size(); i++){
            float letterWidth = m_font->stringWidth( std::string(1,m_text[i]) );
            
            if( m_text[i] == ' '){
                m_lettersWidth.push_back(letterWidth);
                m_wordsWidth.push_back(wordWidth);
                wordWidth = 0.;
                word = "";
            } else {
                m_lettersWidth.push_back(letterWidth);
                wordWidth += letterWidth;
                word += &m_text[i];
            }
        }
        
        m_label = m_font->getStringBoundingBox(m_text);
        m_bChanged = false;
    } else {
        bVisible = false;
    }
}

void glmFeatureLabelLine::updateProjection(){
    
    if(m_font!=NULL&&m_text!="NONE"){
        
        if(m_bChanged){
            updateCached();
        }
        
        //  Project polyline
        //
        glm::ivec4 viewport;
        glm::mat4x4 mvmatrix, projmatrix;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
        glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
        
        std::cout << viewport.x << "," << viewport.y << "," << viewport.z << "," << viewport.w << std::endl;
        
        m_anchorLine.clear();
        for (int i = 0; i < polyline.size(); i++) {
            glm::vec3 v = glm::project(polyline[i], mvmatrix, projmatrix, viewport);
            if( v.z >= 0.0 && v.z <= 1.0){
                m_anchorLine.add(v);
            }
        }
        
        //  Does the text actually fits on line???
        //
        bVisible =  m_anchorLine.size() > 1.0
                    && m_anchorLine.getLength() > 0.0
                    && m_label.width < m_anchorLine.getLength();
        
        if(bVisible){
            seedAnchorOn(0.5);
//            seedAnchorsEvery(200);
        }
        
    } else {
        bVisible = false;
    }
};

void glmFeatureLabelLine::seedAnchorOn(float _pct ){
    float totalLength = m_anchorLine.getLength();
    
    float offsetPct = _pct;
    while (totalLength*offsetPct - m_label.width*offsetPct + m_label.width > totalLength) {
        offsetPct -= 0.01;
    }
    
    float offset = totalLength*offsetPct-m_label.width*_pct;
    if(offset<0.0|| offset > totalLength){
        return;
    }
    
    m_anchorDistances.clear();
    m_anchorDistances.push_back(offset);
}

void glmFeatureLabelLine::seedAnchorsEvery(float _distance){
    float totalLength = m_anchorLine.getLength();
    float stepLength = m_label.width+_distance;
    
    m_anchorDistances.clear();
    
    float seed = stepLength*0.5;
    while (seed < totalLength-stepLength) {
        m_anchorDistances.push_back(seed);
        seed += stepLength;
    }
}

void glmFeatureLabelLine::draw(){
    if(m_font!=NULL&&m_text!="NONE"&&bVisible){
        
        for (auto &it : m_anchorDistances){
            drawLetterByLetter(it);
        }
        
    }
}

void glmFeatureLabelLine::drawWordByWord(float _offset){
    
}

void glmFeatureLabelLine::drawLetterByLetter(float _offset){
    glm::ivec4 viewport;
    glGetIntegerv(GL_VIEWPORT, &viewport[0]);
    glmRectangle screen = glmRectangle(viewport);
    
    //  Orientation
    //
    float angle = PI;
    glm::vec3 diff = m_anchorLine[0]-m_anchorLine[m_anchorLine.size()-1];
    angle = atan2f(-diff.y, diff.x);
    
    if(angle < PI*0.5 && angle > -PI*0.5){
        for (int i = m_text.length()-1; i >=0 ; i--) {
            
            glm::vec3 src = m_anchorLine.getPositionAt(_offset);
            
            if(screen.inside(src)){
                double rot = m_anchorLine.getAngleAt(_offset);
                
                glPushMatrix();
                glTranslated(src.x, src.y, src.z);
                
                glScalef(1,-1,1);
                glRotated(rot*RAD_TO_DEG, 0, 0, -1);
                
                glScaled(-1, -1, 1);
                glTranslated(-m_lettersWidth[i], 0, 0);
                
                glTranslatef(0., -m_label.height*0.5,0.);
                m_font->drawString( std::string(1,m_text[i]));
                glPopMatrix();
                _offset += m_lettersWidth[i];
            } else {
                break;
            }
            
        }
    } else {
        for (int i = 0; i < m_text.length(); i++) {
            
            glm::vec3 src = m_anchorLine.getPositionAt(_offset);
            
            if(screen.inside(src)){
                double rot = m_anchorLine.getAngleAt(_offset);
                
                glPushMatrix();
                glTranslated(src.x, src.y, src.z);
                
                glScalef(1,-1,1);
                glRotated(rot*RAD_TO_DEG, 0, 0, -1);
                
                glTranslatef(0., -m_label.height*0.5,0.);
                m_font->drawString( std::string(1,m_text[i]));
                
                glPopMatrix();
                _offset += m_lettersWidth[i];
            } else {
                break;
            }
        }
    }
}