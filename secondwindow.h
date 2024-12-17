#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QTextEdit>

namespace Ui {
class secondwindow;
}

class secondwindow : public QWidget
{
    Q_OBJECT

public:
    explicit secondwindow(QWidget *parent = nullptr);
    ~secondwindow();
    void setResultsText(const QString& text);

private:
    Ui::secondwindow *ui;
    QTableWidget *resultsTableMainTask;
    QTextEdit *resultsTextEdit;


};

#endif // SECONDWINDOW_H
