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
        
        if (bVisible) {
            float distance = 500;
            
            if(m_label.width + distance > m_anchorLine.getLength()){
                seedAnchorAt(0.5);
            } else {
                seedAnchorsEvery(distance);
            }
            
        }

    } else {
        bVisible = false;
    }
};

void glmFeatureLabelLine::seedAnchorAt(float _pct ){
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
    
    //  Calculate how many Labels with margins can fit?
    int nTimes = totalLength/stepLength;
    float tLenght = stepLength*(float)nTimes;
    
    //  Center the Labels
    float seed = (totalLength-tLenght)*0.5;
    
    //  Add anchor points for seeds every _distance
    m_anchorDistances.clear();
    while (seed < totalLength-stepLength) {
        m_anchorDistances.push_back(seed+_distance*0.5);
        seed += stepLength;
    }
}

void glmFeatureLabelLine::drawLine(){
    
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x1111);
	m_anchorLine.draw();
    glDisable(GL_LINE_STIPPLE);
    
    for (int i = 0; i < m_anchorLine.size(); i++) {
        
        if(i == 0 ){
            glLineWidth(2);
            drawCross(m_anchorLine[i],5);
        } else {
            glLineWidth(1);
            drawCross(m_anchorLine[i]);
        }
        
    }
}

void glmFeatureLabelLine::draw(const glm::vec3 &_camPos ){
    if(m_font!=NULL&&m_text!="NONE"&&bVisible){
        
        float alpha = glm::dot( glm::normalize(_camPos-m_centroid),glm::vec3(0.,0.,1.));
        glColor4f(1., 1., 1., alpha);
        
        for (auto &it : m_anchorDistances){
            drawLetterByLetter(it, _camPos);
        }
    }
}

void glmFeatureLabelLine::drawWordByWord(float _offset, const glm::vec3 &_camPos){
    
}

void glmFeatureLabelLine::drawLetterByLetter(float _offset, const glm::vec3 &_camPos){
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