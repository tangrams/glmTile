//
//  glmLabelPoint.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/2/14.
//
//

#include "glmLabelPoint.h"

glmLabelPoint::glmLabelPoint(float _x, float _y, float _text_width){
    x = _x;
    y = _y;
    z = 0.;
    label.width = _text_width;
    label.height = 14;
    label.x = 0;
    label.y = -label.height;
    
    area = sqrt(label.width*label.width+label.height*label.height)*2.;
    
    glm::vec2 mutual_repulsion;
    mutual_repulsion.x = 2. * cos(QUARTER_PI);
    mutual_repulsion.y = 2. * sin(QUARTER_PI);
    angle = atan2(mutual_repulsion.y, mutual_repulsion.x);
    
    margin = 5.0;
}

bool glmLabelPoint::compute(glmLabelPoint &_other){
    
    if (glm::distance((glm::vec3)*this,(glm::vec3)_other) <= area){
        
        if(label.inside(_other)){
            glm::vec3 diff = (_other-label.getCenter());
            double a = atan2(diff.y,diff.x);
            if (a < -PI) a += PI*2.;
            if (a > PI) a -= PI*2.;
            
            angle += a*0.01;
            wrapRad(angle);
            
            return true;
        } else if(label.intersects(_other.label)){
            glm::vec3 diff = (_other.label.getCenter()-label.getCenter());
            double a = atan2(diff.y,diff.x);
            if (a < -PI) a += PI*2.;
            if (a > PI) a -= PI*2.;
            
            angle += a*0.01;
            wrapRad(angle);
            
            _other.angle -= a*0.01;
            wrapRad(_other.angle);
            
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
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
}
void glmLabelPoint::draw(bool _bDebug){
    glColor3f(1,1,1);
    drawCross(*this);
    
    if(_bDebug){
        
        glm::vec3 ancherPoint = glm::vec3(x + margin * cos(angle),
                                          y + margin * sin(-angle),
                                          0.0f);
        glColor3f(0.0,1.0,1.0);
        drawLine(*this, ancherPoint);
        
        glColor3f(1,0,0);
        label.drawBorders();
    }
}
