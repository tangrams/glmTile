//
//  glmFeatureLabelLine.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#include "glmFeatureLabelLine.h"

glmFeatureLabelLine::glmFeatureLabelLine():labelsDistance(500){
}

glmFeatureLabelLine::glmFeatureLabelLine(const std::string &_text):labelsDistance(500){
    setText(_text);
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
        
        //  Update Fonts/Text cached values if something change
        //
        if(m_bChanged){
            updateCached();
        }
        
        //  Clear Previus computed values
        //
        m_anchorLines.clear();
        
        //  Get Matrixes
        //
        glm::ivec4 viewport;
        glm::mat4x4 mvmatrix, projmatrix;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
        glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
        
        //  Project the road into 2D screen position
        //
        for (auto &iShape: shapes){
            glmSmartLine smartline;
            for (int i = 0; i < iShape.size(); i++) {
                glm::vec3 v = glm::project(iShape[i], mvmatrix, projmatrix, viewport);
                if( v.z >= 0.0 && v.z <= 1.0){
                    smartline.add(v);
                }
            }
            
            if(smartline.size()>1
               && smartline.getLength() > 0.0
               && m_label.width < smartline.getLength()){
                m_anchorLines.push_back(smartline);
            }
        }
        
        //  There is something to show??
        //
        bVisible = m_anchorLines.size() > 0.0;
        
        if (bVisible) {
            
            for (auto &it: m_anchorLines) {
                
                //  Place the anchor points for the text labels
                //
                seedAnchorOnSegmentsAt(it,labelsDistance*0.25,labelsDistance); // Multiple labels will apear every XXXXX screen pixels
                
                if (it.marks.size() == 0) {
                    seedAnchorsEvery(it,labelsDistance);
                    it.bLetterByLetter  = true;
                }
                
                if (it.marks.size() == 0) {
                    seedAnchorAt(it, 0.5);
                    it.bLetterByLetter = true;
                }
            }
        }
        
    } else {
        bVisible = false;
    }
};

void glmFeatureLabelLine::seedAnchorAt(glmSmartLine &_anchorLine, float _pct ){
    float totalLength = _anchorLine.getLength();
    
    float offsetPct = _pct;
    while (totalLength*offsetPct - m_label.width*offsetPct + m_label.width > totalLength) {
        offsetPct -= 0.01;
    }
    
    float offset = totalLength*offsetPct-m_label.width*_pct;
    if(offset<0.0|| offset > totalLength){
        return;
    }
    
    //  Add single position to mark
    _anchorLine.marks.push_back(offset);
}

void glmFeatureLabelLine::seedAnchorsEvery(glmSmartLine &_anchorLine, float _distance){
    float totalLength = _anchorLine.getLength();
    float stepLength = m_label.width+_distance;
    
    //  Calculate how many Labels with margins can fit?
    int nTimes = totalLength/stepLength;
    float tLenght = stepLength*(float)nTimes;
    
    //  Center the Labels
    float seed = (totalLength-tLenght)*0.5;
    
    //  Add anchor points for seeds every _distance
    while (seed < totalLength-stepLength) {
        _anchorLine.marks.push_back(seed+_distance*0.5);
        seed += stepLength;
    }
}

void glmFeatureLabelLine::seedAnchorOnSegmentsAt(glmSmartLine &_anchorLine, float _minDistance, float _maxDistance){
    
    float lastSeed = 0.0;
    for (int i = 0; i < _anchorLine.size()-1; i++) {
        float offset = _anchorLine.getDistances()[i];
        float segmentLength = _anchorLine.getPolars()[i].r;
        
        //  Fits?
        //
        if( segmentLength >  m_label.width+_minDistance ){

            //  How many times?
            int nTimes = segmentLength/(m_label.width+_maxDistance);
            
            //  At what distance between each other?
            float margin = (segmentLength-m_label.width*(float)nTimes)/((float)nTimes+1.0);
            
            //  Add anchors points for seeds
            float seed = margin;
            for (int i = 0; i < nTimes; i++) {
                float potentialSeed = offset + seed ;
                
                if( potentialSeed-lastSeed > _minDistance ){
                    lastSeed = potentialSeed;
                    _anchorLine.marks.push_back(lastSeed);
                }
                
                seed += m_label.width+margin;
            }
            
        } else if ( segmentLength > m_label.width){
            
            //  Only one time
            //
            float margin = (segmentLength-m_label.width)*0.5;
            float potentialSeed = offset + margin ;
            if( potentialSeed-lastSeed > _minDistance){
                lastSeed = potentialSeed;
                _anchorLine.marks.push_back(lastSeed);
            }
        }
    }
}

void glmFeatureLabelLine::drawDebug(){
    
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x1111);
    for (auto &it: m_anchorLines) {
        it.draw();
    }
    glDisable(GL_LINE_STIPPLE);
    
    for (auto &it: m_anchorLines) {
        for (int i = 0; i < it.size(); i++) {
            if(i == 0 ){
                glLineWidth(2);
                drawCross(it[i],5);
            } else {
                glLineWidth(1);
                drawCross(it[i]);
            }
        }
    }
}

void glmFeatureLabelLine::draw2D(){
    if(m_font!=NULL&&m_text!="NONE"&&bVisible){
        for (auto &it: m_anchorLines) {
            
            float alpha = 1.0;
            
            if(m_cameraPos!=0){
                alpha = glm::dot( glm::normalize( *m_cameraPos - it.originalCentroid),glm::vec3(0.,0.,1.));
            }
            
            glColor4f(1., 1., 1., alpha);
            
            if(it.bLetterByLetter){
                drawLetterByLetter(it);
            } else {
                drawAllTextAtOnce(it);
            }
        }
        
    }
}

void glmFeatureLabelLine::drawAllTextAtOnce(const glmSmartLine &_anchorLine){
    for (auto _offset : _anchorLine.marks){
        glm::ivec4 viewport;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glmRectangle screen = glmRectangle(viewport);
        
        //  Orientation
        //
        float angle = PI;
        glm::vec3 diff = _anchorLine[0]-_anchorLine[_anchorLine.size()-1];
        angle = atan2f(-diff.y, diff.x);
        
        if(angle < PI*0.5 && angle > -PI*0.5){
            glm::vec3 src = _anchorLine.getPositionAt(_offset);
            
            if(screen.inside(src)){
                double rot = _anchorLine.getAngleAt(_offset);
                
                glPushMatrix();
                glTranslated(src.x, src.y, src.z);
                
                glScalef(1,-1,1);
                glRotated(rot*RAD_TO_DEG, 0, 0, -1);
                
                glScaled(-1, -1, 1);
                glTranslated(-m_label.width, 0, 0);
                
                glTranslatef(0., -m_label.height*0.5,0.);
                m_font->drawString( m_text );
                glPopMatrix();
                
            } else {
                break;
            }
        } else {
            glm::vec3 src = _anchorLine.getPositionAt(_offset);
            
            if(screen.inside(src)){
                double rot = _anchorLine.getAngleAt(_offset);
                
                glPushMatrix();
                glTranslated(src.x, src.y, src.z);
                
                glScalef(1,-1,1);
                glRotated(rot*RAD_TO_DEG, 0, 0, -1);
                
                glTranslatef(0., -m_label.height*0.5,0.);
                m_font->drawString( m_text );
                
                glPopMatrix();
            } else {
                break;
            }
        }
    }
}

void glmFeatureLabelLine::drawWordByWord(const glmSmartLine &_anchorLine){
    
}

void glmFeatureLabelLine::drawLetterByLetter(const glmSmartLine &_anchorLine){
    for (auto _offset : _anchorLine.marks){
        glm::ivec4 viewport;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glmRectangle screen = glmRectangle(viewport);
        
        //  Orientation
        //
        float angle = PI;
        glm::vec3 diff = _anchorLine[0]-_anchorLine[_anchorLine.size()-1];
        angle = atan2f(-diff.y, diff.x);
        
        if(angle < PI*0.5 && angle > -PI*0.5){
            for (int i = m_text.length()-1; i >=0 ; i--) {
                
                glm::vec3 src = _anchorLine.getPositionAt(_offset);
                
                if(screen.inside(src)){
                    double rot = _anchorLine.getAngleAt(_offset);
                    
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
                
                glm::vec3 src = _anchorLine.getPositionAt(_offset);
                
                if(screen.inside(src)){
                    double rot = _anchorLine.getAngleAt(_offset);
                    
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
}

void glmFeatureLabelLine::draw3D(){
    
}