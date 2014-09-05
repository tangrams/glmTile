//
//  glmLabelPoint.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/2/14.
//
//

#include "glmLabelPoint.h"

glmLabelPoint::glmLabelPoint(): margin(5.0), angle(QUARTER_PI), bChanged(true) {
    
}

glmLabelPoint::glmLabelPoint(const glm::vec3 &_pos, const glmRectangle &_textBounding ){
    setPosition(_pos);
    setLabelBoundingBox(_textBounding);
}

glmLabelPoint::~glmLabelPoint(){
    
}

void glmLabelPoint::setPosition(const glm::vec3 &_pos){
    x = _pos.x;
    y = _pos.y;
    z = _pos.z;
    
    bChanged = true;
}

void glmLabelPoint::setLabelAngle(const double &_radiant){
    angle = _radiant;
    bChanged = true;
}

void glmLabelPoint::setLabelMargin(const float &_margin){
    margin = _margin;
    bChanged = true;
}

void glmLabelPoint::setLabelBoundingBox(const glmRectangle &_textBoundingBox ){
    label.width = _textBoundingBox.width;
    label.height = _textBoundingBox.height;
    bChanged = true;
}

void glmLabelPoint::update(){
    label.x = x + margin * cos(angle);
    label.y = y + margin * sin(-angle);
    
    if (angle <= QUARTER_PI && angle >= -QUARTER_PI){
        //  EAST
        //
        label.x += 0.0;
        label.y += mapValue(angle,
                            QUARTER_PI,     -QUARTER_PI,
                            -label.height,  0);
        
    } else if (angle >= QUARTER_PI && angle <= QUARTER_PI*3.){
        //  NORTH
        //
        label.x += mapValue(angle,
                            QUARTER_PI*3.,  QUARTER_PI,
                            -label.width,   0);
        label.y += -label.height;
    } else if (angle <= -QUARTER_PI && angle >= -QUARTER_PI*3.){
        //  SOUTH
        //
        label.x += mapValue(angle,
                            -QUARTER_PI*3., -QUARTER_PI,
                            -label.width,   0);
        label.y += 0.0;
    } else if (angle > QUARTER_PI*3. || angle < -QUARTER_PI*3. ){
        //  WEST
        //
        label.x -= label.width;
        
        if(angle > 0){
            label.y += mapValue(angle,
                                QUARTER_PI*3., PI,
                                -label.height, -label.height*0.5);
        } else {
            label.y += mapValue(angle,
                                -PI, -QUARTER_PI*3.,
                                -label.height*0.5,0.0);
        }
    }
    bChanged = false;
}

glmRectangle glmLabelPoint::getLabel() const {
    return label;
}

bool glmLabelPoint::isOver(const glmLabelPoint &_other){
    if(bChanged){
        update();
    }
    return label.intersects(_other.getLabel()) || isInside(_other);
}

bool glmLabelPoint::isInside(const glm::vec3 &_point){
    if(bChanged){
        update();
    }
    return label.inside(_point);
}

void glmLabelPoint::drawPoint(const float &_width){
    drawCross(*this,_width);
}

void glmLabelPoint::drawText(glmText &_text){
    if(bChanged){
        update();
    }
    _text.drawOnRectangle(label);
}

void glmLabelPoint::drawLabel(const float &_coornersWidth){
    if(bChanged){
        update();
    }
    
    if(_coornersWidth == 0.0){
        label.drawBorders();
    } else {
        label.drawCorners(_coornersWidth);
    }
}
