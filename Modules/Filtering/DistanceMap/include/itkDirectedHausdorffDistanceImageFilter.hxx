/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkDirectedHausdorffDistanceImageFilter_hxx
#define __itkDirectedHausdorffDistanceImageFilter_hxx

#include "itkDirectedHausdorffDistanceImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkProgressReporter.h"

namespace itk
{

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::DirectedHausdorffDistanceImageFilter():m_MaxDistance(1)
{
  // this filter requires two input images
  this->SetNumberOfRequiredInputs(2);

  m_DistanceMap = NULL;
  m_DirectedHausdorffDistance = NumericTraits< RealType >::Zero;
  m_AverageHausdorffDistance = NumericTraits< RealType >::Zero;
  m_UseImageSpacing     = true;
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::SetInput1(const TInputImage1 *image)
{
  this->SetInput( image );
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::SetInput2(const TInputImage2 *image)
{
  this->SetNthInput( 1, const_cast< TInputImage2 * >( image ) );
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
const typename DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::InputImage1Type *
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::GetInput1()
{
  return this->GetInput();
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
const typename DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::InputImage2Type *
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::GetInput2()
{
  return static_cast< const TInputImage2 * >
         ( this->ProcessObject::GetInput(1) );
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();

  // this filter requires:
  // - the largeset possible region of the first image
  // - the corresponding region of the second image
  if ( this->GetInput1() )
    {
    InputImage1Pointer image1 =
      const_cast< InputImage1Type * >( this->GetInput1() );
    image1->SetRequestedRegionToLargestPossibleRegion();

    if ( this->GetInput2() )
      {
      InputImage2Pointer image2 =
        const_cast< InputImage2Type * >( this->GetInput2() );
      RegionType region = image1->GetRequestedRegion();
      image2->SetRequestedRegion( region );
      }
    }
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::EnlargeOutputRequestedRegion(DataObject *data)
{
  Superclass::EnlargeOutputRequestedRegion(data);
  data->SetRequestedRegionToLargestPossibleRegion();
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::AllocateOutputs()
{
  // Pass the first input through as the output // The output images are really useless in this case
  InputImage1Pointer image =
    const_cast< TInputImage1 * >( this->GetInput1() );

  this->GraftOutput(image);
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::BeforeThreadedGenerateData()
{
  ThreadIdType numberOfThreads = this->GetNumberOfThreads();

  // Resize the thread temporaries
  m_MaxDistance.SetSize(numberOfThreads);
  m_PixelCount.SetSize(numberOfThreads);
  m_Sum.resize(numberOfThreads);

  // Initialize the temporaries
  m_MaxDistance.Fill(NumericTraits< RealType >::Zero);
  m_PixelCount.Fill(0);

  // Compute distance from non-zero pixels in the second image
  typename DistanceMapGenerationFilter::Pointer filter = DistanceMapGenerationFilter::New();

  filter->SetInput( this->GetInput2() );
  filter->SetSquaredDistance(false);
  filter->SetUseImageSpacing(m_UseImageSpacing);
  filter->SetInsideIsPositive(true);
  filter->Update();

  m_DistanceMap = filter->GetOutput();
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::AfterThreadedGenerateData()
{
  ThreadIdType numberOfThreads = this->GetNumberOfThreads();

  m_DirectedHausdorffDistance = NumericTraits< RealType >::Zero;

  RealType        sum = NumericTraits< RealType >::Zero;
  IdentifierType  pixelcount = 0;

  // find max over all threads
  for ( ThreadIdType i = 0; i < numberOfThreads; i++ )
    {
    if ( m_MaxDistance[i] > m_DirectedHausdorffDistance )
      {
      m_DirectedHausdorffDistance = m_MaxDistance[i];
      }
    pixelcount += m_PixelCount[i];
    sum += m_Sum[i].GetSum();
    }

  if( pixelcount != 0 )
    {
    m_AverageHausdorffDistance = sum / static_cast< RealType >( pixelcount );
    }
  else
    {
    itkGenericExceptionMacro( <<"pixelcount is equal to 0" );
    }

  // clean up
  m_DistanceMap = NULL;
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::ThreadedGenerateData(const RegionType & regionForThread,
                       ThreadIdType threadId)
{
  ImageRegionConstIterator< TInputImage1 >    it1 (this->GetInput1(), regionForThread);
  ImageRegionConstIterator< DistanceMapType > it2 (m_DistanceMap, regionForThread);

  // support progress methods/callbacks
  ProgressReporter progress( this, threadId, regionForThread.GetNumberOfPixels() );

  // do the work
  while ( !it1.IsAtEnd() )
    {
    if ( it1.Get() != NumericTraits< InputImage1PixelType >::Zero )
      {
      const RealType val2 = static_cast< RealType >( it2.Get() );
      if ( val2 > m_MaxDistance[threadId] )
        {
        m_MaxDistance[threadId] = val2;
        }
      m_PixelCount[threadId]++;
      m_Sum[threadId].AddElement(val2);
      }

    ++it1;
    ++it2;

    progress.CompletedPixel();
    }
}

template< class TInputImage1, class TInputImage2, class DistanceMapGenerationFilter >
void
DirectedHausdorffDistanceImageFilter< TInputImage1, TInputImage2, DistanceMapGenerationFilter >
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "DirectedHausdorffDistance: "
     << m_DirectedHausdorffDistance << std::endl;
  os << indent << "AverageHausdorffDistance: "
     << m_AverageHausdorffDistance << std::endl;
  os << indent << "Use Image Spacing : "
     << m_UseImageSpacing << std::endl;
}
} // end namespace itk
#endif
