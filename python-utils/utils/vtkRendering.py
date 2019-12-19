import vtk
from vtk.util.colors import tomato
import base64
from vtk.util.numpy_support import vtk_to_numpy


def MakeLUTFromCTF(tableSize):
	'''
	Use array_index color transfer Function to generate the colors in the lookup table.
	See: http://www.vtk.org/doc/nightly/html/classvtkColorTransferFunction.html
	:param: tableSize - The table size
	:return: The lookup table.
	'''
	ctf = vtk.vtkColorTransferFunction()
	ctf.SetColorSpaceToDiverging()
	# blue to red.
	ctf.AddRGBPoint(0.0, 0.0, 0.0, 1.0)
	ctf.AddRGBPoint(0.5, 1.0, 0.5, 0.0)
	ctf.AddRGBPoint(1.0, 1.0, 0.0, 0.0)

	lut = vtk.vtkLookupTable()
	lut.SetNumberOfTableValues(tableSize)
	lut.Build()

	for i in range(0,tableSize):
		rgb = list(ctf.GetColor(float(i)/tableSize))+[1]
		lut.SetTableValue(i,rgb)

	return lut


def createImage(file_name, rendered_image, angleX, angleY, angleZ, scaleX, scaleY, scaleZ, vtkType, arrayName):

	colors = vtk.vtkNamedColors()

	# Read in data
	if vtkType == "vtStructuredGrid":
		reader = vtk.vtkXMLPStructuredGridReader()
	else:  
		reader = vtk.vtkXMLPUnstructuredGridReader()
	reader.SetFileName(file_name)
	reader.Update()  
	output = reader.GetOutput()
	scalar_range = output.GetScalarRange()


	# Get the number or arrays for each 
	number_of_point_arrays = output.GetPointData().GetNumberOfArrays()
	number_of_cell_arrays = output.GetCellData().GetNumberOfArrays()
	number_of_field_arrays = output.GetFieldData().GetNumberOfArrays()
	number_of_total_arrays = number_of_point_arrays + number_of_cell_arrays + number_of_field_arrays

	print("number_of_field_arrays ", number_of_field_arrays)
	print("number_of_cell_arrays ", number_of_cell_arrays)
	print("number_of_point_arrays ", number_of_point_arrays)
   

	# Find the field we are rendering
	data_array_name = []
	for i in range(number_of_point_arrays):
		data_array_name.append(output.GetPointData().GetArrayName(i))  

	for i in range(number_of_cell_arrays):
		data_array_name.append(output.GetCellData().GetArrayName(i))

	for i in range(number_of_field_arrays):
		data_array_name.append(output.GetFieldData().GetArrayName(i))


	array_index = 0
	for i in range(number_of_total_arrays):
		 if arrayName == data_array_name[i]:
			 array_index = i


	tableSize = 0
	if array_index < number_of_point_arrays:
		tableSize = output.GetPointData().GetArray(data_array_name[array_index]).GetNumberOfTuples()
		scalar_range = output.GetPointData().GetArray(data_array_name[array_index]).GetRange()
		colorData = output.GetPointData().GetArray(data_array_name[array_index])
		output.GetPointData().SetScalars(colorData)

	elif array_index < number_of_cell_arrays:
		tableSize = output.GetCellData().GetArray(data_array_name[array_index]).GetNumberOfTuples()
		scalar_range = output.GetCellData().GetArray(data_array_name[array_index]).GetRange()
		colorData = output.GetCellData().GetArray(data_array_name[array_index])
		output.GetCellData().SetScalars(colorData)
	else:
		tableSize = output.GetFieldData().GetArray(data_array_name[array_index]).GetNumberOfTuples()
		scalar_range = output.GetFieldData().GetArray(data_array_name[array_index]).GetRange()
		colorData = output.GetFieldData().GetArray(data_array_name[array_index])
		output.GetFieldData().SelectColorArray(data_array_name[array_index]) 


	lut1 = MakeLUTFromCTF(tableSize)   

	
	# Create the mapper that corresponds the objects of the vtk.vtk file
	# into graphics elements
	mapper = vtk.vtkDataSetMapper()
	mapper.SetInputConnection(reader.GetOutputPort())
	mapper.SetScalarRange(scalar_range)


	if array_index < number_of_point_arrays:
	   mapper.SetScalarModeToUsePointData() 
	elif array_index < number_of_cell_arrays:
		mapper.SetScalarModeToUseCellData()
	else:
		mapper.SetScalarModeToUseFieldData()

	mapper.SetLookupTable(lut1)
	mapper.Update()


	# Create the Actor
	actor = vtk.vtkActor()
	actor.SetMapper(mapper)
	actor.RotateX(angleX)
	actor.RotateY(angleY)
	actor.RotateZ(angleZ)
	
	actor.SetScale(scaleX, scaleY, scaleZ)
	actor.GetProperty().EdgeVisibilityOn()
	actor.GetProperty().SetLineWidth(2.0)
		

	backface = vtk.vtkProperty()
	backface.SetColor(colors.GetColor3d("tomato"))
	actor.SetBackfaceProperty(backface)

	# Create the Renderer
	renderer = vtk.vtkRenderer()
	renderer.AddActor(actor)
	renderer.SetBackground(1, 1, 1)  # Set background to white


	# Create the RendererWindow
	renderer_window = vtk.vtkRenderWindow()
	renderer_window.AddRenderer(renderer)
	renderer_window.SetOffScreenRendering(1)
	renderer_window.SetSize(750,750)

	 # create array_index renderwindowinteractor
	interactor = vtk.vtkRenderWindowInteractor()
	interactor.SetRenderWindow(renderer_window)


	# create the scalar_bar
	scalar_bar = vtk.vtkScalarBarActor()
	scalar_bar.SetLookupTable(lut1)

	# create the scalar_bar_widget
	scalar_bar_widget = vtk.vtkScalarBarWidget()
	scalar_bar_widget.SetInteractor(interactor)
	scalar_bar_widget.SetScalarBarActor(scalar_bar)
	scalar_bar_widget.On()


	#interactor.Initialize()
	renderer_window.Render()

	# screenshot code:
	w2if = vtk.vtkWindowToImageFilter()
	w2if.SetInput(renderer_window)
	w2if.Update()

	writer = vtk.vtkPNGWriter()
	writer.SetFileName(rendered_image)
	writer.SetInputConnection(w2if.GetOutputPort())
	writer.Write()

	# Return the rendered image
	encoded_image = base64.b64encode(open(rendered_image, 'rb').read())
	return encoded_image


rendered_image = "/home/pascal/Desktop/rendered1.png"

#filename = "/home/pascal/projects/InSitu_vtkWriters/build/testStructuredMPI_0.pvts"
#vtkType = "vtStructuredGrid"

filename = "/home/pascal/projects/InSitu_vtkWriters/build/miniAppUns_3.pvtu"
vtkType = "vtUntructuredGrid"

scalar_name = "cell-data-fact"

encoded_image = createImage(filename,rendered_image, 90, 30, 60, 1, 1, 1, vtkType, scalar_name)