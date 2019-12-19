
from paraview.simple import *
from paraview import coprocessing


#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# paraview version 5.5.1-4-g5bcfde9

#--------------------------------------------------------------
# Global screenshot output options
imageFileNamePadding=0
rescale_lookuptable=False


# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.5.1-4-g5bcfde9

      # ----------------------------------------------------------------
      # setup views used in the visualization
      # ----------------------------------------------------------------

      # trace generated using paraview version 5.5.1-4-g5bcfde9

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # get the material library
      materialLibrary1 = GetMaterialLibrary()

      # Create a new 'Render View'
      renderView1 = CreateView('RenderView')
      renderView1.ViewSize = [844, 547]
      renderView1.AxesGrid = 'GridAxes3DActor'
      renderView1.CenterOfRotation = [49.5, 1.5, 1.5]
      renderView1.StereoType = 0
      renderView1.CameraPosition = [99.01435982057743, -166.77452508433953, 78.16171964362127]
      renderView1.CameraFocalPoint = [49.5, 1.5, 1.5]
      renderView1.CameraViewUp = [-0.8411003544913982, -0.0010810132646687787, 0.5408780131275128]
      renderView1.CameraParallelScale = 49.545433694741234
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
          filename='image_%t.png', freq=1, fittoscreen=0, magnification=1, width=844, height=547, cinema={})
      renderView1.ViewTime = datadescription.GetTime()

      # ----------------------------------------------------------------
      # restore active view
      SetActiveView(renderView1)
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # create a new 'XML Partitioned Unstructured Grid Reader'
      # create a producer from a simulation input
      miniApp_ = coprocessor.CreateProducer(datadescription, 'input')

      # ----------------------------------------------------------------
      # setup the visualization in view 'renderView1'
      # ----------------------------------------------------------------

      # show data from miniApp_
      miniApp_Display = Show(miniApp_, renderView1)

      # get color transfer function/color map for 'pressure'
      pressureLUT = GetColorTransferFunction('pressure')
      pressureLUT.RGBPoints = [0.0, 0.231373, 0.298039, 0.752941, 1.5, 0.865003, 0.865003, 0.865003, 3.0, 0.705882, 0.0156863, 0.14902]
      pressureLUT.ScalarRangeInitialized = 1.0

      # get opacity transfer function/opacity map for 'pressure'
      pressurePWF = GetOpacityTransferFunction('pressure')
      pressurePWF.Points = [0.0, 0.0, 0.5, 0.0, 3.0, 1.0, 0.5, 0.0]
      pressurePWF.ScalarRangeInitialized = 1

      # trace defaults for the display properties.
      miniApp_Display.Representation = 'Surface'
      miniApp_Display.ColorArrayName = ['POINTS', 'pressure']
      miniApp_Display.LookupTable = pressureLUT
      miniApp_Display.OSPRayScaleArray = 'pressure'
      miniApp_Display.OSPRayScaleFunction = 'PiecewiseFunction'
      miniApp_Display.SelectOrientationVectors = 'None'
      miniApp_Display.ScaleFactor = 9.9
      miniApp_Display.SelectScaleArray = 'None'
      miniApp_Display.GlyphType = 'Arrow'
      miniApp_Display.GlyphTableIndexArray = 'None'
      # miniApp_Display.GaussianRadius = 0.495
      # miniApp_Display.SetScaleArray = ['POINTS', 'pressure']
      # miniApp_Display.ScaleTransferFunction = 'PiecewiseFunction'
      # miniApp_Display.OpacityArray = ['POINTS', 'pressure']
      # miniApp_Display.OpacityTransferFunction = 'PiecewiseFunction'
      # miniApp_Display.DataAxesGrid = 'GridAxesRepresentation'
      # miniApp_Display.SelectionCellLabelFontFile = ''
      # miniApp_Display.SelectionPointLabelFontFile = ''
      # miniApp_Display.PolarAxes = 'PolarAxesRepresentation'
      # miniApp_Display.ScalarOpacityFunction = pressurePWF
      # miniApp_Display.ScalarOpacityUnitDistance = 13.448699804284049

      # # init the 'PiecewiseFunction' selected for 'ScaleTransferFunction'
      # miniApp_Display.ScaleTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 3.0, 1.0, 0.5, 0.0]

      # # init the 'PiecewiseFunction' selected for 'OpacityTransferFunction'
      # miniApp_Display.OpacityTransferFunction.Points = [0.0, 0.0, 0.5, 0.0, 3.0, 1.0, 0.5, 0.0]

      # # init the 'GridAxesRepresentation' selected for 'DataAxesGrid'
      # miniApp_Display.DataAxesGrid.XTitleFontFile = ''
      # miniApp_Display.DataAxesGrid.YTitleFontFile = ''
      # miniApp_Display.DataAxesGrid.ZTitleFontFile = ''
      # miniApp_Display.DataAxesGrid.XLabelFontFile = ''
      # miniApp_Display.DataAxesGrid.YLabelFontFile = ''
      # miniApp_Display.DataAxesGrid.ZLabelFontFile = ''

      # # init the 'PolarAxesRepresentation' selected for 'PolarAxes'
      # miniApp_Display.PolarAxes.PolarAxisTitleFontFile = ''
      # miniApp_Display.PolarAxes.PolarAxisLabelFontFile = ''
      # miniApp_Display.PolarAxes.LastRadialAxisTextFontFile = ''
      # miniApp_Display.PolarAxes.SecondaryRadialAxesTextFontFile = ''

      # # setup the color legend parameters for each legend in this view

      # # get color legend/bar for pressureLUT in view renderView1
      # pressureLUTColorBar = GetScalarBar(pressureLUT, renderView1)
      # pressureLUTColorBar.Title = 'pressure'
      # pressureLUTColorBar.ComponentTitle = ''
      # pressureLUTColorBar.TitleFontFile = ''
      # pressureLUTColorBar.LabelFontFile = ''

      # # set color bar visibility
      # pressureLUTColorBar.Visibility = 1

      # # show color legend
      # miniApp_Display.SetScalarBarVisibility(renderView1, True)

      # ----------------------------------------------------------------
      # setup color maps and opacity mapes used in the visualization
      # note: the Get..() functions create a new object, if needed
      # ----------------------------------------------------------------

      # ----------------------------------------------------------------
      # finally, restore active source
      SetActiveSource(miniApp_)
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
