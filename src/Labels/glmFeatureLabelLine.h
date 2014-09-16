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
    glmFeatureLabelLine(const std::string &_text);
    virtual ~glmFeatureLabelLine();
    
    void    updateProjection();
    void    draw2D();
    void    draw3D();
    void    drawDebug();
    
    float   labelsDistance;
    
protected:
    void    updateCached();

    void    seedAnchorAt(glmSmartLine &_anchorLine, float _pct);
    void    seedAnchorsEvery(glmSmartLine &_anchorLine, float _distance);
    void    seedAnchorOnSegmentsAt(glmSmartLine &_anchorLine, float _minDistance, float _maxDistance);
    
    void    drawAllTextAtOnce(const glmSmartLine &_anchorLine);
    void    drawWordByWord(const glmSmartLine &_anchorLine);
    void    drawLetterByLetter(const glmSmartLine &_anchorLine);
    
    //  Dynamic projected 2D label line and anchors
    std::vector<glmSmartLine> m_anchorLines;
    
    //  Text cached data
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
    glmRectangle m_label;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;