/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOsprayCameraNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOsprayCameraNode.h"

#include "vtkCamera.h"
#include "vtkCollectionIterator.h"
#include "vtkObjectFactory.h"
#include "vtkOsprayRendererNode.h"
#include "vtkRenderer.h"
#include "vtkViewNodeCollection.h"

#include "ospray/ospray.h"

//============================================================================
vtkStandardNewMacro(vtkOsprayCameraNode);

//----------------------------------------------------------------------------
vtkOsprayCameraNode::vtkOsprayCameraNode()
{
}

//----------------------------------------------------------------------------
vtkOsprayCameraNode::~vtkOsprayCameraNode()
{
}

//----------------------------------------------------------------------------
void vtkOsprayCameraNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkOsprayCameraNode::Render(bool prepass)
{
  if (prepass)
    {
    vtkOsprayRendererNode *orn =
      static_cast<vtkOsprayRendererNode *>(
        this->GetFirstAncestorOfType("vtkOsprayRendererNode"));

    vtkRenderer *ren = vtkRenderer::SafeDownCast(orn->GetRenderable());
    int tiledSize[2];
    int tiledOrigin[2];
    ren->GetTiledSizeAndOrigin(&tiledSize[0], &tiledSize[1],
                            &tiledOrigin[0], &tiledOrigin[1]);


    OSPCamera ospCamera = ospNewCamera("perspective");
    ospSetObject(orn->GetORenderer(),"camera", ospCamera);

    vtkCamera *cam = static_cast<vtkCamera *>(this->Renderable);
    ospSetf(ospCamera,"aspect", float(tiledSize[0])/float(tiledSize[1]));
    ospSetf(ospCamera,"fovy",cam->GetViewAngle());
    double *pos = cam->GetPosition();
    ospSet3f(ospCamera,"pos",pos[0], pos[1], pos[2]);
    ospSet3f(ospCamera,"up",
      cam->GetViewUp()[0], cam->GetViewUp()[1], cam->GetViewUp()[2]);
    double *dop = cam->GetDirectionOfProjection();
    ospSet3f(ospCamera,"dir", dop[0], dop[1], dop[2]);

    ospCommit(ospCamera);
    ospRelease(ospCamera);
    }
}
