#pragma once
#include "animaRefactoredAsymmetricBMRegistrationMethod.h"

#include <animaBalooSVFTransformAgregator.h>
#include <animaDenseSVFTransformAgregator.h>

namespace anima
{

template <typename TInputImageType>
void
RefactoredAsymmetricBMRegistrationMethod <TInputImageType>
::PerformOneIteration(InputImageType *refImage, InputImageType *movingImage, TransformPointer &addOn)
{
    itk::TimeProbe tmpTime;
    tmpTime.Start();

    this->GetBlockMatcher()->SetForceComputeBlocks(false);
    this->GetBlockMatcher()->SetReferenceImage(this->GetFixedImage());
    this->GetBlockMatcher()->SetMovingImage(movingImage);
    this->GetBlockMatcher()->Update();

    tmpTime.Stop();
    std::cout << "Matching performed in " << tmpTime.GetTotal() << std::endl;

    this->GetAgregator()->SetInputRegions(this->GetFixedImage(), this->GetBlockMatcher()->GetBlockRegions());

    if (this->GetAgregator()->GetOutputTransformType() == AgregatorType::SVF)
    {
        typedef anima::BalooSVFTransformAgregator<InputImageType::ImageDimension> SVFAgregatorType;
        SVFAgregatorType *tmpAgreg = dynamic_cast <SVFAgregatorType *> (this->GetAgregator());

        if (tmpAgreg)
            tmpAgreg->SetDamIndexes(this->GetBlockMatcher()->GetDamIndexes());
        else
        {
            typedef anima::DenseSVFTransformAgregator<InputImageType::ImageDimension> SVFAgregatorType;
            SVFAgregatorType *tmpDenseAgreg = dynamic_cast <SVFAgregatorType *> (this->GetAgregator());

            tmpDenseAgreg->SetDamIndexes(this->GetBlockMatcher()->GetDamIndexes());
        }
    }

    this->GetAgregator()->SetInputWeights(this->GetBlockMatcher()->GetBlockWeights());
    this->GetAgregator()->SetInputTransforms(this->GetBlockMatcher()->GetBlockTransformPointers());

    addOn = this->GetAgregator()->GetOutput();
}

}
