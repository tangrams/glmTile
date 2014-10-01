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

glmFeatureLabelLine::glmFeatureLabelLine(const std::string &_text){
    setText(_text);
}

glmFeatureLabelLine::~glmFeatureLabelLine(){
    
}

int glmFeatureLabelLine::seedAnchorsEvery(glmAnchorLine &_anchorLine, float _minDistance){
    
    //  Update Fonts/Text cached values if something change
    //
    if(m_bChanged){
        updateCached();
    }

    return _anchorLine.fit(m_label.width+_minDistance, _anchorLine.getLength());
}

int glmFeatureLabelLine::seedAnchorOnSegmentsEvery(glmAnchorLine &_anchorLine, float _minDistance){
    //  Update Fonts/Text cached values if something change
    //
    if(m_bChanged){
        updateCached();
    }
    
    int count = 0;
    for (int i = 0 ; i < _anchorLine.m_segmentsMarks.size(); i++ ) {
        if (_anchorLine.m_segmentsMarks[i].m_bVisible) {
            count += _anchorLine.m_segmentsMarks[i].fit( m_label.width+_minDistance, _anchorLine.getPolars()[i].r );
        } else if ( _anchorLine.m_segmentsMarks[i].m_marks.size() > 0){
            _anchorLine.m_segmentsMarks[i].clear();
        }
    }
    
    return count;
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


void glmFeatureLabelLine::draw2D(){
    if(m_font!=NULL&&m_text!="NONE"&&bVisible){
        
        //  Global m_alpha
        //
        if(m_cameraPos!=0 && bVisible){
            float angle = glm::dot(glm::normalize( *m_cameraPos - shapes[0].getCentroid()),glm::vec3(0.,0.,1.));
            m_alpha = lerpValue(m_alpha,powf( CLAMP(angle,0.01,1.0), 0.6 ),0.1);
        } else {
            m_alpha = lerpValue(m_alpha,0.0, 0.1);
        }
        
        if(m_alpha > 0.1){
            for (auto &it: m_anchorLines) {
                drawTextOn(it);
            }
        }
        
    }
}

void glmFeatureLabelLine::drawTextOn( glmAnchorLine &_anchorLine ){
    
    if(_anchorLine.m_nSegmentLabels > 0){
        
        //  ALL THE TEXT at once ( for streight lines )
        //
        for (int i = 0; i < _anchorLine.size()-1; i++){
            
            //  For each populated
            //
            if(_anchorLine.m_segmentsMarks[i].m_marks.size() > 0 ){
                
                //  Compute offset and rotation
                //
                glm::vec3 segmentOffset = _anchorLine[i];
                glm::vec3 segmentDir = _anchorLine[i+1]-_anchorLine[i];
                float segmentAngle = _anchorLine.getPolars()[i].a;
                float segmentDistance = _anchorLine.getPolars()[i].r;
                
                for (auto &mark : _anchorLine.m_segmentsMarks[i].m_marks){
                
                    glm::vec3 src = segmentOffset + segmentDir*mark.m_pct;
                    glmRectangle boundingBox = glmPolyline(m_label,segmentAngle).getBoundingBox();
                    boundingBox.translate( segmentOffset + segmentDir*mark.m_pct  );
                    
                    bool bOver = false;
//                    for (int i = 0; i < pointLabels->size(); i++ ){
//                        if(pointLabels->at(i)->bVisible){
//                            if( boundingBox.intersects(pointLabels->at(i)->getLabel(0) ) ){
//                                bOver = true;
//                                break;
//                            }
//                        }
//                    }
                    
                    if(bVisible){
                        mark.m_alpha = lerpValue(mark.m_alpha,m_alpha,0.1);
                    } else {
                        mark.m_alpha = lerpValue(mark.m_alpha,0.0,0.1);
                    }
                    
                    if(!bOver){
                        
                        glPushMatrix();
                        glTranslated(src.x, src.y, src.z);
                        
                        glScalef(1,-1,1);
                        glRotated(segmentAngle*RAD_TO_DEG, 0, 0, -1);
                        
                        if(segmentAngle > HALF_PI && segmentAngle < HALF_PI){
                            glScaled(-1, -1, 1);
                        }
                        
                        glTranslatef(-m_label.width*0.5, -m_label.height*0.5,0.);
                        m_font->drawString( m_text, mark.m_alpha );
                        glPopMatrix();
                    }
                    
                }
            }
        }
        
    } else {
        
        //  LETTER BY LETTER ( for smooth curves )
        //
        for (auto &mark : _anchorLine.m_marks){
            
            //  Orientation
            //
            float startDist = mark.m_pct*_anchorLine.getLength()-m_label.width*0.5;
            float endDist = mark.m_pct*_anchorLine.getLength()+m_label.width*0.5;
            
            glm::vec3 diff = _anchorLine.getPositionAt(endDist)-_anchorLine.getPositionAt(startDist);
            float angle = atan2f(-diff.y, diff.x);
            
            bool bUpsideDown = (angle > HALF_PI && angle < HALF_PI);
            
            //  Fade
            //
            if(bVisible){
                mark.m_alpha = lerpValue(mark.m_alpha,m_alpha,0.1);
            } else {
                mark.m_alpha = lerpValue(mark.m_alpha,0.0,0.1);
            }
            
            float offset = startDist;
            for (int i = 0; i < m_text.length(); i++) {
                
                glm::vec3 src = _anchorLine.getPositionAt(offset);
                double rot = _anchorLine.getAngleAt(offset);
                
                glPushMatrix();
                glTranslated(src.x, src.y, src.z);
                
                glScalef(1.,-1.,1.);
                glRotated(rot*RAD_TO_DEG, 0., 0., -1.);
                
                int index = i;
                if(bUpsideDown){
                    index = (m_text.length()-1)-i;
                    glScaled(-1., -1., 1.);
                    glTranslated(-m_lettersWidth[index], 0, 0);
                }
                
                glTranslatef(0., -m_label.height*0.5,0.);
                m_font->drawString( std::string(1,m_text[index]), mark.m_alpha );
                
                glPopMatrix();
                offset += m_lettersWidth[index];
            }
            
        }
    }
}

void glmFeatureLabelLine::drawDebug(){
    glColor4f(m_font->colorFront.x,m_font->colorFront.y,m_font->colorFront.z,m_alpha);
    
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
                
                //  Print distances
                //
//                glPushMatrix();
//                glTranslated(it[i].x, it[i].y, it[i].z);
//                glScalef(0.75,-0.75,1);
//                glRotated(it.getPolars()[i-1].a*RAD_TO_DEG, 0, 0, -1);
//                glTranslated(5.,3.,0.);
//                m_font->drawString( toString( (int)it.getDistances()[i]), m_alpha );
//                glPopMatrix();
            }
        }
    }
}