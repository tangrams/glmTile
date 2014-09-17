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
    
    float   maxDistance;
    
protected:
    void    updateCached();

    void    seedAnchorAt(glmSmartLine &_anchorLine, float _pct);
    void    seedAnchorsEvery(glmSmartLine &_anchorLine, float _minDistance, float _maxDistance);
    void    seedAnchorOnSegmentsAt(glmSmartLine &_anchorLine, float _minDistance, float _maxDistance);
    
    void    drawAllTextAtOnce(const glmSmartLine &_anchorLine);
    void    drawWordByWord(const glmSmartLine &_anchorLine);
    void    drawLetterByLetter(const glmSmartLine &_anchorLine);
    
    //  Text cached data
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;