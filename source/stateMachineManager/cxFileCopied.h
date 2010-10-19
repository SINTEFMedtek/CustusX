#ifndef CXFILECOPIED_H_
#define CXFILECOPIED_H_

#include <string>
#include <QObject>
#include "sscForwardDeclarations.h"
#include "sscData.h"

namespace cx
{

/** \class FileCopied
 *
 * \brief This is a helper class for MainWindow::importDataSlot().
 * Contain a slot that checks if a file is finished copying and updates the
 * ssc:Image object
 *
 * \date Feb 8, 2010
 * \author Ole Vegard Solberg, SINTEF
 */
class FileCopied : public QObject
{
  Q_OBJECT
public:
  FileCopied(const QString& absolutefilePath,
             const QString& relativefilePath,
             ssc::DataPtr data);///< Creates the object

signals:
  void fileCopiedCorrectly();///< Sends a signal when the copy is verified
protected:
  QString mFilePath;
  QString mRelativeFilePath;
  ssc::DataPtr mData;
public slots:
  void areFileCopiedSlot();///< Checks if the file is finished copying and updates the ssc::Image
};

} // namespace cx


#endif /* CXFILECOPIED_H_ */
