/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkHardConnectedComponentImageFilter.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkHardConnectedComponentImageFilter_txx
#define _itkHardConnectedComponentImageFilter_txx

#include "itkImageRegionIterator.h"

namespace itk
{
template< class TInputImage, class TOutputImage >
void
HardConnectedComponentImageFilter< TInputImage, TOutputImage >
::GenerateData()
{
  int i,p,q,m;

  unsigned short eq_tab[65535];
  unsigned char flags[65535];
  OutputPixelType    label,max_label = 0;
  IndexType          index,current;
  SizeType           size;
  ListType::iterator  iter;
  TOutputImage::RegionType region;

  typename TOutputImage::Pointer output = this->GetOutput();
  typename TInputImage::Pointer input = this->GetInput();

  size = input->GetLargestPossibleRegion().GetSize();
  index = IndexType::ZeroIndex;
  region.SetSize(size);
  region.SetIndex(index);
  output->SetLargestPossibleRegion( region );
  output->SetBufferedRegion( region );
  output->SetRequestedRegion( region );
  output->Allocate();

  itk::ImageRegionIterator <TInputImage> it(input,input->GetRequestedRegion());
  itk::ImageRegionIterator <TOutputImage> ot(output,output->GetRequestedRegion());

  it.GoToBegin();
  ot.GoToBegin();
  for(;!it.IsAtEnd(); ++it,++ot)
    if(it.Get() != 0)
      ot.Set(65535);

  eq_tab[0] = 0;
  ot.GoToBegin();
  for(;!ot.IsAtEnd(); ++ot)
    {
      if(ot.Get()) 
        {
          for(i = 0;i<ImageDimension;i++)
            {          
              current = ot.GetIndex();
              current[i] = current[i]-1;
              if(current[i]<0)
                label = 0;
              else
                label = output->GetPixel(current);
              if(label)
                if(ot.Get() == 65535)
                  ot.Set(label);
                else if((ot.Get() != label) && (eq_tab[ot.Get()] != eq_tab[label]))
                   if(eq_tab[ot.Get()] > eq_tab[label])
                     {
                       q = eq_tab[ot.Get()];
                       for(p = q;p<=max_label;p++)
                         if(eq_tab[p] == q)
                           eq_tab[p] = eq_tab[label];
                     }
                   else
                     {
                       q = eq_tab[label];
                       for(p = q;p<=max_label;p++)
                         if(eq_tab[p] == q)
                           eq_tab[p] = eq_tab[ot.Get()];
                     }
            }
          if(ot.Get() == 65535)
            {
              ++max_label;
              eq_tab[max_label] = max_label;
              ot.Set(max_label);
              if(max_label == 65535)
                return;
            }
        }
    }

  for(p = 1;p<=max_label;p++)
    {
      for(m = p; (m<=max_label) && (eq_tab[m]!=p); m++);
      if(m>max_label)
        {
          for(m = p; eq_tab[m]<p; m++);
          if(m<=max_label)
            {
              for(i = m;i<max_label;i++)
                if(eq_tab[i] == m)
                  eq_tab[i] = p;
            }
        }
    }

  memset(flags,0,max_label+1);
  for(iter = m_Seeds.begin();iter!=m_Seeds.end();iter++)
    {
      current = *iter;
      m = eq_tab[output->GetPixel(current)];
      for(i = m;i<=max_label;i++)
        if(eq_tab[i] == m)
          flags[i] = 1;
    }

  ot.GoToBegin();
  if(m_Seeds.size()==0)
    for(;!ot.IsAtEnd(); ++ot)
      ot.Set(eq_tab[ot.Get()]);
  else
    for(;!ot.IsAtEnd(); ++ot)
      ot.Set(flags[ot.Get()]);

}

} // end namespace itk

#endif
