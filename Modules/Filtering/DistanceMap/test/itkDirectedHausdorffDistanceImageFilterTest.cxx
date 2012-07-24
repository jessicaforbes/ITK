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
#include "itkImageFileWriter.h"

#include "itkDirectedHausdorffDistanceImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"

int itkDirectedHausdorffDistanceImageFilterTest( int , char * [] )
{
  typedef double Pixel1Type;
  typedef double Pixel2Type;
  enum { ImageDimension = 3 };

  typedef itk::Image<Pixel1Type,ImageDimension> Image1Type;
  typedef itk::Image<Pixel2Type,ImageDimension> Image2Type;

  typedef itk::ImageFileWriter<Image2Type>  WriterType;

  Image1Type::Pointer image1 = Image1Type::New();
  Image2Type::Pointer image2 = Image2Type::New();
    { //Make the images
    Image1Type::SizeType size;
    size.Fill( 10 );

    image1->SetRegions( size );
    image2->SetRegions( size );

    image1->Allocate();
    image2->Allocate();

    image1->FillBuffer( itk::NumericTraits<Pixel1Type>::Zero );
    image2->FillBuffer( itk::NumericTraits<Pixel2Type>::Zero );

    typedef Image1Type::RegionType RegionType;
    RegionType region1;
    RegionType region2;

    typedef Image1Type::IndexType IndexType;
    IndexType index;

    size.Fill( 4 );
    index.Fill( 2 );
    region1.SetSize( size );
    region1.SetIndex( index );

    size.Fill( 3 );
    index.Fill( 4 );
    region2.SetSize( size );
    region2.SetIndex( index );

      {
      itk::ImageRegionIterator<Image1Type> it1( image1, region1 );
      while ( !it1.IsAtEnd() )
        {
        it1.Set( itk::NumericTraits<Pixel1Type>::One );
        ++it1;
        }
      }
    itk::ImageRegionIterator<Image2Type> it2( image2, region2 );
    while ( !it2.IsAtEnd() )
      {
      it2.Set( itk::NumericTraits<Pixel2Type>::One );
      ++it2;
      }

      {
      typedef itk::ImageFileWriter<Image1Type>   WriterType;
      WriterType::Pointer writer = WriterType::New();
      writer->SetInput( image1 );
      writer->SetFileName( "i1.nii.gz" );
      writer->Update();
      }

      {
      typedef itk::ImageFileWriter<Image1Type>   WriterType;
      WriterType::Pointer writer = WriterType::New();
      writer->SetInput( image2 );
      writer->SetFileName( "i2.nii.gz" );
      writer->Update();
      }
    }

    {
    typedef itk::DirectedHausdorffDistanceImageFilter
      <Image1Type, Image2Type, itk::SignedMaurerDistanceMapImageFilter< Image1Type, Image2Type > >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput1( image1 );
    filter->SetInput2( image2 );
    filter->SetUseImageSpacing( true  );
    filter->Update();
    filter->Print(std::cout);

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( filter->GetOutput() );
    writer->SetFileName( "dh_sm.nii.gz" );
    writer->Update();
    }

    {
    typedef itk::DirectedHausdorffDistanceImageFilter
      <Image1Type, Image2Type, itk::SignedDanielssonDistanceMapImageFilter< Image1Type, Image2Type > >  FilterType;

    FilterType::Pointer filter = FilterType::New();
    filter->SetInput1( image1 );
    filter->SetInput2( image2 );
    filter->SetUseImageSpacing( true  );
    filter->Update();
    filter->Print(std::cout);

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( filter->GetOutput() );
    writer->SetFileName( "dh_dd.nii.gz" );
    writer->Update();
    }

  return EXIT_SUCCESS;
}
