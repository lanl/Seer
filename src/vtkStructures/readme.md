# File formats of VTK
 * ImageData        .vti, Serial | PImageData        .pvti, Parallel | vtkImageData          (structured)
 * PolyData         .vtp, Serial | PPolyData         .pvtp, Parallel | vtkPolyData         (unstructured)
 * RectilinearGrid  .vtr, Serial | PRectilinearGrid  .pvtr, Parallel | vtkRectilinearGrid    (structured)
 * StructuredGrid   .vts, Serial | PStructuredGrid   .pvts, Parallel | vtkStructuredGrid     (structured)
 * UnstructuredGrid .vtu, Serial | PUnstructuredGrid .pvtu, Parallel | vtkUnstructuredGrid (unstructured)

Refrence: https://www.vtk.org/img/file-formats.pdf
Example: https://www.vtk.org/Wiki/VTK/Examples/Cxx, https://lorensen.github.io/VTKExamples/site/Cxx/
Catalyst examples: https://github.com/Kitware/ParaView/tree/master/Examples/Catalyst


## Implemented
  * UnstructuredGrid .vtu, Serial | PUnstructuredGrid .pvtu, Parallel | vtkUnstructuredGrid (unstructured)
    * used for points, like in cosmology

  * StructuredGrid   .vts, Serial | PStructuredGrid   .pvts, Parallel | vtkStructuredGrid     (structured)
  	* used for grids, Eulerian simulation

  * RectilinearGrid  .vtr, Serial | PRectilinearGrid  .pvtr, Parallel | vtkRectilinearGrid    (structured)



# VTK has three types of data (https://www.vtk.org/Wiki/VTK/Tutorials/DataStorage):
- FieldData
	Arrays attached to the FieldData of a dataset describe global properties of the data. That is, if you want to save the time at which the data were recorded, you would put that value in the FieldData. If you wanted to name the data set, you would also put that in the FieldData. There are no restrictions about the length of arrays that are added to the FieldData.

- CellData
	If you have a dataset which has values/data at every cell (e.g. every triangle in a mesh has a specified color), you should add these data to the CellData of the dataset. The length of arrays added to CellData must equal the number of cells in the dataset (dataset->GetNumberOfCells()).

- PointData 
	If you have a dataset which has values/data at every point, you'll want to use PointData.