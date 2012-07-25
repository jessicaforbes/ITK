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
#ifndef __itkVectorImageToImagePixelAccessor_h
#define __itkVectorImageToImagePixelAccessor_h

#include "itkMacro.h"

namespace itk
{
namespace Accessor
{

/** \class VectorImageToImagePixelAccessor
 * \brief Extract components from a VectorImage.
 *
 * This accessor is used to extract components from a VectorImage. It is used
 * from VectorImageComponentExtractAdaptor. The component to extract is set
 * using SetExtractComponentIdx.
 *
 * \note
 * This work is part of the National Alliance for Medical Image Computing
 * (NAMIC), funded by the National Institutes of Health through the NIH Roadmap
 * for Medical Research, Grant U54 EB005149.
 * \sa ImageAdaptor
 * \sa PixelAccessor
 *
 * \ingroup ImageAdaptors
 * \ingroup ITKImageAdaptors
 */
template< class TType >
class ITK_EXPORT VectorImageToImagePixelAccessor
  : private DefaultVectorPixelAccessor< TType >
{
public:
  /** Standard class typedefs. */
  typedef VectorImageToImagePixelAccessor Self;

  /** External typedef. It defines the external aspect
   * that this class will exhibit. */
  typedef TType ExternalType;

  /** Internal typedef used by the ImageAdaptor for the buffer pointer */
  /** Internal typedef. It defines the internal real
   * representation of data */
  typedef TType InternalType;

  typedef unsigned int VectorLengthType;
  typedef VariableLengthVector< TType > ActualPixelType;

  inline void Set(ActualPixelType output, const ExternalType & input) const
  {
    output[m_ComponentIdx] = input;
  }

  inline void Set(InternalType &output, const ExternalType & input,
                  const unsigned long offset) const
  {
    return Set( Superclass::Get( output, offset ), input );
  }

  inline ExternalType Get(const ActualPixelType & input) const
  {
    ExternalType output;

    output = input[m_ComponentIdx];
    return output;
  }

  inline ExternalType Get(const InternalType &input, const SizeValueType offset) const
  {
    return Get( Superclass::Get(input, offset) );
  }

  void SetExtractComponentIdx(VectorLengthType idx)
  {
    m_ComponentIdx = idx;
  }

  VectorLengthType GetExtractComponentIdx() const
  {
    return m_ComponentIdx;
  }

  /** Set the length of each vector in the VectorImage */
  void SetVectorLength(VectorLengthType l)
  {
    Superclass::SetVectorLength( l );
  }

  /** Get Vector lengths */
  VectorLengthType GetVectorLength() const { return Superclass::GetVectorLength(); }

  VectorImageToImagePixelAccessor( unsigned int length = 1)
    :m_ComponentIdx(0)
    {
    Superclass::SetVectorLength( length );
    }

protected:
  typedef DefaultVectorPixelAccessor< TType > Superclass;

private:
  VectorLengthType m_ComponentIdx;
};
} // end namespace Accessor
} // end namespace itk

#endif // __itkVectorImageToImagePixelAccessor_h
