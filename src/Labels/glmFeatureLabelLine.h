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
    
    float   getDepth() const;
    void    updateProjection();
    void    draw();
    
    glmPolyline polyline;
    
    glm::vec2 offset;
    
protected:
    void    updateCached();
    
    glmSmartLine m_anchorLine;
    glmRectangle m_label;
    
    std::vector<float> m_wordsWidth;
    std::vector<float> m_lettersWidth;
};

typedef std::tr1::shared_ptr<glmFeatureLabelLine> glmFeatureLabelLineRef;