//
//  glmFeatureLabelLine.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#include "glmFeatureLabelLine.h"

glmFeatureLabelLine::glmFeatureLabelLine():maxDistance(300){
}

glmFeatureLabelLine::glmFeatureLabelLine(const std::string &_text):maxDistance(500){
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
                smartline.originalCentroid = iShape.getCentroid();
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
                seedAnchorOnSegmentsAt(it,minDistance,maxDistance);
                
                if (it.marks.size() == 0) {
                    seedAnchorsEvery(it,minDistance,maxDistance);
                    it.bLetterByLetter  = true;
                }
                
//                if (it.marks.size() == 0) {
//                    seedAnchorAt(it, 0.5);
//                    it.bLetterByLetter = true;
//                }
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

void glmFeatureLabelLine::seedAnchorsEvery(glmSmartLine &_anchorLine, float _minDistance, float _maxDistance){
    float segmentLength = _anchorLine.getLength();
    
    //  How many times?
    int nTimes = segmentLength/(m_label.width+_maxDistance);
    
    //  At what distance between each other?
    float margin = (segmentLength-m_label.width*(float)nTimes)/((float)nTimes+1.0);
    
    //  Center the Labels
    float seed = margin;
    
    //  Add anchor points for seeds every _distance
    for (int i = 0; i < nTimes; i++) {
        _anchorLine.marks.push_back(seed);
        seed += m_label.width+margin;
    }
}

void glmFeatureLabelLine::seedAnchorOnSegmentsAt(glmSmartLine &_anchorLine, float _minDistance, float _maxDistance){
    
    float lastSeed = 0.0;
    for (int i = 0; i < _anchorLine.size()-1; i++) {
        float offset = _anchorLine.getDistances()[i];
        
        float segmentLength = _anchorLine.getDistances()[i+1]-_anchorLine.getDistances()[i];//_anchorLine.getPolars()[i].r;
        
        //  Fits?
        //
        if( segmentLength >= (m_label.width+_maxDistance) ){

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
                    seed += m_label.width+margin;
                }
            }
            
        } else if ( segmentLength >= m_label.width+_minDistance){
            
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

void glmFeatureLabelLine::draw2D(){
    if(m_font!=NULL&&m_text!="NONE"&&bVisible){
        if(m_cameraPos!=0 && bVisible){
            float angle = glm::dot(glm::normalize( *m_cameraPos - shapes[0].getCentroid()),glm::vec3(0.,0.,1.));
            m_alpha = lerpValue(m_alpha,powf( CLAMP(angle,0.01,1.0), 1.15 ),0.1);
        } else {
            m_alpha = lerpValue(m_alpha,0.0, 0.1);
        }
        
        if(m_alpha > 0.1){
            for (auto &it: m_anchorLines) {
                if(it.bLetterByLetter){
                    drawLetterByLetter(it);
                } else {
                    drawAllTextAtOnce(it);
                }
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
                
                glPushMatrix();
                glTranslated(it[i].x, it[i].y, it[i].z);
                glScalef(0.75,-0.75,1);
                glRotated(it.getPolars()[i-1].a*RAD_TO_DEG, 0, 0, -1);
                glTranslated(5.,3.,0.);
                m_font->drawString( toString( (int)it.getDistances()[i]), m_alpha );
                glPopMatrix();
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
        
        glm::vec3 src = _anchorLine.getPositionAt(_offset);
        
        if(screen.inside(src)){
            double rot = _anchorLine.getAngleAt(_offset);
            
            glmRectangle boundingBox = glmPolyline(m_label,angle).getBoundingBox();
            boundingBox.translate(_anchorLine.getPositionAt(_offset+m_label.width*0.5));
//            boundingBox.drawCorners(); // Draw boundign box for debug
            
            bool bOver = false;
            
            for (int i = 0; i < pointLabels->size(); i++ ){
                if(pointLabels->at(i)->bVisible){
                    if( boundingBox.intersects(pointLabels->at(i)->getLabel(0) ) ){
                        bOver = true;
                        break;
                    }
                }
            }
            
            if(!bOver){
                if(angle < PI*0.5 && angle > -PI*0.5){
                    
                    glPushMatrix();
                    glTranslated(src.x, src.y, src.z);
                    
                    glScalef(1,-1,1);
                    glRotated(rot*RAD_TO_DEG, 0, 0, -1);
                    
                    glScaled(-1, -1, 1);
                    glTranslated(-m_label.width, 0, 0);
                    
                    glTranslatef(0., -m_label.height*0.5,0.);
                    m_font->drawString( m_text , m_alpha);
                    glPopMatrix();
                    
                } else {
                    
                    glPushMatrix();
                    glTranslated(src.x, src.y, src.z);
                    
                    glScalef(1,-1,1);
                    glRotated(rot*RAD_TO_DEG, 0, 0, -1);
                    
                    glTranslatef(0., -m_label.height*0.5,0.);
                    m_font->drawString( m_text, m_alpha );
                    
                    glPopMatrix();
                    
                }
            }
        }
        
    }
}

void glmFeatureLabelLine::drawWordByWord(const glmSmartLine &_anchorLine){
    //  TODO:
    //
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
                    m_font->drawString( std::string(1,m_text[i]), m_alpha );
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
                    m_font->drawString( std::string(1,m_text[i]), m_alpha );
                    
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