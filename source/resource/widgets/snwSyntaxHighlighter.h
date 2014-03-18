#ifndef SNWSYNTAXHIGHLIGHTER_H_
#define SNWSYNTAXHIGHLIGHTER_H_

#include <QSyntaxHighlighter>

namespace snw
{

/**\brief Provides syntax highlighting for sonowand log files.
 *
 * Adapted for CustusX use: Simple xml highlighter.
 *
 * \ingroup cx_resource_widgets
 */
class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
	SyntaxHighlighter(QTextDocument* parent);
	virtual void highlightBlock(const QString &text);
	virtual ~SyntaxHighlighter() {}
private:
	void highlightTimestamp(const QString &text);
	void applyFormat(const QString &text, const QTextCharFormat& format, const QString pattern);
};

}

#endif /*SNWSYNTAXHIGHLIGHTER_H_*/
