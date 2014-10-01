//
//  glmAnchorLine.h
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/26/14.
//
//

#pragma once

#include "glmFastPolyline.h"

struct AnchorMark {
    AnchorMark():m_pct(0.0),m_alpha(0.0){};
    AnchorMark(float _pct):m_pct(_pct),m_alpha(0.0){};
    
    float m_pct;
    float m_alpha;
};

class AnchorSegment {
public:
    
    AnchorSegment();
    
    int     fit(float _min, float _max, float _at);
    void    more();
    void    less();
    void    clearMarks();
    
    std::vector<AnchorMark> m_marks;
    
    int     m_nLevel;
    bool    m_bVisible;
};

class glmAnchorLine : public glmFastPolyline, public AnchorSegment {
public:
    
    glmAnchorLine();
    
    virtual void project( const glmPolyline &_poly, const glm::mat4x4 &_mvmatrix, const glm::mat4x4 &_projmatrix, const glm::ivec4 &_viewport );
    virtual void clear();
    
    std::vector< AnchorSegment > m_segmentsMarks;
    
    int m_nSegmentLabels;
};