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
    
    void    seedAnchorAt(float _pct);
    void    seedAnchorsEvery(float _distance);
    
    glmPolyline polyline;
    glm::vec3   m_centroid;

protected:
    void    updateCached();

    void    drawWordByWord(float _offset, const glm::vec3 &_camPos);
    void    drawLetterByLetter(float _offset, const glm::vec3 &_camPos);
    
    glmSmartLine m_anchorLine;
    glmRectangle m_label;
    
    std::vector<float> m_anchorDistances;
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;