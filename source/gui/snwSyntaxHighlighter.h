#ifndef SNWSYNTAXHIGHLIGHTER_H_
#define SNWSYNTAXHIGHLIGHTER_H_

#include <QtGui>

namespace snw
{

/**Provides syntax highlighting for sonowand log files 
 * in snw::StatusWindow..
 *
 * Adapted for CustusX use: Simple xml highlighter.
 */
class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
	SyntaxHighlighter(QTextDocument* parent);
	virtual void highlightBlock(const QString &text);
private:
	void highlightTimestamp(const QString &text);
	void applyFormat(const QString &text, const QTextCharFormat& format, const QString pattern);
};

}

#endif /*SNWSYNTAXHIGHLIGHTER_H_*/
