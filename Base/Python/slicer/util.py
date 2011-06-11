
#
# General
#

EXIT_SUCCESS = 0
EXIT_FAILURE = 1

def quit():
  exit(EXIT_SUCCESS)

def exit(status=EXIT_SUCCESS):
  from slicer import app
  app.exit(status)

def restart():
  from slicer import app
  app.restart()

#
# UI
#

def lookupTopLevelWidget(objectName, verbose = True):
  """Loop over all top level widget associated with 'slicer.app' and 
  return the one matching 'objectName'"""
  from slicer import app
  for w in app.topLevelWidgets():
    if w.objectName == objectName: return w
  if verbose:
    print "Failed to obtain reference to '%s'" % objectName
  return None

def mainWindow(verbose = True):
  return lookupTopLevelWidget('qSlicerMainWindow', verbose)

def pythonShell(verbose = True):
  return lookupTopLevelWidget('pythonConsole', verbose)

def showStatusMessage(message, duration = 0):
  mw = mainWindow(verbose=False)
  if mw:
    mw.statusBar().showMessage(message, duration)

def findChildren(widget=None,name=""):
  """ return a list of child widgets that match the passed name """
  # TODO: figure out why the native QWidget.findChildren method
  # does not seem to work from PythonQt
  if not widget:
    widget = mainWindow()
  children = []
  parents = [widget]
  while parents != []:
    p = parents.pop()
    parents += p.children()
    if p.name == name:
      children.append(p)
  return children

#
# IO
#

def loadColorTable(filename):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.ColorTableFile
  properties = {}
  return app.coreIOManager().loadNodes(filetype, properties)

def loadDTI(filename):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.DTIFile
  properties = {}
  return app.coreIOManager().loadNodes(filetype, properties)

def loadFiducialList(filename):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.FiducialListFile
  properties = {'fileName':filename}
  return app.coreIOManager().loadNodes(filetype, properties)

def loadModel(filename):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.ModelFile
  properties = {'fileName':filename}
  return app.coreIOManager().loadNodes(filetype, properties)

def loadScalarOverlay(filename):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.ScalarOverlayFile
  properties = {'fileName':filename}
  return app.coreIOManager().loadNodes(filetype, properties)

def loadScene(filename, clear = True):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.SceneFile
  properties = {'fileName':filename}
  return app.coreIOManager().loadNodes(filetype, properties)

def loadTransform(filename):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.TransformFile
  properties = {'fileName':filename}
  return app.coreIOManager().loadNodes(filetype, properties)

def loadVolume(filename):
  from slicer import app, qSlicerIO
  filetype = qSlicerIO.VolumeFile
  properties = {'fileName':filename}
  return app.coreIOManager().loadNodes(filetype, properties)

#
# Module
#

def selectModule(module):
  moduleName = module
  if not isinstance(module, basestring):
    moduleName = module.name
  w = mainWindow()
  if not w: return
  w.moduleSelector().selectModule(moduleName)

def moduleNames():
  from slicer import app
  return app.moduleManager().factoryManager().moduleNames()

def getModule(moduleName):
  from slicer import app
  module = app.moduleManager().module(moduleName);
  if not module:
    print "Could not find module with name '%s" % moduleName
    return None
  return module

def getModuleGui(module):
  if isinstance(module, basestring):
    module = getModule(module)
  if not module:
    return None
  widgetRepr = module.widgetRepresentation()
  if not widgetRepr:
    print "Could not find module widget representation with name '%s" % moduleName
    return None
  return widgetRepr

#
# MRML
#

def getNodes(pattern = ""):
    """Return a dictionary of nodes where the name or id matches the 'pattern'.
    Providing an empty 'pattern' string will return all nodes.
    """
    import slicer
    nodes = {}
    scene = slicer.mrmlScene
    count = scene.GetNumberOfNodes()
    for idx in range(count):
      node = scene.GetNthNode(idx)
      name = node.GetName()
      id = node.GetID()
      if name.find(pattern) >= 0 or id.find(pattern) >= 0:
        nodes[node.GetName()] = node
    return nodes

def getNode(pattern = "", index = 0):
    """Return the indexth node where name or id matches 'pattern'.
    Providing an empty 'pattern' string will return all nodes.
    """
    nodes = getNodes(pattern)
    try:
      if nodes.keys():
        return nodes[nodes.keys()[index]]
    except IndexError:
      return None
