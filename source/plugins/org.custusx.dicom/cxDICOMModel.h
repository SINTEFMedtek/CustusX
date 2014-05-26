/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __DICOMModel_h
#define __DICOMModel_h

// Qt includes 
#include <QAbstractItemModel>
#include <QMetaType>
#include <QSqlDatabase>
#include <QStringList>

namespace cx
{

class DICOMModelPrivate;

/// \ingroup DICOM_Core
class DICOMModel
//  : public QStandardItemModel
  : public QAbstractItemModel
{
  Q_OBJECT
  typedef QAbstractItemModel Superclass;
  Q_ENUMS(IndexType)
  /// startLevel contains the hierarchy depth the model contains
  Q_PROPERTY(IndexType endLevel READ endLevel WRITE setEndLevel);
public:

  enum {
    UIDRole = Qt::UserRole,
    TypeRole
  };

  enum IndexType{
    RootType,
    PatientType,
    StudyType,
    SeriesType,
    ImageType
  };

  explicit DICOMModel(QObject* parent = 0);
  virtual ~DICOMModel();

  void setDatabase(const QSqlDatabase& dataBase);
  void setDatabase(const QSqlDatabase& dataBase, const QMap<QString,QVariant>& parameters);

  /// Set it before populating the model
  DICOMModel::IndexType endLevel()const;
  void setEndLevel(DICOMModel::IndexType level);

  virtual bool canFetchMore ( const QModelIndex & parent ) const;
  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual void fetchMore ( const QModelIndex & parent );
  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
  // can return true even if rowCount returns 0, you should use canFetchMore/fetchMore to populate
  // the children.
  virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const;
  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex parent ( const QModelIndex & index ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
  virtual bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );
  // Sorting resets the model because fetched/unfetched items could disappear/appear respectively.
  virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
public Q_SLOTS:
  virtual void reset();
protected:
  QScopedPointer<DICOMModelPrivate> d_ptr;

  bool setChildData(const QModelIndex &index, const QVariant &value, int role);
  bool setParentData(const QModelIndex &index, const QVariant &value, int role);

private:
  Q_DECLARE_PRIVATE(DICOMModel);
  Q_DISABLE_COPY(DICOMModel);
};

} // namespace cx

Q_DECLARE_METATYPE(cx::DICOMModel::IndexType)

#endif
