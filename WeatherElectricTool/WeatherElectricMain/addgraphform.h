#ifndef ADDGRAPHFORM_H
#define ADDGRAPHFORM_H

#include "dataconnector.h"

#include <QWidget>

namespace Ui {
class AddGraphForm;
}

class MainWindow;

class AddGraphForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddGraphForm(MainWindow* mainWindow, QWidget* parent = nullptr);
    ~AddGraphForm();

private slots:
    void addGraphToList(const DataSourceDetails& sourceDetails);

    void on_searchLineEdit_textEdited(const QString &arg1);

private:

    Ui::AddGraphForm* ui_;
    MainWindow* mainWindow_;
    QLayout* infoBoxLayout_;
};

#endif // ADDGRAPHFORM_H
