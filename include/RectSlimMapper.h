#ifndef RECT_SLIM_MAPPER_H
#define RECT_SLIM_MAPPER_H

#include <vcg/complex/algorithms/isotropic_remeshing.h>
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
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkStripper.h>
#include <vtkThinPlateSplineTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangle.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPointLocator.h>
#include <vtkPolyLine.h>

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

#include "VCGDefinitions.h"


using Eigen::Vector2d;
using Eigen::Vector3d;

class RectSlimMapper {
   public:
    RectSlimMapper(vtkPolyData* maskMesh, vtkPoints* inputPts,
                   vtkPolyData* outputMesh);

    // Sample the flattened mesh to a regular grid
    void Sample(int uRes, int vRes);
    void DebugMesh(const std::string& folder);
    // Access UV coordinates
    const std::vector<Vector2d>& GetUV() const { return m_UV; }

   private:
    vtkSmartPointer<vtkPolyData> m_mask;
    vtkPoints* m_curvePts;
    vtkPolyData* m_output;
    vtkSmartPointer<vtkPlaneSource> m_plane;
    vtkSmartPointer<vtkPolyData> m_flatMask;
    vtkSmartPointer<vtkIdList> m_meshBoundaryID;
    

    std::vector<Vector2d> m_UV;
    std::vector<int> m_isBoundary;
    std::vector<vtkIdType> m_boundaryLoop;

    vtkIdType m_boundaryStartID = -1;
    vtkIdType m_boundaryDirectionID = -1;

    // Parameterization: project onto input plane
    void ParameterizeToPlane();
    void GetPlaneBoundaryPoints(vtkPolyData* plane, vtkPoints* boundaryPoints);
    void MakePlaneAndDeform();
    
    void InitialUVFromPlane(vtkPolyData* mesh, const double origin[3],
                            const double p1[3], const double p2[3],
                            std::vector<Vector2d>& UV);
    void ProjectOnBoundary(vtkPolyData* flatMask, vtkPolyData* plane);
    void ProjectOnMesh(vtkPolyData* Poly, vtkPolyData* target,
                       std::vector<int>* ids = nullptr);
    void ConvertVTKToVCG(vtkPolyData* polyData, MyMesh& vcgMesh);
    void ConvertVCGToVTK(MyMesh& vcgMesh, vtkPolyData* polyData);
    void ComputeHarmonicToPlane(vtkPolyData* mesh, vtkPolyData* outFlatMesh);
    bool ExtractSingleBoundaryLoop(
        vtkPolyData* meshBoundary, std::vector<vtkIdType>& loop,
        vtkIdType startVertex = -1, vtkIdType dirStartVertex = -1,
        vtkIdType dirEndVertex = -1);
    void GetBoundaryDirection();
};
#endif