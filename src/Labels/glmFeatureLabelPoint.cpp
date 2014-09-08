//
//  glmFeatureLabelPoint.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#include "glmFeatureLabelPoint.h"

glmFeatureLabelPoint::glmFeatureLabelPoint():m_centroid(0.0,0.0,0.0), m_anchorPoint(0.0,0.0,0.0), m_margin(8.0), m_angle(HALF_PI) {
};

glmFeatureLabelPoint::~glmFeatureLabelPoint(){

};

void glmFeatureLabelPoint::setPosition(const glm::vec3 &_pos){
    m_centroid = _pos;
    m_bChanged = true;
}

void glmFeatureLabelPoint::setLabelAngle(const double &_radiant){
    m_angle = _radiant;
    m_bChanged = true;
}

void glmFeatureLabelPoint::setLabelMargin(const float &_margin){
    m_margin = _margin;
    m_bChanged = true;
}

glm::vec3 glmFeatureLabelPoint::getScreenPosition() const{
    return m_anchorPoint;
}

glmRectangle glmFeatureLabelPoint::getLabel() const{
    return m_label;
}

glm::vec3 glmFeatureLabelPoint::getAnchorPoint() const{
    return m_anchorPoint;
}

bool glmFeatureLabelPoint::isOver(const glmFeatureLabelPoint *_other){
    if(m_bChanged){
        updateCached();
    }
    return m_label.intersects(_other->getLabel()) || isInside(_other->getAnchorPoint());
}

bool glmFeatureLabelPoint::isInside(const glm::vec3 &_point){
    if(m_bChanged){
        updateCached();
    }
    return m_label.inside(_point);
}

void glmFeatureLabelPoint::updateProjection(){
    
    if(m_font!=NULL&&m_text!="NONE"){
        glm::ivec4 viewport;
        glm::mat4x4 mvmatrix, projmatrix;
        glGetIntegerv(GL_VIEWPORT, &viewport[0]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &mvmatrix[0][0]);
        glGetFloatv(GL_PROJECTION_MATRIX, &projmatrix[0][0]);
        
        if(m_bChanged){
            updateCached();
        }
        
        m_anchorPoint = glm::project(m_centroid, mvmatrix, projmatrix, viewport);
        
        m_label.x = m_anchorPoint.x + m_margin * cos(m_angle);
        m_label.y = m_anchorPoint.y + m_margin * sin(-m_angle);
        
        if (m_angle <= QUARTER_PI && m_angle >= -QUARTER_PI){
            //  EAST
            //
            m_label.x += 0.0;
            m_label.y += mapValue(m_angle,
                                  QUARTER_PI,     -QUARTER_PI,
                                  -m_label.height,  0);
            
        } else if (m_angle >= QUARTER_PI && m_angle <= QUARTER_PI*3.){
            //  NORTH
            //
            m_label.x += mapValue(m_angle,
                                  QUARTER_PI*3.,  QUARTER_PI,
                                  -m_label.width,   0);
            m_label.y += -m_label.height;
        } else if (m_angle <= -QUARTER_PI && m_angle >= -QUARTER_PI*3.){
            //  SOUTH
            //
            m_label.x += mapValue(m_angle,
                                  -QUARTER_PI*3., -QUARTER_PI,
                                  -m_label.width,   0);
            m_label.y += 0.0;
        } else if (m_angle > QUARTER_PI*3. || m_angle < -QUARTER_PI*3. ){
            //  WEST
            //
            m_label.x -= m_label.width;
            
            if(m_angle > 0){
                m_label.y += mapValue(m_angle,
                                      QUARTER_PI*3., PI,
                                      -m_label.height, -m_label.height*0.5);
            } else {
                m_label.y += mapValue(m_angle,
                                      -PI, -QUARTER_PI*3.,
                                      -m_label.height*0.5,0.0);
            }
        }
        
        bVisible = m_anchorPoint.z >= 0.0 && m_anchorPoint.z <= 1.0;
    } else {
        bVisible = false;
    }
};

void glmFeatureLabelPoint::updateCached(){
    if(m_font!=NULL&&m_text!="NONE"){
        m_label = m_font->getStringBoundingBox(m_text);
        m_bChanged = false;
    }
}

void glmFeatureLabelPoint::draw(){
    if(m_font != NULL){
        if(m_bChanged){
            updateCached();
        }
        
        drawCross(m_anchorPoint,3.0);
//        m_label.drawCorners(3.0);
//        m_label.drawBorders();
        
        m_font->drawString(m_text, m_label.getBottomLeft());
    }
}
