//
//  glmFeatureLabelLine.h
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#pragma once

#include "glmFeatureLabel.h"

#include "glmPolyline.h"
#include "glmSmartLine.h"

class glmFeatureLabelLine : public glmFeatureLabel{
public:
    
    glmFeatureLabelLine();
    virtual ~glmFeatureLabelLine();
    
    void    updateProjection();
    void    draw();
    
    void    seedAnchorAt(float _pct);
    void    seedAnchorsEvery(float _distance);
    
    glmPolyline polyline;

protected:
    void    updateCached();

    void    drawWordByWord(float _offset);
    void    drawLetterByLetter(float _offset);
    
    glmSmartLine m_anchorLine;
    glmRectangle m_label;
    
    std::vector<float> m_anchorDistances;
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;