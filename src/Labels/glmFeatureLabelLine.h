//
//  glmFeatureLabelLine.h
//
//  Created by Patricio Gonzalez Vivo on 9/8/14.
//
//

#pragma once

#include "glmFeatureLabel.h"

#include "glmPolyline.h"
#include "glmAnchorLine.h"

#include "glmFeatureLabelPoint.h"

class glmFeatureLabelLine : public glmFeatureLabel{
public:
    
    glmFeatureLabelLine();
    glmFeatureLabelLine(const std::string &_text);
    virtual ~glmFeatureLabelLine();
    
    void    updateProjection();
    void    draw2D();
    void    draw3D();
    void    drawDebug();
    
    float   maxDistance;
    
    std::vector<glmFeatureLabelPointRef> *pointLabels;
    
protected:
    void    updateCached();

    void    seedAnchorAt(glmAnchorLine &_anchorLine, float _pct);
    void    seedAnchorsEvery(glmAnchorLine &_anchorLine, float _minDistance, float _maxDistance);
    void    seedAnchorOnSegmentsAt(glmAnchorLine &_anchorLine, float _minDistance, float _maxDistance);
    
    void    drawAllTextAtOnce(const glmAnchorLine &_anchorLine);
    void    drawWordByWord(const glmAnchorLine &_anchorLine);
    void    drawLetterByLetter(const glmAnchorLine &_anchorLine);
    
    //  Text cached data
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;