

#ifndef HARMONIC_PARAMETERISER_H
#define HARMONIC_PARAMETERISER_H

#include <vtkAppendArcLength.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCellLocator.h>
#include <vtkConnectivityFilter.h>
#include <vtkFeatureEdges.h>
#include <vtkFloatArray.h>
#include <vtkIdFilter.h>
#include <vtkMath.h>
#include <vtkParametricSpline.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPointLocator.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>
#include <vtkXMLPolyDataWriter.h>

#include <vcg/complex/algorithms/isotropic_remeshing.h>
#include "VCGDefinitions.h"

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <unordered_set>
#include <vector>


using Eigen::Vector2d;
using Eigen::Vector3d;

class HarmonicParameteriser {
   public:
    HarmonicParameteriser(vtkPolyData* maskMesh, vtkPoints* inputPts,
                   vtkPolyData* outputMesh);

    // Sample the flattened mesh to a regular grid
    void Sample(int uRes, int vRes);
    void DebugMesh(const std::string& folder);

   private:
    vtkSmartPointer<vtkPolyData> m_mask;
    vtkPoints* m_curvePts;
    vtkPolyData* m_output;

    vtkSmartPointer<vtkPolyData> m_flatMask;
    std::vector<Vector2d> m_UV;
    std::vector<vtkIdType> m_boundaryLoop;

    vtkIdType m_boundaryStartID = -1;
    vtkIdType m_boundaryDirectionID = -1;

    void InitialUV(vtkPolyData* flatMesh, std::vector<Vector2d>& UV);
    void ConvertVTKToVCG(vtkPolyData* polyData, MyMesh& vcgMesh);
    void ConvertVCGToVTK(MyMesh& vcgMesh, vtkPolyData* polyData);
    void ComputeHarmonicToPlane(vtkPolyData* mesh, vtkPolyData* outFlatMesh);
    bool ExtractSingleBoundaryLoop(vtkPolyData* meshBoundary,
                                   vtkPoints* curvePts,
                                   std::vector<vtkIdType>& loop);
};
#endif