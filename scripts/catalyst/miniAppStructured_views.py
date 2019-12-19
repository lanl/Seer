
from paraview.simple import *
from paraview import coprocessing


#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# paraview version 5.5.2

#--------------------------------------------------------------
# Global screenshot output options
imageFileNamePadding=0
rescale_lookuptable=False


# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.5.2

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      # trace generated using paraview version 5.5.2

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # get the material library
      materialLibrary1 = GetMaterialLibrary()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [3201, 1660]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [2.0, 2.5, 3.5]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [13.222621127002732, 10.481099271175227, 15.592948447115667]
      renderView1.CameraFocalPoint = [1.9999999999999998, 2.5, 3.4999999999999987]
      renderView1.CameraViewUp = [-0.3391197317582044, 0.898618610957035, -0.27835660504807963]
      renderView1.CameraParallelScale = 4.743416490252569
      renderView1.Background = [0.0, 0.0, 0.0]
      renderView1.OSPRayMaterialLibrary = materialLibrary1

      # init the 'GridAxes3DActor' selected for 'AxesGrid'
      renderView1.AxesGrid.XTitleFontFile = ''
      renderView1.AxesGrid.YTitleFontFile = ''
      renderView1.AxesGrid.ZTitleFontFile = ''
      renderView1.AxesGrid.XLabelFontFile = ''
      renderView1.AxesGrid.YLabelFontFile = ''
      renderView1.AxesGrid.ZLabelFontFile = ''

      # register the view with coprocessor
      # and provide it with information such as the filename to use,
      # how frequently to write the images, etc.
      coprocessor.RegisterView(renderView1,
          filename='image_%t.png', freq=1, fittoscreen=0, magnification=1, width=3201, height=1660, cinema={})
      renderView1.ViewTime = datadescription.GetTime()

      # ----------------------------------------------------------------
      # restore active view
      SetActiveView(renderView1)
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'XML Partitioned Structured Grid Reader'
      # create a producer from a simulation input
      #testStructuredMPI_ = coprocessor.CreateProducer(datadescription, 'testStructuredMPI_*')
      testStructuredMPI_ = coprocessor.CreateProducer(datadescription, 'input')

      # create a new 'XML Partitioned Unstructured Grid Reader'
      #miniApp_ = XMLPartitionedUnstructuredGridReader(FileName=['/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_0.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_1.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_2.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_3.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_4.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_5.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_6.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_7.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_8.pvtu', '/home/pascal/projects/InSitu_vtkWriters/buildII/miniApp_9.pvtu'])
      #miniApp_.PointArrayStatus = ['pressure']

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from testStructuredMPI_
      testStructuredMPI_Display = Show(testStructuredMPI_, renderView1)

      # get color transfer function/color map for 'celldatascalar'
      celldatascalarLUT = GetColorTransferFunction('celldatascalar')
      celldatascalarLUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 1.725, 0.865003, 0.865003, 0.865003, 3.45, 0.705882, 0.0156863, 0.14902]
      celldatascalarLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'celldatascalar'
      celldatascalarPWF = GetOpacityTransferFunction('celldatascalar')
      celldatascalarPWF.Points = [0.0, 0.0, 0.5, 0.0, 3.45, 1.0, 0.5, 0.0]
      celldatascalarPWF.ScalarRangeInitialized = 1

      # trace defaults for the display properties.
      testStructuredMPI_Display.Representation = 'Surface With Edges'
      testStructuredMPI_Display.ColorArrayName = ['CELLS', 'cell-data-scalar']
      testStructuredMPI_Display.LookupTable = celldatascalarLUT
      testStructuredMPI_Display.OSPRayScaleArray = 'vert-data'
      testStructuredMPI_Display.OSPRayScaleFunction = 'PiecewiseFunction'
      testStructuredMPI_Display.SelectOrientationVectors = 'None'
      testStructuredMPI_Display.ScaleFactor = 0.7000000000000001
      testStructuredMPI_Display.SelectScaleArray = 'None'
      testStructuredMPI_Display.GlyphType = 'Arrow'
      testStructuredMPI_Display.GlyphTableIndexArray = 'None'


      # show color legend
      testStructuredMPI_Display.SetScalarBarVisibility(renderView1, True)

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(testStructuredMPI_)
      # ----------------------------------------------------------------
    return Pipeline()

  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [1]}
  coprocessor.SetUpdateFrequencies(freqs)
  return coprocessor


#--------------------------------------------------------------
# Global variable that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView and the update frequency
coprocessor.EnableLiveVisualization(False, 1)

# ---------------------- Data Selection method ----------------------

def RequestDataDescription(datadescription):
    "Callback to populate the request for current timestep"
    global coprocessor
    if datadescription.GetForceOutput() == True:
        # We are just going to request all fields and meshes from the simulation
        # code/adaptor.
        for i in range(datadescription.GetNumberOfInputDescriptions()):
            datadescription.GetInputDescription(i).AllFieldsOn()
            datadescription.GetInputDescription(i).GenerateMeshOn()
        return

    # setup requests for all inputs based on the requirements of the
    # pipeline.
    coprocessor.LoadRequestedData(datadescription)

# ------------------------ Processing method ------------------------

def DoCoProcessing(datadescription):
    "Callback to do co-processing for current timestep"
    global coprocessor

    # Update the coprocessor by providing it the newly generated simulation data.
    # If the pipeline hasn't been setup yet, this will setup the pipeline.
    coprocessor.UpdateProducers(datadescription)

    # Write output data, if appropriate.
    coprocessor.WriteData(datadescription);

    # Write image capture (Last arg: rescale lookup table), if appropriate.
    coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
