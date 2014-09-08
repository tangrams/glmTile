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
    
    glm::vec3 getScreenPosition() const;
    void    updateProjection();
    void    draw();
    
    glmPolyline polyline;

protected:
    void    updateCached();
    
    void    drawWordByWord(float _offset);
    void    drawLetterByLetter(float _offset);
    
    glm::vec3   m_anchorPoint;
    glmSmartLine m_anchorLine;
    glmRectangle m_label;
    
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
    
    glm::vec2 m_offset;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;