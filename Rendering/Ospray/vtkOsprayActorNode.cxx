/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOsprayActorNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOsprayActorNode.h"

#include "vtkActor.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataArray.h"
#include "vtkInformation.h"
#include "vtkInformationIntegerKey.h"
#include "vtkInformationObjectBaseKey.h"
#include "vtkInformationStringKey.h"
#include "vtkMapper.h"
#include "vtkObjectFactory.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPolyData.h"
#include "vtkViewNodeCollection.h"

#include "ospray/ospray.h"
#include "ospray/common/OSPCommon.h"

vtkInformationKeyMacro(vtkOsprayActorNode, ENABLE_SCALING, Integer);
vtkInformationKeyMacro(vtkOsprayActorNode, SCALE_ARRAY_NAME, String);
vtkInformationKeyMacro(vtkOsprayActorNode, SCALE_FUNCTION, ObjectBase);

//============================================================================
vtkStandardNewMacro(vtkOsprayActorNode);

//----------------------------------------------------------------------------
vtkOsprayActorNode::vtkOsprayActorNode()
{
}

//----------------------------------------------------------------------------
vtkOsprayActorNode::~vtkOsprayActorNode()
{
}

//----------------------------------------------------------------------------
void vtkOsprayActorNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkOsprayActorNode::SetEnableScaling(int value, vtkActor *actor)
{
  if (!actor)
    {
    return;
    }
  vtkMapper *mapper = actor->GetMapper();
  if (mapper)
    {
    vtkInformation *info = mapper->GetInformation();
    info->Set(vtkOsprayActorNode::ENABLE_SCALING(), value);
    }
}

//----------------------------------------------------------------------------
int vtkOsprayActorNode::GetEnableScaling(vtkActor *actor)
{
  if (!actor)
    {
    return 0;
    }
  vtkMapper *mapper = actor->GetMapper();
  if (mapper)
    {
    vtkInformation *info = mapper->GetInformation();
    if (info && info->Has(vtkOsprayActorNode::ENABLE_SCALING()))
      {
      return (info->Get(vtkOsprayActorNode::ENABLE_SCALING()));
      }
    }
  return 0;
}

//----------------------------------------------------------------------------
void vtkOsprayActorNode::SetScaleArrayName
  (const char *arrayName, vtkActor *actor)
{
  if (!actor)
    {
    return;
    }
  vtkMapper *mapper = actor->GetMapper();
  if (mapper)
    {
    vtkInformation *mapperInfo = mapper->GetInformation();
    mapperInfo->Set(vtkOsprayActorNode::SCALE_ARRAY_NAME(), arrayName);
    }
}

//----------------------------------------------------------------------------
void vtkOsprayActorNode::SetScaleFunction(vtkPiecewiseFunction *scaleFunction,
                                          vtkActor *actor)
{
  if (!actor)
    {
    return;
    }
  vtkMapper *mapper = actor->GetMapper();
  if (mapper)
    {
    vtkInformation *mapperInfo = mapper->GetInformation();
    mapperInfo->Set(vtkOsprayActorNode::SCALE_FUNCTION(), scaleFunction);
    }
}

//----------------------------------------------------------------------------
unsigned long vtkOsprayActorNode::GetMTime()
{
  unsigned long mtime = this->Superclass::GetMTime();
  vtkActor *act = (vtkActor*)this->GetRenderable();
  if (act->GetMTime() > mtime)
    {
    mtime = act->GetMTime();
    }
  vtkDataObject * dobj = NULL;
  vtkPolyData *poly = NULL;
  vtkMapper *mapper = act->GetMapper();
  if (mapper)
    {
    //if (act->GetRedrawMTime() > mtime)
    //  {
    //  mtime = act->GetRedrawMTime();
    // }
    if (mapper->GetMTime() > mtime)
      {
      mtime = mapper->GetMTime();
      }
    if (mapper->GetInformation()->GetMTime() > mtime)
      {
      mtime = mapper->GetInformation()->GetMTime();
      }
    dobj = mapper->GetInputDataObject(0, 0);
    poly = vtkPolyData::SafeDownCast(dobj);
    }
  if (poly)
    {
    if (poly->GetMTime() > mtime)
      {
      mtime = poly->GetMTime();
      }
    }
  else if (dobj)
    {
    vtkCompositeDataSet *comp = vtkCompositeDataSet::SafeDownCast
      (dobj);
    if (comp)
      {
      vtkCompositeDataIterator*dit = comp->NewIterator();
      dit->SkipEmptyNodesOn();
      while(!dit->IsDoneWithTraversal())
        {
        poly = vtkPolyData::SafeDownCast(comp->GetDataSet(dit));
        if (poly)
          {
          if (poly->GetMTime() > mtime)
            {
            mtime = poly->GetMTime();
            }
          }
        dit->GoToNextItem();
        }
      dit->Delete();
      }
    }
  return mtime;
}
