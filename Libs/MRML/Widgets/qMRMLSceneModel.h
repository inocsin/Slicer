/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qMRMLSceneModel_h
#define __qMRMLSceneModel_h

// Qt includes
#include <QStandardItemModel>

// CTK includes
#include <ctkPimpl.h>
#include <ctkVTKObject.h>

// qMRML includes
#include "qMRMLWidgetsExport.h"

class vtkMRMLNode;
class vtkMRMLScene;
class QAction;

class qMRMLSceneModelPrivate;

//------------------------------------------------------------------------------
///
/// qMRMLSceneModel has at least 1 column with the scene as a top level item.
/// See below an example of model when nameColumn = 0 (default) and idColumn = 1
/// with 1 postItem ("Add new node"):
///
/// \verbatim
///    Column 0           Column 1
///
///  - Scene
///    |- ViewNode        vtkMRMLViewNode1
///    |- CameraNode      vtkMRMLCameraNode1
///    ...
///    |- my_volume.nrrd  vtkMRMLScalarVolumeNode1
///    |- Add new node
/// \endverbatim
/// More columns can be added by setting the column index of the differen
/// columns: name, id, checkable, visibility, toolTip, extraItem and the ones
/// defined in subclasses.
/// Subclasses must reimplement maxColumnId if they add new column properties
class QMRML_WIDGETS_EXPORT qMRMLSceneModel : public QStandardItemModel
{
  Q_OBJECT
  QVTK_OBJECT

  /// This property controls whether to observe or not the modified event of
  /// the node and update the node item data accordingly.
  /// It can be useful when the modified property is displayed
  /// (name, id, visibility...)
  /// True by default
  Q_PROPERTY (bool listenNodeModifiedEvent READ listenNodeModifiedEvent WRITE setListenNodeModifiedEvent)

  /// Control wether the model actively listens to the scene.
  /// If LazyUpdate is true, the model ignores added node events when the
  /// scene is importing/restoring, but synchronize with the scene once its
  /// imported/restored.
  Q_PROPERTY (bool lazyUpdate READ lazyUpdate WRITE setLazyUpdate)

  /// Control in which column vtkMRMLNode names are displayed (Qt::DisplayRole).
  /// A value of -1 hides it. First column (0) by default.
  /// If no property is set in a column, nothing is displayed.
  Q_PROPERTY (int nameColumn READ nameColumn WRITE setNameColumn)
  /// Control in which column vtkMRMLNode IDs are displayed (Qt::DisplayRole).
  /// A value of -1 hides it. Hidden by default (value of -1)
  Q_PROPERTY (int idColumn READ idColumn WRITE setIDColumn)
  /// Control in which column vtkMRMLNode::Selected are displayed (Qt::CheckStateRole).
  /// A value of -1 hides it. Hidden by default (value of -1).
  Q_PROPERTY (int checkableColumn READ checkableColumn WRITE setCheckableColumn)
  /// Control in which column vtkMRMLNode::Visibility are displayed (Qt::DecorationRole).
  /// A value of -1 hides it. Hidden by default (value of -1).
  Q_PROPERTY (int visibilityColumn READ visibilityColumn WRITE setVisibilityColumn)
  /// Control in which column tooltips are displayed (Qt::ToolTipRole).
  /// A value of -1 hides it. Hidden by default (value of -1).
  Q_PROPERTY (int toolTipNameColumn READ toolTipNameColumn WRITE setToolTipNameColumn)
  /// Control in which column the extra items are displayed
  /// A value of -1 hides it (not tested). 0 by default
  Q_PROPERTY( int extraItemColumn READ extraItemColumn WRITE setExtraItemColumn)
public:
  typedef QStandardItemModel Superclass;
  qMRMLSceneModel(QObject *parent=0);
  virtual ~qMRMLSceneModel();

  enum ItemDataRole{
    UIDRole = Qt::UserRole + 1,
    PointerRole,
    ExtraItemsRole,
    VisibilityRole,
    LastRole
    };

  /// 0 by default
  virtual void setMRMLScene(vtkMRMLScene* scene);
  vtkMRMLScene* mrmlScene()const;

  /// 0 until a valid scene is set
  QStandardItem* mrmlSceneItem()const;

  /// invalid until a valid scene is set
  QModelIndex mrmlSceneIndex()const;

  /// Return the vtkMRMLNode associated to the node index.
  /// 0 if the node index is not a MRML node (i.e. vtkMRMLScene, extra item...)
  inline vtkMRMLNode* mrmlNodeFromIndex(const QModelIndex &nodeIndex)const;
  vtkMRMLNode* mrmlNodeFromItem(QStandardItem* nodeItem)const;
  QModelIndex indexFromNode(vtkMRMLNode* node, int column = 0)const;
  // Utility function
  QStandardItem* itemFromNode(vtkMRMLNode* node, int column = 0)const;
  // Return all the QModelIndexes (all the columns) for a given node
  QModelIndexList indexes(vtkMRMLNode* node)const;

  /// Option that activates the expensive listening of the vtkMRMLNode Modified
  /// events. When listening, the signal itemDataChanged() is fired when a
  /// vtkMRMLNode is modified.
  void setListenNodeModifiedEvent(bool listen);
  bool listenNodeModifiedEvent()const;

  bool lazyUpdate()const;
  void setLazyUpdate(bool lazy);

  int nameColumn()const;
  void setNameColumn(int column);

  int idColumn()const;
  void setIDColumn(int column);

  int checkableColumn()const;
  void setCheckableColumn(int column);

  int visibilityColumn()const;
  void setVisibilityColumn(int column);

  int toolTipNameColumn()const;
  void setToolTipNameColumn(int column);

  int extraItemColumn()const;
  void setExtraItemColumn(int column);

  /// Extra items that are prepended to the node list
  /// Warning, setPreItems() resets the model, the currently selected item is lost
  void setPreItems(const QStringList& extraItems, QStandardItem* parent);
  QStringList preItems(QStandardItem* parent)const;

  /// Extra items that are appended to the node list
  /// Warning, setPostItems() resets the model, the currently selected item is lost
  void setPostItems(const QStringList& extraItems, QStandardItem* parent);
  QStringList postItems(QStandardItem* parent)const;

  /// Doesn't support drop actions, scene model subclasses can support drop
  /// actions though.
  virtual Qt::DropActions supportedDropActions()const;
  virtual QMimeData* mimeData(const QModelIndexList& indexes)const;
  virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                            int row, int column, const QModelIndex &parent);

  /// Must be reimplemented in derived classes
  /// Returns 0 (scene is not a node) in qMRMLSceneModel
  virtual vtkMRMLNode* parentNode(vtkMRMLNode* node)const;
  /// Must be reimplemented in derived classes
  virtual int          nodeIndex(vtkMRMLNode* node)const;
  /// fast function that only check the type of the node to know if it can be a child.
  virtual bool         canBeAChild(vtkMRMLNode* node)const;
  /// Must be reimplemented in derived classes
  /// Returns false in qMRMLSceneModel
  virtual bool         canBeAParent(vtkMRMLNode* node)const;
  /// Must be reimplemented in derived classes.
  /// Doesn't reparent and returns false by qMRMLSceneModel
  virtual bool         reparent(vtkMRMLNode* node, vtkMRMLNode* newParent);

protected slots:

  virtual void onMRMLSceneNodeAboutToBeAdded(vtkMRMLScene* scene, vtkMRMLNode* node);
  virtual void onMRMLSceneNodeAboutToBeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node);
  virtual void onMRMLSceneNodeAdded(vtkMRMLScene* scene, vtkMRMLNode* node);
  virtual void onMRMLSceneNodeRemoved(vtkMRMLScene* scene, vtkMRMLNode* node);

  virtual void onMRMLSceneAboutToBeImported(vtkMRMLScene* scene);
  virtual void onMRMLSceneImported(vtkMRMLScene* scene);
  virtual void onMRMLSceneAboutToBeClosed(vtkMRMLScene* scene);
  virtual void onMRMLSceneClosed(vtkMRMLScene* scene);

  void onMRMLSceneDeleted(vtkObject* scene);

  void onMRMLNodeModified(vtkObject* node);
  /// The node has its ID changed. The scene model needs to update the UIDRole
  /// associated with the node in order to keep being in sync.
  void onMRMLNodeIDChanged(vtkObject* node, void* callData);
  void onItemChanged(QStandardItem * item);
  void delayedItemChanged();

  /// Recompute the number of columns in the model.
  /// To be called when a XXXColumn is set.
  /// Needs maxColumnId() to be reimplemented in subclasses
  void updateColumnCount();

signals :
  /// This signal is sent when a user is about to reparent a Node by
  /// a drag and drop
  void aboutToReparentByDragAndDrop(vtkMRMLNode* node, vtkMRMLNode* newParent);

  ///  This signal is sent after a user draged and droped a Node in the
  /// qMRMLTreeView
  void reparentedByDragAndDrop(vtkMRMLNode* node, vtkMRMLNode* newParent);

protected:

  qMRMLSceneModel(qMRMLSceneModelPrivate* pimpl, QObject *parent=0);

  virtual void updateScene();
  virtual void populateScene();
  virtual QStandardItem* insertNode(vtkMRMLNode* node);
  virtual QStandardItem* insertNode(vtkMRMLNode* node, QStandardItem* parent, int row = -1);
  virtual void observeNode(vtkMRMLNode* node);

  virtual bool isANode(const QStandardItem* item)const;
  virtual QFlags<Qt::ItemFlag> nodeFlags(vtkMRMLNode* node, int column)const;

  /// You shouldn't need to call this function.
  void updateNodeItems();

  /// Generic function that updates the item data and flags from the node.
  /// You probably want to reimplement updateItemDataFromNode() instead.
  /// \sa updateNodeFromItemData, updateNodeFromItem, updateItemDataFromNode,
  /// nodeFlags
  virtual void updateItemFromNode(QStandardItem* item, vtkMRMLNode* node, int column);

  /// To reimplement if you want custom display of the QStandardItem from
  /// the MRML node.
  /// Example:
  /// \code
  /// void MySceneModel::updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column)
  /// if (column == 3)
  ///   {
  ///   item->setText(node->GetFoo());
  ///   return;
  ///   }
  /// this->Superclass::updateItemDataFromNode(item, node, column);
  /// }
  /// \endcode
  /// \sa updateNodeFromItemData, updateNodeFromItem, updateItemFromNode,
  /// nodeFlags
  virtual void updateItemDataFromNode(QStandardItem* item, vtkMRMLNode* node, int column);
  /// Generic function that updates the node from the item data and flags.
  /// You probably want to reimplement updateNodeFromItemData() instead.
  /// \sa updateItemDataFromNode, updateNodeFromItemData, updateItemFromNode,
  /// nodeFlags
  virtual void updateNodeFromItem(vtkMRMLNode* node, QStandardItem* item);
  /// To reimplement if you want to propagate user changes on QStandardItem
  /// into the MRML node.
  /// Example:
  /// <code>
  /// void MySceneModel::updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item)
  /// if (column == 3)
  ///   {
  ///   node->SetFoo(item->text());
  ///   return;
  ///   }
  /// this->Superclass::updateNodeFromItemData(node, item);
  /// }
  /// </code>
  /// \sa updateItemFromNode, updateNodeFromItemData, updateItemFromNode,
  /// nodeFlags
  virtual void updateNodeFromItemData(vtkMRMLNode* node, QStandardItem* item);

  /// Update the items associated with the node and uid.
  void updateNodeItems(vtkMRMLNode* node, const QString& uid);

  static void onMRMLSceneEvent(vtkObject* vtk_obj, unsigned long event,
                               void* client_data, void* call_data);

  /// Must be reimplemented in subclasses that add new column types
  virtual int maxColumnId()const;
protected:
  QScopedPointer<qMRMLSceneModelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qMRMLSceneModel);
  Q_DISABLE_COPY(qMRMLSceneModel);
};

void printStandardItem(QStandardItem* item, const QString& offset);

// -----------------------------------------------------------------------------
vtkMRMLNode* qMRMLSceneModel::mrmlNodeFromIndex(const QModelIndex &nodeIndex)const
{
  return this->mrmlNodeFromItem(this->itemFromIndex(nodeIndex));
}

#endif
