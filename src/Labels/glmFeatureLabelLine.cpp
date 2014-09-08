//
//  glmFeatureLabelLine.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#include "glmFeatureLabelLine.h"

glmFeatureLabelLine::glmFeatureLabelLine(): m_offset(0.5,0.5){
    
}

glmFeatureLabelLine::~glmFeatureLabelLine(){
    
}

glm::vec3 glmFeatureLabelLine::getScreenPosition() const{
    return m_anchorPoint;
}

void glmFeatureLabelLine::updateProjection(){
    
    if(m_font!=NULL&&m_text!="NONE"){
        glm::ivec4 viewport;
        glm::mat4x4 mvmatrix, projmatrix;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
        glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
        
        if(m_bChanged){
            updateCached();
        }
        
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
                    && m_anchorLine.getLength() < 0.0
                    && m_label.width < m_anchorLine.getLength();
        
        
    } else {
        bVisible = false;
    }
};

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

void glmFeatureLabelLine::draw(){
    
    if(m_font!=NULL&&m_text!="NONE"&&bVisible){
        
        float lineLength = m_anchorLine.getLength();

        //  Set ancher point
        //
        float offsetPct = m_offset.x;
        while (lineLength*offsetPct - m_label.width*offsetPct + m_label.width > lineLength) {
            offsetPct -= 0.01;
        }
        
        float offset = lineLength*offsetPct-m_label.width*m_offset.x;
        if(offset<0.0|| offset > lineLength){
            return;
        }
        
        m_anchorPoint = m_anchorLine.getPositionAt(offset+m_label.width*m_offset.x);
        
        drawLetterByLetter(offset);
    }
}

void glmFeatureLabelLine::drawWordByWord(float _offset){
    
}

void glmFeatureLabelLine::drawLetterByLetter(float _offset){
    //  Orientation
    //
    float angle = PI;
    glm::vec3 diff = m_anchorLine[0]-m_anchorLine[m_anchorLine.size()-1];
    angle = atan2f(-diff.y, diff.x);
    
    if(angle < PI*0.5 && angle > -PI*0.5){
        for (int i = m_text.length()-1; i >=0 ; i--) {
            
            glm::vec3 src = m_anchorLine.getPositionAt(_offset);
            double rot = m_anchorLine.getAngleAt(_offset);
            
            glPushMatrix();
            glTranslated(src.x, src.y, src.z);
            
            glScalef(1,-1,1);
            glRotated(rot*RAD_TO_DEG, 0, 0, -1);
            
            glScaled(-1, -1, 1);
            glTranslated(-m_lettersWidth[i], 0, 0);
            
            glTranslatef(0., -m_label.height*m_offset.t,0.);
            m_font->drawString( std::string(1,m_text[i]));
            glPopMatrix();
            _offset += m_lettersWidth[i];
        }
    } else {
        for (int i = 0; i < m_text.length(); i++) {
            
            glm::vec3 src = m_anchorLine.getPositionAt(_offset);
            double rot = m_anchorLine.getAngleAt(_offset);
            
            glPushMatrix();
            glTranslated(src.x, src.y, src.z);
            
            glScalef(1,-1,1);
            glRotated(rot*RAD_TO_DEG, 0, 0, -1);
            
            glTranslatef(0., -m_label.height*m_offset.t,0.);
            m_font->drawString( std::string(1,m_text[i]));
            
            glPopMatrix();
            _offset += m_lettersWidth[i];
        }
    }

}