//
//  glmAnchorLine.cpp
//  Labeling
//
//  Created by Patricio Gonzalez Vivo on 9/26/14.
//
//

#include "glmAnchorLine.h"

AnchorSegment::AnchorSegment():m_nLevel(0),m_bVisible(false){
    
}

void AnchorSegment::more(){
    
    //  Increment one level
    m_nLevel++;
    
    //  Calculate the total of equidistant points and the step between them
    int nTotal = powf(2, m_nLevel);
    float step = 1./(float)nTotal;
    
    //  New marks are going to be computed apart and the appended
    std::vector<AnchorMark> newMarks;
    
    //  Add the step (or definition) at the begining
    newMarks.push_back( AnchorMark(step) );
    
    // For each value recorderd add
    for (int i = 0 ; i < m_marks.size(); i++) {
        newMarks.push_back( AnchorMark(m_marks[i].m_pct+step) );
    }
    
    //  Append the new marks
    m_marks.insert(m_marks.end(), newMarks.begin(), newMarks.end());
    
    // on Level     0   //  1           //  2                           //
    //  Add         1   //  2           //  4                           //
    
    // on Memory looks like:
    //  ->          0.5 //  0.25 , 0.75 //  0.125, 0.375, 0.625, 0.875  //
    
    // In these way is easy to delete (you delete the pow of the level-1 )
}

void AnchorSegment::less(){
    
    if(m_nLevel == 0){
        return;
    }
    
    m_nLevel--;
    
    if(m_nLevel == 0){
        clearMarks();
        return;
    }
    
    int nTotal = pow(2, m_nLevel);
    m_marks.erase(m_marks.end()-nTotal,m_marks.end());
}

int AnchorSegment::fit(float _min, float _max, float _at){
    
    //  What level should have?
    //  ( The number of labels is exponential to the level in order to make them consistent over several zoom distances, the grow must be exponential )
    int nLevel = (int)sqrt(_at/_max);
    
    if (nLevel == 0){
        nLevel = (int)sqrt(_at/_min);
    }
    
    if (nLevel>7) {
        nLevel = 4;
    }
    
    //  Increase the level to fill the line with labels
    while ( m_nLevel != nLevel ) {
        if( m_nLevel < nLevel){
            more();
        } else if (m_nLevel > nLevel){
            less();
        }
    }
    
    return m_marks.size();
}

void AnchorSegment::clearMarks(){
    m_marks.clear();
    m_nLevel = 0;
    m_bVisible = false;
}


glmAnchorLine::glmAnchorLine():m_nSegmentLabels(0){
    
};

void glmAnchorLine::project( const glmPolyline &_poly, const glm::mat4x4 &_mvmatrix, const glm::mat4x4 &_projmatrix, const glm::ivec4 &_viewport ){
    glmRectangle viewport = glmRectangle(_viewport);
    
    //  Pair segments data
    //
    if( m_segmentsMarks.size() != _poly.size() || m_points.size() != _poly.size()){
        m_points.resize(_poly.size());
        m_segmentsMarks.resize(_poly.size());
    }
    
    //  Update verteces
    //
    m_bVisible = false;
    bool bClipPrev = false;
    bool bPrevNonVisible = false;
    for (int i = 0; i < _poly.size(); i++) {
        
        glm::vec3 v = glm::project(_poly[i], _mvmatrix, _projmatrix, _viewport);
        bool insideViewport = viewport.inside(v);
        bool isVisible = ( v.z > 0.9 && v.z <= 1.0 && insideViewport);

        m_points[i] = v;
        
        if(!insideViewport || bClipPrev){
            glmRectangle vp = glmRectangle(_viewport);
            
            if(i > 0){
                vp.clip(m_points[i-1], m_points[i]);
                bClipPrev = false;
                isVisible = true;
            } else {
                bClipPrev = true;
            }
        }
        
        //  With only one visible segment ALL the line is tag as visible
        //
        if(isVisible){
            m_bVisible = true;
        } else {
            
            if(bPrevNonVisible){
                m_segmentsMarks.erase(m_segmentsMarks.begin()+i-1, m_segmentsMarks.end());
                break;
            } else {
                bPrevNonVisible = true;
            }
        }
            
        //  For each segment (last point don't have segment)
        //
        m_segmentsMarks[i].m_bVisible = isVisible;
    }

    //  Update distances and angles
    //
    updateCache();
}

void glmAnchorLine::clear(){
    glmFastPolyline::clear();
    
    m_marks.clear();
    m_segmentsMarks.clear();
}