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
    void    draw(const glm::vec3 &_camPos = glm::vec3(0.,0.,0.));
    void    drawLine();

    glmPolyline polyline;
    
    std::vector<glmPolyline> blocks;

protected:
    void    updateCached();

    void    seedAnchorAt(glmSmartLine &_anchorLine, float _pct);
    void    seedAnchorsEvery(glmSmartLine &_anchorLine, float _distance);
    void    seedAnchorOnSegmentsAt(glmSmartLine &_anchorLine, float _minDistance);
    
    void    drawWordByWord(const glmSmartLine &_anchorLine, const glm::vec3 &_camPos);
    void    drawLetterByLetter(const glmSmartLine &_anchorLine, const glm::vec3 &_camPos);
    
    //  Dynamic projected 2D labels lines and anchors
    std::vector<glmSmartLine> m_anchorLines;
    
    //  Text cached data
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
    glmRectangle m_label;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;